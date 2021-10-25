/*
 * Copyright 2012 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Ben Skeggs
 */

#include "nvc0.h"
#include "fuc/hubnve0.fuc.h"
#include "fuc/gpcnve0.fuc.h"

/*******************************************************************************
 * Graphics object classes
 ******************************************************************************/

static struct nouveau_oclass
nve0_graph_sclass[] = {
	{ 0x902d, &nouveau_object_ofuncs },
	{ 0xa040, &nouveau_object_ofuncs },
	{ 0xa097, &nouveau_object_ofuncs },
	{ 0xa0c0, &nouveau_object_ofuncs },
	{ 0xa0b5, &nouveau_object_ofuncs },
	{}
};

/*******************************************************************************
 * PGRAPH context
 ******************************************************************************/

static struct nouveau_oclass
nve0_graph_cclass = {
	.handle = NV_ENGCTX(GR, 0xe0),
	.ofuncs = &(struct nouveau_ofuncs) {
		.ctor = nvc0_graph_context_ctor,
		.dtor = nvc0_graph_context_dtor,
		.init = _nouveau_graph_context_init,
		.fini = _nouveau_graph_context_fini,
		.rd32 = _nouveau_graph_context_rd32,
		.wr32 = _nouveau_graph_context_wr32,
	},
};

/*******************************************************************************
 * PGRAPH engine/subdev functions
 ******************************************************************************/

static void
nve0_graph_ctxctl_isr(struct nvc0_graph_priv *priv)
{
	u32 ustat = nv_rd32(priv, 0x409c18);

	if (ustat & 0x00000001)
		nv_error(priv, "CTXCTRL ucode error\n");
	if (ustat & 0x00080000)
		nv_error(priv, "CTXCTRL watchdog timeout\n");
	if (ustat & ~0x00080001)
		nv_error(priv, "CTXCTRL 0x%08x\n", ustat);

	nvc0_graph_ctxctl_debug(priv);
	nv_wr32(priv, 0x409c20, ustat);
}

static void
nve0_graph_trap_isr(struct nvc0_graph_priv *priv, int chid, u64 inst)
{
	u32 trap = nv_rd32(priv, 0x400108);
	int rop;

	if (trap & 0x00000001) {
		u32 stat = nv_rd32(priv, 0x404000);
		nv_error(priv, "DISPATCH ch %d [0x%010llx] 0x%08x\n",
			 chid, inst, stat);
		nv_wr32(priv, 0x404000, 0xc0000000);
		nv_wr32(priv, 0x400108, 0x00000001);
		trap &= ~0x00000001;
	}

	if (trap & 0x00000010) {
		u32 stat = nv_rd32(priv, 0x405840);
		nv_error(priv, "SHADER ch %d [0x%010llx] 0x%08x\n",
			 chid, inst, stat);
		nv_wr32(priv, 0x405840, 0xc0000000);
		nv_wr32(priv, 0x400108, 0x00000010);
		trap &= ~0x00000010;
	}

	if (trap & 0x02000000) {
		for (rop = 0; rop < priv->rop_nr; rop++) {
			u32 statz = nv_rd32(priv, ROP_UNIT(rop, 0x070));
			u32 statc = nv_rd32(priv, ROP_UNIT(rop, 0x144));
			nv_error(priv, "ROP%d ch %d [0x%010llx] 0x%08x 0x%08x\n",
				 rop, chid, inst, statz, statc);
			nv_wr32(priv, ROP_UNIT(rop, 0x070), 0xc0000000);
			nv_wr32(priv, ROP_UNIT(rop, 0x144), 0xc0000000);
		}
		nv_wr32(priv, 0x400108, 0x02000000);
		trap &= ~0x02000000;
	}

	if (trap) {
		nv_error(priv, "TRAP ch %d [0x%010llx] 0x%08x\n",
			 chid, inst, trap);
		nv_wr32(priv, 0x400108, trap);
	}
}

