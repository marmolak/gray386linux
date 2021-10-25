/*
 * Copyright (C) 2008 Maarten Maathuis.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "nouveau_drm.h"
#include "nouveau_dma.h"

#include "nv50_display.h"
#include "nouveau_crtc.h"
#include "nouveau_encoder.h"
#include "nouveau_connector.h"
#include "nouveau_fbcon.h"
#include <drm/drm_crtc_helper.h>
#include "nouveau_fence.h"

#include <core/gpuobj.h>
#include <subdev/timer.h>

static void nv50_display_bh(unsigned long);

static inline int
nv50_sor_nr(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);

	if (device->chipset  < 0x90 ||
	    device->chipset == 0x92 ||
	    device->chipset == 0xa0)
		return 2;

	return 4;
}

u32
nv50_display_active_crtcs(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);
	u32 mask = 0;
	int i;

	if (device->chipset  < 0x90 ||
	    device->chipset == 0x92 ||
	    device->chipset == 0xa0) {
		for (i = 0; i < 2; i++)
			mask |= nv_rd32(device, NV50_PDISPLAY_SOR_MODE_CTRL_C(i));
	} else {
		for (i = 0; i < 4; i++)
			mask |= nv_rd32(device, NV90_PDISPLAY_SOR_MODE_CTRL_C(i));
	}

	for (i = 0; i < 3; i++)
		mask |= nv_rd32(device, NV50_PDISPLAY_DAC_MODE_CTRL_C(i));

	return mask & 3;
}

int
nv50_display_early_init(struct drm_device *dev)
{
	return 0;
}

void
nv50_display_late_takedown(struct drm_device *dev)
{
}

int
nv50_display_sync(struct drm_device *dev)
{
	struct nv50_display *disp = nv50_display(dev);
	struct nouveau_channel *evo = disp->master;
	int ret;

	ret = RING_SPACE(evo, 6);
	if (ret == 0) {
		BEGIN_NV04(evo, 0, 0x0084, 1);
		OUT_RING  (evo, 0x80000000);
		BEGIN_NV04(evo, 0, 0x0080, 1);
		OUT_RING  (evo, 0);
		BEGIN_NV04(evo, 0, 0x0084, 1);
		OUT_RING  (evo, 0x00000000);

		nv_wo32(disp->ramin, 0x2000, 0x00000000);
		FIRE_RING (evo);

		if (nv_wait_ne(disp->ramin, 0x2000, 0xffffffff, 0x00000000))
			return 0;
	}

	return 0;
}

int
nv50_display_init(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_channel *evo;
	int ret, i;
	u32 val;

	NV_DEBUG(drm, "\n");

	nv_wr32(device, 0x00610184, nv_rd32(device, 0x00614004));

	/*
	 * I think the 0x006101XX range is some kind of main control area
	 * that enables things.
	 */
	/* CRTC? */
	for (i = 0; i < 2; i++) {
		val = nv_rd32(device, 0x00616100 + (i * 0x800));
		nv_wr32(device, 0x00610190 + (i * 0x10), val);
		val = nv_rd32(device, 0x00616104 + (i * 0x800));
		nv_wr32(device, 0x00610194 + (i * 0x10), val);
		val = nv_rd32(device, 0x00616108 + (i * 0x800));
		nv_wr32(device, 0x00610198 + (i * 0x10), val);
		val = nv_rd32(device, 0x0061610c + (i * 0x800));
		nv_wr32(device, 0x0061019c + (i * 0x10), val);
	}

	/* DAC */
	for (i = 0; i < 3; i++) {
		val = nv_rd32(device, 0x0061a000 + (i * 0x800));
		nv_wr32(device, 0x006101d0 + (i * 0x04), val);
	}

	/* SOR */
	for (i = 0; i < nv50_sor_nr(dev); i++) {
		val = nv_rd32(device, 0x0061c000 + (i * 0x800));
		nv_wr32(device, 0x006101e0 + (i * 0x04), val);
	}

	/* EXT */
	for (i = 0; i < 3; i++) {
		val = nv_rd32(device, 0x0061e000 + (i * 0x800));
		nv_wr32(device, 0x006101f0 + (i * 0x04), val);
	}

	for (i = 0; i < 3; i++) {
		nv_wr32(device, NV50_PDISPLAY_DAC_DPMS_CTRL(i), 0x00550000 |
			NV50_PDISPLAY_DAC_DPMS_CTRL_PENDING);
		nv_wr32(device, NV50_PDISPLAY_DAC_CLK_CTRL1(i), 0x00000001);
	}

	/* The precise purpose is unknown, i suspect it has something to do
	 * with text mode.
	 */
	if (nv_rd32(device, NV50_PDISPLAY_INTR_1) & 0x100) {
		nv_wr32(device, NV50_PDISPLAY_INTR_1, 0x100);
		nv_wr32(device, 0x006194e8, nv_rd32(device, 0x006194e8) & ~1);
		if (!nv_wait(device, 0x006194e8, 2, 0)) {
			NV_ERROR(drm, "timeout: (0x6194e8 & 2) != 0\n");
			NV_ERROR(drm, "0x6194e8 = 0x%08x\n",
						nv_rd32(device, 0x6194e8));
			return -EBUSY;
		}
	}

	for (i = 0; i < 2; i++) {
		nv_wr32(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i), 0x2000);
		if (!nv_wait(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i),
			     NV50_PDISPLAY_CURSOR_CURSOR_CTRL2_STATUS, 0)) {
			NV_ERROR(drm, "timeout: CURSOR_CTRL2_STATUS == 0\n");
			NV_ERROR(drm, "CURSOR_CTRL2 = 0x%08x\n",
				 nv_rd32(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i)));
			return -EBUSY;
		}

		nv_wr32(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i),
			NV50_PDISPLAY_CURSOR_CURSOR_CTRL2_ON);
		if (!nv_wait(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i),
			     NV50_PDISPLAY_CURSOR_CURSOR_CTRL2_STATUS,
			     NV50_PDISPLAY_CURSOR_CURSOR_CTRL2_STATUS_ACTIVE)) {
			NV_ERROR(drm, "timeout: "
				      "CURSOR_CTRL2_STATUS_ACTIVE(%d)\n", i);
			NV_ERROR(drm, "CURSOR_CTRL2(%d) = 0x%08x\n", i,
				 nv_rd32(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i)));
			return -EBUSY;
		}
	}

	nv_wr32(device, NV50_PDISPLAY_PIO_CTRL, 0x00000000);
	nv_mask(device, NV50_PDISPLAY_INTR_0, 0x00000000, 0x00000000);
	nv_wr32(device, NV50_PDISPLAY_INTR_EN_0, 0x00000000);
	nv_mask(device, NV50_PDISPLAY_INTR_1, 0x00000000, 0x00000000);
	nv_wr32(device, NV50_PDISPLAY_INTR_EN_1,
		     NV50_PDISPLAY_INTR_EN_1_CLK_UNK10 |
		     NV50_PDISPLAY_INTR_EN_1_CLK_UNK20 |
		     NV50_PDISPLAY_INTR_EN_1_CLK_UNK40);

	ret = nv50_evo_init(dev);
	if (ret)
		return ret;
	evo = nv50_display(dev)->master;

	nv_wr32(device, NV50_PDISPLAY_OBJECTS, (nv50_display(dev)->ramin->addr >> 8) | 9);

	ret = RING_SPACE(evo, 3);
	if (ret)
		return ret;
	BEGIN_NV04(evo, 0, NV50_EVO_UNK84, 2);
	OUT_RING  (evo, NV50_EVO_UNK84_NOTIFY_DISABLED);
	OUT_RING  (evo, NvEvoSync);

	return nv50_display_sync(dev);
}

