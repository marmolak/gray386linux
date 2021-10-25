/* Copyright (c) 2012 Coraid, Inc.  See COPYING for GPL terms. */
#define VERSION "50"
#define AOE_MAJOR 152
#define DEVICE_NAME "aoe"

/* set AOE_PARTITIONS to 1 to use whole-disks only
 * default is 16, which is 15 partitions plus the whole disk
 */
#ifndef AOE_PARTITIONS
#define AOE_PARTITIONS (16)
#endif

#define WHITESPACE " \t\v\f\n"

enum {
	AOECMD_ATA,
	AOECMD_CFG,
	AOECMD_VEND_MIN = 0xf0,

	AOEFL_RSP = (1<<3),
	AOEFL_ERR = (1<<2),

	AOEAFL_EXT = (1<<6),
	AOEAFL_DEV = (1<<4),
	AOEAFL_ASYNC = (1<<1),
	AOEAFL_WRITE = (1<<0),

	AOECCMD_READ = 0,
	AOECCMD_TEST,
	AOECCMD_PTEST,
	AOECCMD_SET,
	AOECCMD_FSET,

	AOE_HVER = 0x10,
};

struct aoe_hdr {
	unsigned char dst[6];
	unsigned char src[6];
	__be16 type;
	unsigned char verfl;
	unsigned char err;
	__be16 major;
	unsigned char minor;
	unsigned char cmd;
	__be32 tag;
};

struct aoe_atahdr {
	unsigned char aflags;
	unsigned char errfeat;
	unsigned char scnt;
	unsigned char cmdstat;
	unsigned char lba0;
	unsigned char lba1;
	unsigned char lba2;
	unsigned char lba3;
	unsigned char lba4;
	unsigned char lba5;
	unsigned char res[2];
};

struct aoe_cfghdr {
	__be16 bufcnt;
	__be16 fwver;
	unsigned char scnt;
	unsigned char aoeccmd;
	unsigned char cslen[2];
};

enum {
	DEVFL_UP = 1,	/* device is installed in system and ready for AoE->ATA commands */
	DEVFL_TKILL = (1<<1),	/* flag for timer to know when to kill self */
	DEVFL_EXT = (1<<2),	/* device accepts lba48 commands */
	DEVFL_GDALLOC = (1<<3),	/* need to alloc gendisk */
	DEVFL_KICKME = (1<<4),	/* slow polling network card catch */
	DEVFL_NEWSIZE = (1<<5),	/* need to update dev size in block layer */
};

enum {
	DEFAULTBCNT = 2 * 512,	/* 2 sectors */
	MIN_BUFS = 16,
	NTARGETS = 8,
	NAOEIFS = 8,
	NSKBPOOLMAX = 256,
	NFACTIVE = 61,

	TIMERTICK = HZ / 10,
	MINTIMER = HZ >> 2,
	MAXTIMER = HZ << 1,
};

struct buf {
	ulong nframesout;
	ulong resid;
	ulong bv_resid;
	sector_t sector;
	struct bio *bio;
	struct bio_vec *bv;
	struct request *rq;
};

struct frame {
	struct list_head head;
	u32 tag;
	ulong waited;
	struct aoetgt *t;		/* parent target I belong to */
	sector_t lba;
	struct sk_buff *skb;		/* command skb freed on module exit */
	struct sk_buff *r_skb;		/* response skb for async processing */
	struct buf *buf;
	struct bio_vec *bv;
	ulong bcnt;
	ulong bv_off;
};

struct aoeif {
	struct net_device *nd;
	ulong lost;
	int bcnt;
};

struct aoetgt {
	unsigned char addr[6];
	ushort nframes;
	struct aoedev *d;			/* parent device I belong to */
	struct list_head ffree;			/* list of free frames */
	struct aoeif ifs[NAOEIFS];
	struct aoeif *ifp;	/* current aoeif in use */
	ushort nout;
	ushort maxout;
	ulong falloc;
	ulong lastwadj;		/* last window adjustment */
	int minbcnt;
	int wpkts, rpkts;
};

struct aoedev {
	struct aoedev *next;
	ulong sysminor;
	ulong aoemajor;
	u16 aoeminor;
	u16 flags;
	u16 nopen;		/* (bd_openers isn't available without sleeping) */
	u16 rttavg;		/* round trip average of requests/responses */
	u16 mintimer;
	u16 fw_ver;		/* version of blade's firmware */
	u16 lasttag;		/* last tag sent */
	u16 useme;
	ulong ref;
	struct work_struct work;/* disk create work struct */
	struct gendisk *gd;
	struct request_queue *blkq;
	struct hd_geometry geo; 
	sector_t ssize;
	struct timer_list timer;
	spinlock_t lock;
	struct sk_buff_head skbpool;
	mempool_t *bufpool;	/* for deadlock-free Buf allocation */
	struct {		/* pointers to work in progress */
		struct buf *buf;
		struct bio *nxbio;
		struct request *rq;
	} ip;
	ulong maxbcnt;
	struct list_head factive[NFACTIVE];	/* hash of active frames */
	struct aoetgt *targets[NTARGETS];
	struct aoetgt **tgt;	/* target in use when working */
	struct aoetgt *htgt;	/* target needing rexmit assistance */
	ulong ntargets;
	ulong kicked;
};

/* kthread tracking */
struct ktstate {
	struct completion rendez;
	struct task_struct *task;
	wait_queue_head_t *waitq;
	int (*fn) (void);
	char *name;
	spinlock_t *lock;
};

int aoeblk_init(void);
void aoeblk_exit(void);
void aoeblk_gdalloc(void *);
void aoedisk_rm_sysfs(struct aoedev *d);

int aoechr_init(void);
void aoechr_exit(void);
void aoechr_error(char *);

void aoecmd_work(struct aoedev *d);
void aoecmd_cfg(ushort aoemajor, unsigned char aoeminor);
struct sk_buff *aoecmd_ata_rsp(struct sk_buff *);
void aoecmd_cfg_rsp(struct sk_buff *);
void aoecmd_sleepwork(struct work_struct *);
void aoecmd_cleanslate(struct aoedev *);
void aoecmd_exit(void);
int aoecmd_init(void);
struct sk_buff *aoecmd_ata_id(struct aoedev *);
void aoe_freetframe(struct frame *);
void aoe_flush_iocq(void);
void aoe_end_request(struct aoedev *, struct request *, int);
int aoe_ktstart(struct ktstate *k);
void aoe_ktstop(struct ktstate *k);

int aoedev_init(void);
void aoedev_exit(void);
struct aoedev *aoedev_by_aoeaddr(ulong maj, int min, int do_alloc);
void aoedev_downdev(struct aoedev *d);
int aoedev_flush(const char __user *str, size_t size);
void aoe_failbuf(struct aoedev *, struct buf *);
void aoedev_put(struct aoedev *);

int aoenet_init(void);
void aoenet_exit(void);
void aoenet_xmit(struct sk_buff_head *);
int is_aoe_netif(struct net_device *ifp);
int set_aoe_iflist(const char __user *str, size_t size);