static void
nve0_graph_intr(struct nouveau_subdev *subdev)
{
	struct nouveau_fifo *pfifo = nouveau_fifo(subdev);
	struct nouveau_engine *engine = nv_engine(subdev);
	struct nouveau_object *engctx;
	struct nouveau_handle *handle;
	struct nvc0_graph_priv *priv = (void *)subdev;
	u64 inst = nv_rd32(priv, 0x409b00) & 0x0fffffff;
	u32 stat = nv_rd32(priv, 0x400100);
	u32 addr = nv_rd32(priv, 0x400704);
	u32 mthd = (addr & 0x00003ffc);
	u32 subc = (addr & 0x00070000) >> 16;
	u32 data = nv_rd32(priv, 0x400708);
	u32 code = nv_rd32(priv, 0x400110);
	u32 class = nv_rd32(priv, 0x404200 + (subc * 4));
	int chid;

	engctx = nouveau_engctx_get(engine, inst);
	chid   = pfifo->chid(pfifo, engctx);

	if (stat & 0x00000010) {
		handle = nouveau_handle_get_class(engctx, class);
		if (!handle || nv_call(handle->object, mthd, data)) {
			nv_error(priv, "ILLEGAL_MTHD ch %d [0x%010llx] "
				     "subc %d class 0x%04x mthd 0x%04x "
				     "data 0x%08x\n",
				 chid, inst, subc, class, mthd, data);
		}
		nouveau_handle_put(handle);
		nv_wr32(priv, 0x400100, 0x00000010);
		stat &= ~0x00000010;
	}

	if (stat & 0x00000020) {
		nv_error(priv, "ILLEGAL_CLASS ch %d [0x%010llx] subc %d "
			     "class 0x%04x mthd 0x%04x data 0x%08x\n",
			 chid, inst, subc, class, mthd, data);
		nv_wr32(priv, 0x400100, 0x00000020);
		stat &= ~0x00000020;
	}

	if (stat & 0x00100000) {
		nv_error(priv, "DATA_ERROR [");
		nouveau_enum_print(nv50_data_error_names, code);
		printk("] ch %d [0x%010llx] subc %d class 0x%04x "
		       "mthd 0x%04x data 0x%08x\n",
		       chid, inst, subc, class, mthd, data);
		nv_wr32(priv, 0x400100, 0x00100000);
		stat &= ~0x00100000;
	}

	if (stat & 0x00200000) {
		nve0_graph_trap_isr(priv, chid, inst);
		nv_wr32(priv, 0x400100, 0x00200000);
		stat &= ~0x00200000;
	}

	if (stat & 0x00080000) {
		nve0_graph_ctxctl_isr(priv);
		nv_wr32(priv, 0x400100, 0x00080000);
		stat &= ~0x00080000;
	}

	if (stat) {
		nv_error(priv, "unknown stat 0x%08x\n", stat);
		nv_wr32(priv, 0x400100, stat);
	}

	nv_wr32(priv, 0x400500, 0x00010001);
	nouveau_engctx_put(engctx);
}

static int
nve0_graph_ctor(struct nouveau_object *parent, struct nouveau_object *engine,
	       struct nouveau_oclass *oclass, void *data, u32 size,
	       struct nouveau_object **pobject)
{
	struct nouveau_device *device = nv_device(parent);
	struct nvc0_graph_priv *priv;
	int ret, i;

	ret = nouveau_graph_create(parent, engine, oclass, false, &priv);
	*pobject = nv_object(priv);
	if (ret)
		return ret;

	nv_subdev(priv)->unit = 0x18001000;
	nv_subdev(priv)->intr = nve0_graph_intr;
	nv_engine(priv)->cclass = &nve0_graph_cclass;
	nv_engine(priv)->sclass = nve0_graph_sclass;

