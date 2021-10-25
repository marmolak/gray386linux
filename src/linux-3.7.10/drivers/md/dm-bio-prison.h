/*
 * Copyright (C) 2011-2012 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */

#ifndef DM_BIO_PRISON_H
#define DM_BIO_PRISON_H

#include "persistent-data/dm-block-manager.h" /* FIXME: for dm_block_t */
#include "dm-thin-metadata.h" /* FIXME: for dm_thin_id */

#include <linux/list.h>
#include <linux/bio.h>

/*----------------------------------------------------------------*/

/*
 * Sometimes we can't deal with a bio straight away.  We put them in prison
 * where they can't cause any mischief.  Bios are put in a cell identified
 * by a key, multiple bios can be in the same cell.  When the cell is
 * subsequently unlocked the bios become available.
 */
struct dm_bio_prison;
struct dm_bio_prison_cell;

/* FIXME: this needs to be more abstract */
struct dm_cell_key {
	int virtual;
	dm_thin_id dev;
	dm_block_t block;
};

struct dm_bio_prison *dm_bio_prison_create(unsigned nr_cells);
void dm_bio_prison_destroy(struct dm_bio_prison *prison);

/*
 * This may block if a new cell needs allocating.  You must ensure that
 * cells will be unlocked even if the calling thread is blocked.
 *
 * Returns 1 if the cell was already held, 0 if @inmate is the new holder.
 */
int dm_bio_detain(struct dm_bio_prison *prison, struct dm_cell_key *key,
		  struct bio *inmate, struct dm_bio_prison_cell **ref);

void dm_cell_release(struct dm_bio_prison_cell *cell, struct bio_list *bios);
void dm_cell_release_no_holder(struct dm_bio_prison_cell *cell, struct bio_list *inmates);
void dm_cell_error(struct dm_bio_prison_cell *cell);

/*----------------------------------------------------------------*/

/*
 * We use the deferred set to keep track of pending reads to shared blocks.
 * We do this to ensure the new mapping caused by a write isn't performed
 * until these prior reads have completed.  Otherwise the insertion of the
 * new mapping could free the old block that the read bios are mapped to.
 */

struct dm_deferred_set;
struct dm_deferred_entry;

struct dm_deferred_set *dm_deferred_set_create(void);
void dm_deferred_set_destroy(struct dm_deferred_set *ds);

struct dm_deferred_entry *dm_deferred_entry_inc(struct dm_deferred_set *ds);
void dm_deferred_entry_dec(struct dm_deferred_entry *entry, struct list_head *head);
int dm_deferred_set_add_work(struct dm_deferred_set *ds, struct list_head *work);

/*----------------------------------------------------------------*/

#endif