void
nv50_display_fini(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_device *device = nouveau_dev(dev);
	struct nv50_display *disp = nv50_display(dev);
	struct nouveau_channel *evo = disp->master;
	struct drm_crtc *drm_crtc;
	int ret, i;

	NV_DEBUG(drm, "\n");

	list_for_each_entry(drm_crtc, &dev->mode_config.crtc_list, head) {
		struct nouveau_crtc *crtc = nouveau_crtc(drm_crtc);

		nv50_crtc_blank(crtc, true);
	}

	ret = RING_SPACE(evo, 2);
	if (ret == 0) {
		BEGIN_NV04(evo, 0, NV50_EVO_UPDATE, 1);
		OUT_RING(evo, 0);
	}
	FIRE_RING(evo);

	/* Almost like ack'ing a vblank interrupt, maybe in the spirit of
	 * cleaning up?
	 */
	list_for_each_entry(drm_crtc, &dev->mode_config.crtc_list, head) {
		struct nouveau_crtc *crtc = nouveau_crtc(drm_crtc);
		uint32_t mask = NV50_PDISPLAY_INTR_1_VBLANK_CRTC_(crtc->index);

		if (!crtc->base.enabled)
			continue;

		nv_wr32(device, NV50_PDISPLAY_INTR_1, mask);
		if (!nv_wait(device, NV50_PDISPLAY_INTR_1, mask, mask)) {
			NV_ERROR(drm, "timeout: (0x610024 & 0x%08x) == "
				      "0x%08x\n", mask, mask);
			NV_ERROR(drm, "0x610024 = 0x%08x\n",
				 nv_rd32(device, NV50_PDISPLAY_INTR_1));
		}
	}

	for (i = 0; i < 2; i++) {
		nv_wr32(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i), 0);
		if (!nv_wait(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i),
			     NV50_PDISPLAY_CURSOR_CURSOR_CTRL2_STATUS, 0)) {
			NV_ERROR(drm, "timeout: CURSOR_CTRL2_STATUS == 0\n");
			NV_ERROR(drm, "CURSOR_CTRL2 = 0x%08x\n",
				 nv_rd32(device, NV50_PDISPLAY_CURSOR_CURSOR_CTRL2(i)));
		}
	}

	nv50_evo_fini(dev);

	for (i = 0; i < 3; i++) {
		if (!nv_wait(device, NV50_PDISPLAY_SOR_DPMS_STATE(i),
			     NV50_PDISPLAY_SOR_DPMS_STATE_WAIT, 0)) {
			NV_ERROR(drm, "timeout: SOR_DPMS_STATE_WAIT(%d) == 0\n", i);
			NV_ERROR(drm, "SOR_DPMS_STATE(%d) = 0x%08x\n", i,
				  nv_rd32(device, NV50_PDISPLAY_SOR_DPMS_STATE(i)));
		}
	}

	/* disable interrupts. */
	nv_wr32(device, NV50_PDISPLAY_INTR_EN_1, 0x00000000);
}