	if (nouveau_boolopt(device->cfgopt, "NvGrUseFW", false)) {
		nv_info(priv, "using external firmware\n");
		if (nvc0_graph_ctor_fw(priv, "fuc409c", &priv->fuc409c) ||
		    nvc0_graph_ctor_fw(priv, "fuc409d", &priv->fuc409d) ||
		    nvc0_graph_ctor_fw(priv, "fuc41ac", &priv->fuc41ac) ||
		    nvc0_graph_ctor_fw(priv, "fuc41ad", &priv->fuc41ad))
			return -EINVAL;
		priv->firmware = true;
	}

	ret = nouveau_gpuobj_new(parent, NULL, 0x1000, 256, 0, &priv->unk4188b4);
	if (ret)
		return ret;

	ret = nouveau_gpuobj_new(parent, NULL, 0x1000, 256, 0, &priv->unk4188b8);
	if (ret)
		return ret;

	for (i = 0; i < 0x1000; i += 4) {
		nv_wo32(priv->unk4188b4, i, 0x00000010);
		nv_wo32(priv->unk4188b8, i, 0x00000010);
	}

	priv->gpc_nr =  nv_rd32(priv, 0x409604) & 0x0000001f;
	priv->rop_nr = (nv_rd32(priv, 0x409604) & 0x001f0000) >> 16;
	for (i = 0; i < priv->gpc_nr; i++) {
		priv->tpc_nr[i] = nv_rd32(priv, GPC_UNIT(i, 0x2608));
		priv->tpc_total += priv->tpc_nr[i];
	}

	switch (nv_device(priv)->chipset) {
	case 0xe4:
		if (priv->tpc_total == 8)
			priv->magic_not_rop_nr = 3;
		else
		if (priv->tpc_total == 7)
			priv->magic_not_rop_nr = 1;
		break;
	case 0xe7:
		priv->magic_not_rop_nr = 1;
		break;
	default:
		break;
	}

	return 0;
}

static void
nve0_graph_init_obj418880(struct nvc0_graph_priv *priv)
{
	int i;

	nv_wr32(priv, GPC_BCAST(0x0880), 0x00000000);
	nv_wr32(priv, GPC_BCAST(0x08a4), 0x00000000);
	for (i = 0; i < 4; i++)
		nv_wr32(priv, GPC_BCAST(0x0888) + (i * 4), 0x00000000);
	nv_wr32(priv, GPC_BCAST(0x08b4), priv->unk4188b4->addr >> 8);
	nv_wr32(priv, GPC_BCAST(0x08b8), priv->unk4188b8->addr >> 8);
}

static void
nve0_graph_init_regs(struct nvc0_graph_priv *priv)
{
	nv_wr32(priv, 0x400080, 0x003083c2);
	nv_wr32(priv, 0x400088, 0x0001ffe7);
	nv_wr32(priv, 0x40008c, 0x00000000);
	nv_wr32(priv, 0x400090, 0x00000030);
	nv_wr32(priv, 0x40013c, 0x003901f7);
	nv_wr32(priv, 0x400140, 0x00000100);
	nv_wr32(priv, 0x400144, 0x00000000);
	nv_wr32(priv, 0x400148, 0x00000110);
	nv_wr32(priv, 0x400138, 0x00000000);
	nv_wr32(priv, 0x400130, 0x00000000);
	nv_wr32(priv, 0x400134, 0x00000000);
	nv_wr32(priv, 0x400124, 0x00000002);
}

static void
nve0_graph_init_units(struct nvc0_graph_priv *priv)
{
	nv_wr32(priv, 0x409ffc, 0x00000000);
	nv_wr32(priv, 0x409c14, 0x00003e3e);
	nv_wr32(priv, 0x409c24, 0x000f0000);

	nv_wr32(priv, 0x404000, 0xc0000000);
	nv_wr32(priv, 0x404600, 0xc0000000);
	nv_wr32(priv, 0x408030, 0xc0000000);
	nv_wr32(priv, 0x404490, 0xc0000000);
	nv_wr32(priv, 0x406018, 0xc0000000);
	nv_wr32(priv, 0x407020, 0xc0000000);
	nv_wr32(priv, 0x405840, 0xc0000000);
	nv_wr32(priv, 0x405844, 0x00ffffff);

	nv_mask(priv, 0x419cc0, 0x00000008, 0x00000008);
	nv_mask(priv, 0x419eb4, 0x00001000, 0x00001000);

}