int
nv50_display_create(struct drm_device *dev)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct dcb_table *dcb = &drm->vbios.dcb;
	struct drm_connector *connector, *ct;
	struct nv50_display *priv;
	int ret, i;

	NV_DEBUG(drm, "\n");

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	nouveau_display(dev)->priv = priv;
	nouveau_display(dev)->dtor = nv50_display_destroy;
	nouveau_display(dev)->init = nv50_display_init;
	nouveau_display(dev)->fini = nv50_display_fini;

	/* Create CRTC objects */
	for (i = 0; i < 2; i++) {
		ret = nv50_crtc_create(dev, i);
		if (ret)
			return ret;
	}

	/* We setup the encoders from the BIOS table */
	for (i = 0 ; i < dcb->entries; i++) {
		struct dcb_output *entry = &dcb->entry[i];

		if (entry->location != DCB_LOC_ON_CHIP) {
			NV_WARN(drm, "Off-chip encoder %d/%d unsupported\n",
				entry->type, ffs(entry->or) - 1);
			continue;
		}

		connector = nouveau_connector_create(dev, entry->connector);
		if (IS_ERR(connector))
			continue;

		switch (entry->type) {
		case DCB_OUTPUT_TMDS:
		case DCB_OUTPUT_LVDS:
		case DCB_OUTPUT_DP:
			nv50_sor_create(connector, entry);
			break;
		case DCB_OUTPUT_ANALOG:
			nv50_dac_create(connector, entry);
			break;
		default:
			NV_WARN(drm, "DCB encoder %d unknown\n", entry->type);
			continue;
		}
	}

	list_for_each_entry_safe(connector, ct,
				 &dev->mode_config.connector_list, head) {
		if (!connector->encoder_ids[0]) {
			NV_WARN(drm, "%s has no encoders, removing\n",
				drm_get_connector_name(connector));
			connector->funcs->destroy(connector);
		}
	}

	tasklet_init(&priv->tasklet, nv50_display_bh, (unsigned long)dev);

	ret = nv50_evo_create(dev);
	if (ret) {
		nv50_display_destroy(dev);
		return ret;
	}

	return 0;
}

void
nv50_display_destroy(struct drm_device *dev)
{
	struct nv50_display *disp = nv50_display(dev);

	nv50_evo_destroy(dev);
	kfree(disp);
}

struct nouveau_bo *
nv50_display_crtc_sema(struct drm_device *dev, int crtc)
{
	return nv50_display(dev)->crtc[crtc].sem.bo;
}

void
nv50_display_flip_stop(struct drm_crtc *crtc)
{
	struct nv50_display *disp = nv50_display(crtc->dev);
	struct nouveau_crtc *nv_crtc = nouveau_crtc(crtc);
	struct nv50_display_crtc *dispc = &disp->crtc[nv_crtc->index];
	struct nouveau_channel *evo = dispc->sync;
	int ret;

	ret = RING_SPACE(evo, 8);
	if (ret) {
		WARN_ON(1);
		return;
	}

	BEGIN_NV04(evo, 0, 0x0084, 1);
	OUT_RING  (evo, 0x00000000);
	BEGIN_NV04(evo, 0, 0x0094, 1);
	OUT_RING  (evo, 0x00000000);
	BEGIN_NV04(evo, 0, 0x00c0, 1);
	OUT_RING  (evo, 0x00000000);
	BEGIN_NV04(evo, 0, 0x0080, 1);
	OUT_RING  (evo, 0x00000000);
	FIRE_RING (evo);
}

int
nv50_display_flip_next(struct drm_crtc *crtc, struct drm_framebuffer *fb,
		       struct nouveau_channel *chan)
{
	struct nouveau_drm *drm = nouveau_drm(crtc->dev);
	struct nouveau_framebuffer *nv_fb = nouveau_framebuffer(fb);
	struct nv50_display *disp = nv50_display(crtc->dev);
	struct nouveau_crtc *nv_crtc = nouveau_crtc(crtc);
	struct nv50_display_crtc *dispc = &disp->crtc[nv_crtc->index];
	struct nouveau_channel *evo = dispc->sync;
	int ret;

	ret = RING_SPACE(evo, chan ? 25 : 27);
	if (unlikely(ret))
		return ret;

	/* synchronise with the rendering channel, if necessary */
	if (likely(chan)) {
		ret = RING_SPACE(chan, 10);
		if (ret) {
			WIND_RING(evo);
			return ret;
		}

		if (nv_device(drm->device)->chipset < 0xc0) {
			BEGIN_NV04(chan, 0, 0x0060, 2);
			OUT_RING  (chan, NvEvoSema0 + nv_crtc->index);
			OUT_RING  (chan, dispc->sem.offset);
			BEGIN_NV04(chan, 0, 0x006c, 1);
			OUT_RING  (chan, 0xf00d0000 | dispc->sem.value);
			BEGIN_NV04(chan, 0, 0x0064, 2);
			OUT_RING  (chan, dispc->sem.offset ^ 0x10);
			OUT_RING  (chan, 0x74b1e000);
			BEGIN_NV04(chan, 0, 0x0060, 1);
			if (nv_device(drm->device)->chipset < 0x84)
				OUT_RING  (chan, NvSema);
			else
				OUT_RING  (chan, chan->vram);
		} else {
			u64 offset = nvc0_fence_crtc(chan, nv_crtc->index);
			offset += dispc->sem.offset;
			BEGIN_NVC0(chan, 0, 0x0010, 4);
			OUT_RING  (chan, upper_32_bits(offset));
			OUT_RING  (chan, lower_32_bits(offset));
			OUT_RING  (chan, 0xf00d0000 | dispc->sem.value);
			OUT_RING  (chan, 0x1002);
			BEGIN_NVC0(chan, 0, 0x0010, 4);
			OUT_RING  (chan, upper_32_bits(offset));
			OUT_RING  (chan, lower_32_bits(offset ^ 0x10));
			OUT_RING  (chan, 0x74b1e000);
			OUT_RING  (chan, 0x1001);
		}
		FIRE_RING (chan);
	} else {
		nouveau_bo_wr32(dispc->sem.bo, dispc->sem.offset / 4,
				0xf00d0000 | dispc->sem.value);
	}

	/* queue the flip on the crtc's "display sync" channel */
	BEGIN_NV04(evo, 0, 0x0100, 1);
	OUT_RING  (evo, 0xfffe0000);
	if (chan) {
		BEGIN_NV04(evo, 0, 0x0084, 1);
		OUT_RING  (evo, 0x00000100);
	} else {
		BEGIN_NV04(evo, 0, 0x0084, 1);
		OUT_RING  (evo, 0x00000010);
		/* allows gamma somehow, PDISP will bitch at you if
		 * you don't wait for vblank before changing this..
		 */
		BEGIN_NV04(evo, 0, 0x00e0, 1);
		OUT_RING  (evo, 0x40000000);
	}
	BEGIN_NV04(evo, 0, 0x0088, 4);
	OUT_RING  (evo, dispc->sem.offset);
	OUT_RING  (evo, 0xf00d0000 | dispc->sem.value);
	OUT_RING  (evo, 0x74b1e000);
	OUT_RING  (evo, NvEvoSync);
	BEGIN_NV04(evo, 0, 0x00a0, 2);
	OUT_RING  (evo, 0x00000000);
	OUT_RING  (evo, 0x00000000);
	BEGIN_NV04(evo, 0, 0x00c0, 1);
	OUT_RING  (evo, nv_fb->r_dma);
	BEGIN_NV04(evo, 0, 0x0110, 2);
	OUT_RING  (evo, 0x00000000);
	OUT_RING  (evo, 0x00000000);
	BEGIN_NV04(evo, 0, 0x0800, 5);
	OUT_RING  (evo, nv_fb->nvbo->bo.offset >> 8);
	OUT_RING  (evo, 0);
	OUT_RING  (evo, (fb->height << 16) | fb->width);
	OUT_RING  (evo, nv_fb->r_pitch);
	OUT_RING  (evo, nv_fb->r_format);
	BEGIN_NV04(evo, 0, 0x0080, 1);
	OUT_RING  (evo, 0x00000000);
	FIRE_RING (evo);