static void
nve0_graph_init_gpc_0(struct nvc0_graph_priv *priv)
{
	const u32 magicgpc918 = DIV_ROUND_UP(0x00800000, priv->tpc_total);
	u32 data[TPC_MAX / 8];
	u8  tpcnr[GPC_MAX];
	int i, gpc, tpc;

	nv_wr32(priv, GPC_UNIT(0, 0x3018), 0x00000001);

	memset(data, 0x00, sizeof(data));
	memcpy(tpcnr, priv->tpc_nr, sizeof(priv->tpc_nr));
	for (i = 0, gpc = -1; i < priv->tpc_total; i++) {
		do {
			gpc = (gpc + 1) % priv->gpc_nr;
		} while (!tpcnr[gpc]);
		tpc = priv->tpc_nr[gpc] - tpcnr[gpc]--;

		data[i / 8] |= tpc << ((i % 8) * 4);
	}

	nv_wr32(priv, GPC_BCAST(0x0980), data[0]);
	nv_wr32(priv, GPC_BCAST(0x0984), data[1]);
	nv_wr32(priv, GPC_BCAST(0x0988), data[2]);
	nv_wr32(priv, GPC_BCAST(0x098c), data[3]);

	for (gpc = 0; gpc < priv->gpc_nr; gpc++) {
		nv_wr32(priv, GPC_UNIT(gpc, 0x0914), priv->magic_not_rop_nr << 8 |
						  priv->tpc_nr[gpc]);
		nv_wr32(priv, GPC_UNIT(gpc, 0x0910), 0x00040000 | priv->tpc_total);
		nv_wr32(priv, GPC_UNIT(gpc, 0x0918), magicgpc918);
	}

	nv_wr32(priv, GPC_BCAST(0x1bd4), magicgpc918);
	nv_wr32(priv, GPC_BCAST(0x08ac), nv_rd32(priv, 0x100800));
}

static void
nve0_graph_init_gpc_1(struct nvc0_graph_priv *priv)
{
	int gpc, tpc;

	for (gpc = 0; gpc < priv->gpc_nr; gpc++) {
		nv_wr32(priv, GPC_UNIT(gpc, 0x3038), 0xc0000000);
		nv_wr32(priv, GPC_UNIT(gpc, 0x0420), 0xc0000000);
		nv_wr32(priv, GPC_UNIT(gpc, 0x0900), 0xc0000000);
		nv_wr32(priv, GPC_UNIT(gpc, 0x1028), 0xc0000000);
		nv_wr32(priv, GPC_UNIT(gpc, 0x0824), 0xc0000000);
		for (tpc = 0; tpc < priv->tpc_nr[gpc]; tpc++) {
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x508), 0xffffffff);
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x50c), 0xffffffff);
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x224), 0xc0000000);
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x48c), 0xc0000000);
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x084), 0xc0000000);
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x644), 0x001ffffe);
			nv_wr32(priv, TPC_UNIT(gpc, tpc, 0x64c), 0x0000000f);
		}
		nv_wr32(priv, GPC_UNIT(gpc, 0x2c90), 0xffffffff);
		nv_wr32(priv, GPC_UNIT(gpc, 0x2c94), 0xffffffff);
	}
}

static void
nve0_graph_init_rop(struct nvc0_graph_priv *priv)
{
	int rop;

	for (rop = 0; rop < priv->rop_nr; rop++) {
		nv_wr32(priv, ROP_UNIT(rop, 0x144), 0xc0000000);
		nv_wr32(priv, ROP_UNIT(rop, 0x070), 0xc0000000);
		nv_wr32(priv, ROP_UNIT(rop, 0x204), 0xffffffff);
		nv_wr32(priv, ROP_UNIT(rop, 0x208), 0xffffffff);
	}
}