	dispc->sem.offset ^= 0x10;
	dispc->sem.value++;
	return 0;
}

static u16
nv50_display_script_select(struct drm_device *dev, struct dcb_output *dcb,
			   u32 mc, int pxclk)
{
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nouveau_connector *nv_connector = NULL;
	struct drm_encoder *encoder;
	struct nvbios *bios = &drm->vbios;
	u32 script = 0, or;

	list_for_each_entry(encoder, &dev->mode_config.encoder_list, head) {
		struct nouveau_encoder *nv_encoder = nouveau_encoder(encoder);

		if (nv_encoder->dcb != dcb)
			continue;

		nv_connector = nouveau_encoder_connector_get(nv_encoder);
		break;
	}

	or = ffs(dcb->or) - 1;
	switch (dcb->type) {
	case DCB_OUTPUT_LVDS:
		script = (mc >> 8) & 0xf;
		if (bios->fp_no_ddc) {
			if (bios->fp.dual_link)
				script |= 0x0100;
			if (bios->fp.if_is_24bit)
				script |= 0x0200;
		} else {
			/* determine number of lvds links */
			if (nv_connector && nv_connector->edid &&
			    nv_connector->type == DCB_CONNECTOR_LVDS_SPWG) {
				/* http://www.spwg.org */
				if (((u8 *)nv_connector->edid)[121] == 2)
					script |= 0x0100;
			} else
			if (pxclk >= bios->fp.duallink_transition_clk) {
				script |= 0x0100;
			}

			/* determine panel depth */
			if (script & 0x0100) {
				if (bios->fp.strapless_is_24bit & 2)
					script |= 0x0200;
			} else {
				if (bios->fp.strapless_is_24bit & 1)
					script |= 0x0200;
			}

			if (nv_connector && nv_connector->edid &&
			    (nv_connector->edid->revision >= 4) &&
			    (nv_connector->edid->input & 0x70) >= 0x20)
				script |= 0x0200;
		}
		break;
	case DCB_OUTPUT_TMDS:
		script = (mc >> 8) & 0xf;
		if (pxclk >= 165000)
			script |= 0x0100;
		break;
	case DCB_OUTPUT_DP:
		script = (mc >> 8) & 0xf;
		break;
	case DCB_OUTPUT_ANALOG:
		script = 0xff;
		break;
	default:
		NV_ERROR(drm, "modeset on unsupported output type!\n");
		break;
	}

	return script;
}

static void
nv50_display_unk10_handler(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nv50_display *disp = nv50_display(dev);
	u32 unk30 = nv_rd32(device, 0x610030), mc;
	int i, crtc, or = 0, type = DCB_OUTPUT_ANY;

	NV_DEBUG(drm, "0x610030: 0x%08x\n", unk30);
	disp->irq.dcb = NULL;

	nv_wr32(device, 0x619494, nv_rd32(device, 0x619494) & ~8);

	/* Determine which CRTC we're dealing with, only 1 ever will be
	 * signalled at the same time with the current nouveau code.
	 */
	crtc = ffs((unk30 & 0x00000060) >> 5) - 1;
	if (crtc < 0)
		goto ack;

	/* Nothing needs to be done for the encoder */
	crtc = ffs((unk30 & 0x00000180) >> 7) - 1;
	if (crtc < 0)
		goto ack;

	/* Find which encoder was connected to the CRTC */
	for (i = 0; type == DCB_OUTPUT_ANY && i < 3; i++) {
		mc = nv_rd32(device, NV50_PDISPLAY_DAC_MODE_CTRL_C(i));
		NV_DEBUG(drm, "DAC-%d mc: 0x%08x\n", i, mc);
		if (!(mc & (1 << crtc)))
			continue;

		switch ((mc & 0x00000f00) >> 8) {
		case 0: type = DCB_OUTPUT_ANALOG; break;
		case 1: type = DCB_OUTPUT_TV; break;
		default:
			NV_ERROR(drm, "invalid mc, DAC-%d: 0x%08x\n", i, mc);
			goto ack;
		}

		or = i;
	}

	for (i = 0; type == DCB_OUTPUT_ANY && i < nv50_sor_nr(dev); i++) {
		if (nv_device(drm->device)->chipset  < 0x90 ||
		    nv_device(drm->device)->chipset == 0x92 ||
		    nv_device(drm->device)->chipset == 0xa0)
			mc = nv_rd32(device, NV50_PDISPLAY_SOR_MODE_CTRL_C(i));
		else
			mc = nv_rd32(device, NV90_PDISPLAY_SOR_MODE_CTRL_C(i));

		NV_DEBUG(drm, "SOR-%d mc: 0x%08x\n", i, mc);
		if (!(mc & (1 << crtc)))
			continue;

		switch ((mc & 0x00000f00) >> 8) {
		case 0: type = DCB_OUTPUT_LVDS; break;
		case 1: type = DCB_OUTPUT_TMDS; break;
		case 2: type = DCB_OUTPUT_TMDS; break;
		case 5: type = DCB_OUTPUT_TMDS; break;
		case 8: type = DCB_OUTPUT_DP; break;
		case 9: type = DCB_OUTPUT_DP; break;
		default:
			NV_ERROR(drm, "invalid mc, SOR-%d: 0x%08x\n", i, mc);
			goto ack;
		}

		or = i;
	}

	/* There was no encoder to disable */
	if (type == DCB_OUTPUT_ANY)
		goto ack;

	/* Disable the encoder */
	for (i = 0; i < drm->vbios.dcb.entries; i++) {
		struct dcb_output *dcb = &drm->vbios.dcb.entry[i];

		if (dcb->type == type && (dcb->or & (1 << or))) {
			nouveau_bios_run_display_table(dev, 0, -1, dcb, -1);
			disp->irq.dcb = dcb;
			goto ack;
		}
	}

	NV_ERROR(drm, "no dcb for %d %d 0x%08x\n", or, type, mc);
ack:
	nv_wr32(device, NV50_PDISPLAY_INTR_1, NV50_PDISPLAY_INTR_1_CLK_UNK10);
	nv_wr32(device, 0x610030, 0x80000000);
}