static int
nve0_graph_init_ctxctl(struct nvc0_graph_priv *priv)
{
	u32 r000260;
	int i;

	if (priv->firmware) {
		/* load fuc microcode */
		r000260 = nv_mask(priv, 0x000260, 0x00000001, 0x00000000);
		nvc0_graph_init_fw(priv, 0x409000, &priv->fuc409c, &priv->fuc409d);
		nvc0_graph_init_fw(priv, 0x41a000, &priv->fuc41ac, &priv->fuc41ad);
		nv_wr32(priv, 0x000260, r000260);

		/* start both of them running */
		nv_wr32(priv, 0x409840, 0xffffffff);
		nv_wr32(priv, 0x41a10c, 0x00000000);
		nv_wr32(priv, 0x40910c, 0x00000000);
		nv_wr32(priv, 0x41a100, 0x00000002);
		nv_wr32(priv, 0x409100, 0x00000002);
		if (!nv_wait(priv, 0x409800, 0x00000001, 0x00000001))
			nv_error(priv, "0x409800 wait failed\n");

		nv_wr32(priv, 0x409840, 0xffffffff);
		nv_wr32(priv, 0x409500, 0x7fffffff);
		nv_wr32(priv, 0x409504, 0x00000021);

		nv_wr32(priv, 0x409840, 0xffffffff);
		nv_wr32(priv, 0x409500, 0x00000000);
		nv_wr32(priv, 0x409504, 0x00000010);
		if (!nv_wait_ne(priv, 0x409800, 0xffffffff, 0x00000000)) {
			nv_error(priv, "fuc09 req 0x10 timeout\n");
			return -EBUSY;
		}
		priv->size = nv_rd32(priv, 0x409800);

		nv_wr32(priv, 0x409840, 0xffffffff);
		nv_wr32(priv, 0x409500, 0x00000000);
		nv_wr32(priv, 0x409504, 0x00000016);
		if (!nv_wait_ne(priv, 0x409800, 0xffffffff, 0x00000000)) {
			nv_error(priv, "fuc09 req 0x16 timeout\n");
			return -EBUSY;
		}

		nv_wr32(priv, 0x409840, 0xffffffff);
		nv_wr32(priv, 0x409500, 0x00000000);
		nv_wr32(priv, 0x409504, 0x00000025);
		if (!nv_wait_ne(priv, 0x409800, 0xffffffff, 0x00000000)) {
			nv_error(priv, "fuc09 req 0x25 timeout\n");
			return -EBUSY;
		}

		nv_wr32(priv, 0x409800, 0x00000000);
		nv_wr32(priv, 0x409500, 0x00000001);
		nv_wr32(priv, 0x409504, 0x00000030);
		if (!nv_wait_ne(priv, 0x409800, 0xffffffff, 0x00000000)) {
			nv_error(priv, "fuc09 req 0x30 timeout\n");
			return -EBUSY;
		}

		nv_wr32(priv, 0x409810, 0xb00095c8);
		nv_wr32(priv, 0x409800, 0x00000000);
		nv_wr32(priv, 0x409500, 0x00000001);
		nv_wr32(priv, 0x409504, 0x00000031);
		if (!nv_wait_ne(priv, 0x409800, 0xffffffff, 0x00000000)) {
			nv_error(priv, "fuc09 req 0x31 timeout\n");
			return -EBUSY;
		}

		nv_wr32(priv, 0x409810, 0x00080420);
		nv_wr32(priv, 0x409800, 0x00000000);
		nv_wr32(priv, 0x409500, 0x00000001);
		nv_wr32(priv, 0x409504, 0x00000032);
		if (!nv_wait_ne(priv, 0x409800, 0xffffffff, 0x00000000)) {
			nv_error(priv, "fuc09 req 0x32 timeout\n");
			return -EBUSY;
		}

		nv_wr32(priv, 0x409614, 0x00000070);
		nv_wr32(priv, 0x409614, 0x00000770);
		nv_wr32(priv, 0x40802c, 0x00000001);

		if (priv->data == NULL) {
			int ret = nve0_grctx_generate(priv);
			if (ret) {
				nv_error(priv, "failed to construct context\n");
				return ret;
			}
		}

		return 0;
	}

	/* load HUB microcode */
	r000260 = nv_mask(priv, 0x000260, 0x00000001, 0x00000000);
	nv_wr32(priv, 0x4091c0, 0x01000000);
	for (i = 0; i < sizeof(nve0_grhub_data) / 4; i++)
		nv_wr32(priv, 0x4091c4, nve0_grhub_data[i]);

	nv_wr32(priv, 0x409180, 0x01000000);
	for (i = 0; i < sizeof(nve0_grhub_code) / 4; i++) {
		if ((i & 0x3f) == 0)
			nv_wr32(priv, 0x409188, i >> 6);
		nv_wr32(priv, 0x409184, nve0_grhub_code[i]);
	}

	/* load GPC microcode */
	nv_wr32(priv, 0x41a1c0, 0x01000000);
	for (i = 0; i < sizeof(nve0_grgpc_data) / 4; i++)
		nv_wr32(priv, 0x41a1c4, nve0_grgpc_data[i]);

	nv_wr32(priv, 0x41a180, 0x01000000);
	for (i = 0; i < sizeof(nve0_grgpc_code) / 4; i++) {
		if ((i & 0x3f) == 0)
			nv_wr32(priv, 0x41a188, i >> 6);
		nv_wr32(priv, 0x41a184, nve0_grgpc_code[i]);
	}
	nv_wr32(priv, 0x000260, r000260);

	/* start HUB ucode running, it'll init the GPCs */
	nv_wr32(priv, 0x409800, nv_device(priv)->chipset);
	nv_wr32(priv, 0x40910c, 0x00000000);
	nv_wr32(priv, 0x409100, 0x00000002);
	if (!nv_wait(priv, 0x409800, 0x80000000, 0x80000000)) {
		nv_error(priv, "HUB_INIT timed out\n");
		nvc0_graph_ctxctl_debug(priv);
		return -EBUSY;
	}

	priv->size = nv_rd32(priv, 0x409804);
	if (priv->data == NULL) {
		int ret = nve0_grctx_generate(priv);
		if (ret) {
			nv_error(priv, "failed to construct context\n");
			return ret;
		}
	}

	return 0;
}