static void
nv50_display_unk20_handler(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nv50_display *disp = nv50_display(dev);
	u32 unk30 = nv_rd32(device, 0x610030), tmp, pclk, script, mc = 0;
	struct dcb_output *dcb;
	int i, crtc, or = 0, type = DCB_OUTPUT_ANY;

	NV_DEBUG(drm, "0x610030: 0x%08x\n", unk30);
	dcb = disp->irq.dcb;
	if (dcb) {
		nouveau_bios_run_display_table(dev, 0, -2, dcb, -1);
		disp->irq.dcb = NULL;
	}

	/* CRTC clock change requested? */
	crtc = ffs((unk30 & 0x00000600) >> 9) - 1;
	if (crtc >= 0) {
		pclk  = nv_rd32(device, NV50_PDISPLAY_CRTC_P(crtc, CLOCK));
		pclk &= 0x003fffff;
		if (pclk)
			nv50_crtc_set_clock(dev, crtc, pclk);

		tmp = nv_rd32(device, NV50_PDISPLAY_CRTC_CLK_CTRL2(crtc));
		tmp &= ~0x000000f;
		nv_wr32(device, NV50_PDISPLAY_CRTC_CLK_CTRL2(crtc), tmp);
	}

	/* Nothing needs to be done for the encoder */
	crtc = ffs((unk30 & 0x00000180) >> 7) - 1;
	if (crtc < 0)
		goto ack;
	pclk  = nv_rd32(device, NV50_PDISPLAY_CRTC_P(crtc, CLOCK)) & 0x003fffff;

	/* Find which encoder is connected to the CRTC */
	for (i = 0; type == DCB_OUTPUT_ANY && i < 3; i++) {
		mc = nv_rd32(device, NV50_PDISPLAY_DAC_MODE_CTRL_P(i));
		NV_DEBUG(drm, "DAC-%d mc: 0x%08x\n", i, mc);
		if (!(mc & (1 << crtc)))
			continue;

		switch ((mc & 0x00000f00) >> 8) {
		case 0: type = DCB_OUTPUT_ANALOG; break;
		case 1: type = DCB_OUTPUT_TV; break;
		default:
			NV_ERROR(drm, "invalid mc, DAC-%d: 0x%08x\n", i, mc);
			goto ack;
		}

		or = i;
	}

	for (i = 0; type == DCB_OUTPUT_ANY && i < nv50_sor_nr(dev); i++) {
		if (nv_device(drm->device)->chipset  < 0x90 ||
		    nv_device(drm->device)->chipset == 0x92 ||
		    nv_device(drm->device)->chipset == 0xa0)
			mc = nv_rd32(device, NV50_PDISPLAY_SOR_MODE_CTRL_P(i));
		else
			mc = nv_rd32(device, NV90_PDISPLAY_SOR_MODE_CTRL_P(i));

		NV_DEBUG(drm, "SOR-%d mc: 0x%08x\n", i, mc);
		if (!(mc & (1 << crtc)))
			continue;

		switch ((mc & 0x00000f00) >> 8) {
		case 0: type = DCB_OUTPUT_LVDS; break;
		case 1: type = DCB_OUTPUT_TMDS; break;
		case 2: type = DCB_OUTPUT_TMDS; break;
		case 5: type = DCB_OUTPUT_TMDS; break;
		case 8: type = DCB_OUTPUT_DP; break;
		case 9: type = DCB_OUTPUT_DP; break;
		default:
			NV_ERROR(drm, "invalid mc, SOR-%d: 0x%08x\n", i, mc);
			goto ack;
		}

		or = i;
	}

	if (type == DCB_OUTPUT_ANY)
		goto ack;

	/* Enable the encoder */
	for (i = 0; i < drm->vbios.dcb.entries; i++) {
		dcb = &drm->vbios.dcb.entry[i];
		if (dcb->type == type && (dcb->or & (1 << or)))
			break;
	}

	if (i == drm->vbios.dcb.entries) {
		NV_ERROR(drm, "no dcb for %d %d 0x%08x\n", or, type, mc);
		goto ack;
	}

	script = nv50_display_script_select(dev, dcb, mc, pclk);
	nouveau_bios_run_display_table(dev, script, pclk, dcb, -1);

	if (type == DCB_OUTPUT_DP) {
		int link = !(dcb->dpconf.sor.link & 1);
		if ((mc & 0x000f0000) == 0x00020000)
			nv50_sor_dp_calc_tu(dev, or, link, pclk, 18);
		else
			nv50_sor_dp_calc_tu(dev, or, link, pclk, 24);
	}

	if (dcb->type != DCB_OUTPUT_ANALOG) {
		tmp = nv_rd32(device, NV50_PDISPLAY_SOR_CLK_CTRL2(or));
		tmp &= ~0x00000f0f;
		if (script & 0x0100)
			tmp |= 0x00000101;
		nv_wr32(device, NV50_PDISPLAY_SOR_CLK_CTRL2(or), tmp);
	} else {
		nv_wr32(device, NV50_PDISPLAY_DAC_CLK_CTRL2(or), 0);
	}

	disp->irq.dcb = dcb;
	disp->irq.pclk = pclk;
	disp->irq.script = script;

ack:
	nv_wr32(device, NV50_PDISPLAY_INTR_1, NV50_PDISPLAY_INTR_1_CLK_UNK20);
	nv_wr32(device, 0x610030, 0x80000000);
}

/* If programming a TMDS output on a SOR that can also be configured for
 * DisplayPort, make sure NV50_SOR_DP_CTRL_ENABLE is forced off.
 *
 * It looks like the VBIOS TMDS scripts make an attempt at this, however,
 * the VBIOS scripts on at least one board I have only switch it off on
 * link 0, causing a blank display if the output has previously been
 * programmed for DisplayPort.
 */
static void
nv50_display_unk40_dp_set_tmds(struct drm_device *dev, struct dcb_output *dcb)
{
	struct nouveau_device *device = nouveau_dev(dev);
	int or = ffs(dcb->or) - 1, link = !(dcb->dpconf.sor.link & 1);
	struct drm_encoder *encoder;
	u32 tmp;

	if (dcb->type != DCB_OUTPUT_TMDS)
		return;

	list_for_each_entry(encoder, &dev->mode_config.encoder_list, head) {
		struct nouveau_encoder *nv_encoder = nouveau_encoder(encoder);

		if (nv_encoder->dcb->type == DCB_OUTPUT_DP &&
		    nv_encoder->dcb->or & (1 << or)) {
			tmp  = nv_rd32(device, NV50_SOR_DP_CTRL(or, link));
			tmp &= ~NV50_SOR_DP_CTRL_ENABLED;
			nv_wr32(device, NV50_SOR_DP_CTRL(or, link), tmp);
			break;
		}
	}
}

static void
nv50_display_unk40_handler(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nv50_display *disp = nv50_display(dev);
	struct dcb_output *dcb = disp->irq.dcb;
	u16 script = disp->irq.script;
	u32 unk30 = nv_rd32(device, 0x610030), pclk = disp->irq.pclk;

	NV_DEBUG(drm, "0x610030: 0x%08x\n", unk30);
	disp->irq.dcb = NULL;
	if (!dcb)
		goto ack;

	nouveau_bios_run_display_table(dev, script, -pclk, dcb, -1);
	nv50_display_unk40_dp_set_tmds(dev, dcb);

ack:
	nv_wr32(device, NV50_PDISPLAY_INTR_1, NV50_PDISPLAY_INTR_1_CLK_UNK40);
	nv_wr32(device, 0x610030, 0x80000000);
	nv_wr32(device, 0x619494, nv_rd32(device, 0x619494) | 8);
}