static int
nve0_graph_init(struct nouveau_object *object)
{
	struct nvc0_graph_priv *priv = (void *)object;
	int ret;

	ret = nouveau_graph_init(&priv->base);
	if (ret)
		return ret;

	nve0_graph_init_obj418880(priv);
	nve0_graph_init_regs(priv);
	nve0_graph_init_gpc_0(priv);

	nv_wr32(priv, 0x400500, 0x00010001);
	nv_wr32(priv, 0x400100, 0xffffffff);
	nv_wr32(priv, 0x40013c, 0xffffffff);

	nve0_graph_init_units(priv);
	nve0_graph_init_gpc_1(priv);
	nve0_graph_init_rop(priv);

	nv_wr32(priv, 0x400108, 0xffffffff);
	nv_wr32(priv, 0x400138, 0xffffffff);
	nv_wr32(priv, 0x400118, 0xffffffff);
	nv_wr32(priv, 0x400130, 0xffffffff);
	nv_wr32(priv, 0x40011c, 0xffffffff);
	nv_wr32(priv, 0x400134, 0xffffffff);
	nv_wr32(priv, 0x400054, 0x34ce3464);

	ret = nve0_graph_init_ctxctl(priv);
	if (ret)
		return ret;

	return 0;
}

struct nouveau_oclass
nve0_graph_oclass = {
	.handle = NV_ENGINE(GR, 0xe0),
	.ofuncs = &(struct nouveau_ofuncs) {
		.ctor = nve0_graph_ctor,
		.dtor = nvc0_graph_dtor,
		.init = nve0_graph_init,
		.fini = _nouveau_graph_fini,
	},
};