static void
nv50_display_bh(unsigned long data)
{
	struct drm_device *dev = (struct drm_device *)data;
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_drm *drm = nouveau_drm(dev);

	for (;;) {
		uint32_t intr0 = nv_rd32(device, NV50_PDISPLAY_INTR_0);
		uint32_t intr1 = nv_rd32(device, NV50_PDISPLAY_INTR_1);

		NV_DEBUG(drm, "PDISPLAY_INTR_BH 0x%08x 0x%08x\n", intr0, intr1);

		if (intr1 & NV50_PDISPLAY_INTR_1_CLK_UNK10)
			nv50_display_unk10_handler(dev);
		else
		if (intr1 & NV50_PDISPLAY_INTR_1_CLK_UNK20)
			nv50_display_unk20_handler(dev);
		else
		if (intr1 & NV50_PDISPLAY_INTR_1_CLK_UNK40)
			nv50_display_unk40_handler(dev);
		else
			break;
	}

	nv_wr32(device, NV03_PMC_INTR_EN_0, 1);
}

static void
nv50_display_error_handler(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_drm *drm = nouveau_drm(dev);
	u32 channels = (nv_rd32(device, NV50_PDISPLAY_INTR_0) & 0x001f0000) >> 16;
	u32 addr, data;
	int chid;

	for (chid = 0; chid < 5; chid++) {
		if (!(channels & (1 << chid)))
			continue;

		nv_wr32(device, NV50_PDISPLAY_INTR_0, 0x00010000 << chid);
		addr = nv_rd32(device, NV50_PDISPLAY_TRAPPED_ADDR(chid));
		data = nv_rd32(device, NV50_PDISPLAY_TRAPPED_DATA(chid));
		NV_ERROR(drm, "EvoCh %d Mthd 0x%04x Data 0x%08x "
			      "(0x%04x 0x%02x)\n", chid,
			 addr & 0xffc, data, addr >> 16, (addr >> 12) & 0xf);

		nv_wr32(device, NV50_PDISPLAY_TRAPPED_ADDR(chid), 0x90000000);
	}
}

void
nv50_display_intr(struct drm_device *dev)
{
	struct nouveau_device *device = nouveau_dev(dev);
	struct nouveau_drm *drm = nouveau_drm(dev);
	struct nv50_display *disp = nv50_display(dev);
	uint32_t delayed = 0;

	while (nv_rd32(device, NV50_PMC_INTR_0) & NV50_PMC_INTR_0_DISPLAY) {
		uint32_t intr0 = nv_rd32(device, NV50_PDISPLAY_INTR_0);
		uint32_t intr1 = nv_rd32(device, NV50_PDISPLAY_INTR_1);
		uint32_t clock;

		NV_DEBUG(drm, "PDISPLAY_INTR 0x%08x 0x%08x\n", intr0, intr1);

		if (!intr0 && !(intr1 & ~delayed))
			break;

		if (intr0 & 0x001f0000) {
			nv50_display_error_handler(dev);
			intr0 &= ~0x001f0000;
		}

		if (intr1 & NV50_PDISPLAY_INTR_1_VBLANK_CRTC) {
			intr1 &= ~NV50_PDISPLAY_INTR_1_VBLANK_CRTC;
			delayed |= NV50_PDISPLAY_INTR_1_VBLANK_CRTC;
		}

		clock = (intr1 & (NV50_PDISPLAY_INTR_1_CLK_UNK10 |
				  NV50_PDISPLAY_INTR_1_CLK_UNK20 |
				  NV50_PDISPLAY_INTR_1_CLK_UNK40));
		if (clock) {
			nv_wr32(device, NV03_PMC_INTR_EN_0, 0);
			tasklet_schedule(&disp->tasklet);
			delayed |= clock;
			intr1 &= ~clock;
		}

		if (intr0) {
			NV_ERROR(drm, "unknown PDISPLAY_INTR_0: 0x%08x\n", intr0);
			nv_wr32(device, NV50_PDISPLAY_INTR_0, intr0);
		}

		if (intr1) {
			NV_ERROR(drm,
				 "unknown PDISPLAY_INTR_1: 0x%08x\n", intr1);
			nv_wr32(device, NV50_PDISPLAY_INTR_1, intr1);
		}
	}
}
