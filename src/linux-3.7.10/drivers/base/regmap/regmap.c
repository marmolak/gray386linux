/*
 * Register map access API
 *
 * Copyright 2011 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/device.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/rbtree.h>

#define CREATE_TRACE_POINTS
#include <trace/events/regmap.h>

#include "internal.h"

/*
 * Sometimes for failures during very early init the trace
 * infrastructure isn't available early enough to be used.  For this
 * sort of problem defining LOG_DEVICE will add printks for basic
 * register I/O on a specific device.
 */
#undef LOG_DEVICE

static int _regmap_update_bits(struct regmap *map, unsigned int reg,
			       unsigned int mask, unsigned int val,
			       bool *change);

bool regmap_writeable(struct regmap *map, unsigned int reg)
{
	if (map->max_register && reg > map->max_register)
		return false;

	if (map->writeable_reg)
		return map->writeable_reg(map->dev, reg);

	return true;
}

bool regmap_readable(struct regmap *map, unsigned int reg)
{
	if (map->max_register && reg > map->max_register)
		return false;

	if (map->format.format_write)
		return false;

	if (map->readable_reg)
		return map->readable_reg(map->dev, reg);

	return true;
}

bool regmap_volatile(struct regmap *map, unsigned int reg)
{
	if (!regmap_readable(map, reg))
		return false;

	if (map->volatile_reg)
		return map->volatile_reg(map->dev, reg);

	return true;
}

bool regmap_precious(struct regmap *map, unsigned int reg)
{
	if (!regmap_readable(map, reg))
		return false;

	if (map->precious_reg)
		return map->precious_reg(map->dev, reg);

	return false;
}

static bool regmap_volatile_range(struct regmap *map, unsigned int reg,
	unsigned int num)
{
	unsigned int i;

	for (i = 0; i < num; i++)
		if (!regmap_volatile(map, reg + i))
			return false;

	return true;
}

static void regmap_format_2_6_write(struct regmap *map,
				     unsigned int reg, unsigned int val)
{
	u8 *out = map->work_buf;

	*out = (reg << 6) | val;
}

static void regmap_format_4_12_write(struct regmap *map,
				     unsigned int reg, unsigned int val)
{
	__be16 *out = map->work_buf;
	*out = cpu_to_be16((reg << 12) | val);
}

static void regmap_format_7_9_write(struct regmap *map,
				    unsigned int reg, unsigned int val)
{
	__be16 *out = map->work_buf;
	*out = cpu_to_be16((reg << 9) | val);
}

static void regmap_format_10_14_write(struct regmap *map,
				    unsigned int reg, unsigned int val)
{
	u8 *out = map->work_buf;

	out[2] = val;
	out[1] = (val >> 8) | (reg << 6);
	out[0] = reg >> 2;
}

static void regmap_format_8(void *buf, unsigned int val, unsigned int shift)
{
	u8 *b = buf;

	b[0] = val << shift;
}

static void regmap_format_16_be(void *buf, unsigned int val, unsigned int shift)
{
	__be16 *b = buf;

	b[0] = cpu_to_be16(val << shift);
}

static void regmap_format_16_native(void *buf, unsigned int val,
				    unsigned int shift)
{
	*(u16 *)buf = val << shift;
}

static void regmap_format_24(void *buf, unsigned int val, unsigned int shift)
{
	u8 *b = buf;

	val <<= shift;

	b[0] = val >> 16;
	b[1] = val >> 8;
	b[2] = val;
}

static void regmap_format_32_be(void *buf, unsigned int val, unsigned int shift)
{
	__be32 *b = buf;

	b[0] = cpu_to_be32(val << shift);
}

static void regmap_format_32_native(void *buf, unsigned int val,
				    unsigned int shift)
{
	*(u32 *)buf = val << shift;
}

static unsigned int regmap_parse_8(void *buf)
{
	u8 *b = buf;

	return b[0];
}

static unsigned int regmap_parse_16_be(void *buf)
{
	__be16 *b = buf;

	b[0] = be16_to_cpu(b[0]);

	return b[0];
}

static unsigned int regmap_parse_16_native(void *buf)
{
	return *(u16 *)buf;
}

static unsigned int regmap_parse_24(void *buf)
{
	u8 *b = buf;
	unsigned int ret = b[2];
	ret |= ((unsigned int)b[1]) << 8;
	ret |= ((unsigned int)b[0]) << 16;

	return ret;
}

static unsigned int regmap_parse_32_be(void *buf)
{
	__be32 *b = buf;

	b[0] = be32_to_cpu(b[0]);

	return b[0];
}

static unsigned int regmap_parse_32_native(void *buf)
{
	return *(u32 *)buf;
}

static void regmap_lock_mutex(struct regmap *map)
{
	mutex_lock(&map->mutex);
}

static void regmap_unlock_mutex(struct regmap *map)
{
	mutex_unlock(&map->mutex);
}

static void regmap_lock_spinlock(struct regmap *map)
{
	spin_lock(&map->spinlock);
}

static void regmap_unlock_spinlock(struct regmap *map)
{
	spin_unlock(&map->spinlock);
}

static void dev_get_regmap_release(struct device *dev, void *res)
{
	/*
	 * We don't actually have anything to do here; the goal here
	 * is not to manage the regmap but to provide a simple way to
	 * get the regmap back given a struct device.
	 */
}

static bool _regmap_range_add(struct regmap *map,
			      struct regmap_range_node *data)
{
	struct rb_root *root = &map->range_tree;
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while (*new) {
		struct regmap_range_node *this =
			container_of(*new, struct regmap_range_node, node);

		parent = *new;
		if (data->range_max < this->range_min)
			new = &((*new)->rb_left);
		else if (data->range_min > this->range_max)
			new = &((*new)->rb_right);
		else
			return false;
	}

	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return true;
}

static struct regmap_range_node *_regmap_range_lookup(struct regmap *map,
						      unsigned int reg)
{
	struct rb_node *node = map->range_tree.rb_node;

	while (node) {
		struct regmap_range_node *this =
			container_of(node, struct regmap_range_node, node);

		if (reg < this->range_min)
			node = node->rb_left;
		else if (reg > this->range_max)
			node = node->rb_right;
		else
			return this;
	}

	return NULL;
}

static void regmap_range_exit(struct regmap *map)
{
	struct rb_node *next;
	struct regmap_range_node *range_node;

	next = rb_first(&map->range_tree);
	while (next) {
		range_node = rb_entry(next, struct regmap_range_node, node);
		next = rb_next(&range_node->node);
		rb_erase(&range_node->node, &map->range_tree);
		kfree(range_node);
	}

	kfree(map->selector_work_buf);
}

/**
 * regmap_init(): Initialise register map
 *
 * @dev: Device that will be interacted with
 * @bus: Bus-specific callbacks to use with device
 * @bus_context: Data passed to bus-specific callbacks
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer to
 * a struct regmap.  This function should generally not be called
 * directly, it should be called by bus-specific init functions.
 */
struct regmap *regmap_init(struct device *dev,
			   const struct regmap_bus *bus,
			   void *bus_context,
			   const struct regmap_config *config)
{
	struct regmap *map, **m;
	int ret = -EINVAL;
	enum regmap_endian reg_endian, val_endian;
	int i, j;

	if (!bus || !config)
		goto err;

	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (map == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	if (bus->fast_io) {
		spin_lock_init(&map->spinlock);
		map->lock = regmap_lock_spinlock;
		map->unlock = regmap_unlock_spinlock;
	} else {
		mutex_init(&map->mutex);
		map->lock = regmap_lock_mutex;
		map->unlock = regmap_unlock_mutex;
	}
	map->format.reg_bytes = DIV_ROUND_UP(config->reg_bits, 8);
	map->format.pad_bytes = config->pad_bits / 8;
	map->format.val_bytes = DIV_ROUND_UP(config->val_bits, 8);
	map->format.buf_size = DIV_ROUND_UP(config->reg_bits +
			config->val_bits + config->pad_bits, 8);
	map->reg_shift = config->pad_bits % 8;
	if (config->reg_stride)
		map->reg_stride = config->reg_stride;
	else
		map->reg_stride = 1;
	map->use_single_rw = config->use_single_rw;
	map->dev = dev;
	map->bus = bus;
	map->bus_context = bus_context;
	map->max_register = config->max_register;
	map->writeable_reg = config->writeable_reg;
	map->readable_reg = config->readable_reg;
	map->volatile_reg = config->volatile_reg;
	map->precious_reg = config->precious_reg;
	map->cache_type = config->cache_type;
	map->name = config->name;

	if (config->read_flag_mask || config->write_flag_mask) {
		map->read_flag_mask = config->read_flag_mask;
		map->write_flag_mask = config->write_flag_mask;
	} else {
		map->read_flag_mask = bus->read_flag_mask;
	}

	reg_endian = config->reg_format_endian;
	if (reg_endian == REGMAP_ENDIAN_DEFAULT)
		reg_endian = bus->reg_format_endian_default;
	if (reg_endian == REGMAP_ENDIAN_DEFAULT)
		reg_endian = REGMAP_ENDIAN_BIG;

	val_endian = config->val_format_endian;
	if (val_endian == REGMAP_ENDIAN_DEFAULT)
		val_endian = bus->val_format_endian_default;
	if (val_endian == REGMAP_ENDIAN_DEFAULT)
		val_endian = REGMAP_ENDIAN_BIG;

	switch (config->reg_bits + map->reg_shift) {
	case 2:
		switch (config->val_bits) {
		case 6:
			map->format.format_write = regmap_format_2_6_write;
			break;
		default:
			goto err_map;
		}
		break;

	case 4:
		switch (config->val_bits) {
		case 12:
			map->format.format_write = regmap_format_4_12_write;
			break;
		default:
			goto err_map;
		}
		break;

	case 7:
		switch (config->val_bits) {
		case 9:
			map->format.format_write = regmap_format_7_9_write;
			break;
		default:
			goto err_map;
		}
		break;

	case 10:
		switch (config->val_bits) {
		case 14:
			map->format.format_write = regmap_format_10_14_write;
			break;
		default:
			goto err_map;
		}
		break;

	case 8:
		map->format.format_reg = regmap_format_8;
		break;

	case 16:
		switch (reg_endian) {
		case REGMAP_ENDIAN_BIG:
			map->format.format_reg = regmap_format_16_be;
			break;
		case REGMAP_ENDIAN_NATIVE:
			map->format.format_reg = regmap_format_16_native;
			break;
		default:
			goto err_map;
		}
		break;

	case 32:
		switch (reg_endian) {
		case REGMAP_ENDIAN_BIG:
			map->format.format_reg = regmap_format_32_be;
			break;
		case REGMAP_ENDIAN_NATIVE:
			map->format.format_reg = regmap_format_32_native;
			break;
		default:
			goto err_map;
		}
		break;

	default:
		goto err_map;
	}

	switch (config->val_bits) {
	case 8:
		map->format.format_val = regmap_format_8;
		map->format.parse_val = regmap_parse_8;
		break;
	case 16:
		switch (val_endian) {
		case REGMAP_ENDIAN_BIG:
			map->format.format_val = regmap_format_16_be;
			map->format.parse_val = regmap_parse_16_be;
			break;
		case REGMAP_ENDIAN_NATIVE:
			map->format.format_val = regmap_format_16_native;
			map->format.parse_val = regmap_parse_16_native;
			break;
		default:
			goto err_map;
		}
		break;
	case 24:
		if (val_endian != REGMAP_ENDIAN_BIG)
			goto err_map;
		map->format.format_val = regmap_format_24;
		map->format.parse_val = regmap_parse_24;
		break;
	case 32:
		switch (val_endian) {
		case REGMAP_ENDIAN_BIG:
			map->format.format_val = regmap_format_32_be;
			map->format.parse_val = regmap_parse_32_be;
			break;
		case REGMAP_ENDIAN_NATIVE:
			map->format.format_val = regmap_format_32_native;
			map->format.parse_val = regmap_parse_32_native;
			break;
		default:
			goto err_map;
		}
		break;
	}

	if (map->format.format_write) {
		if ((reg_endian != REGMAP_ENDIAN_BIG) ||
		    (val_endian != REGMAP_ENDIAN_BIG))
			goto err_map;
		map->use_single_rw = true;
	}

	if (!map->format.format_write &&
	    !(map->format.format_reg && map->format.format_val))
		goto err_map;

	map->work_buf = kzalloc(map->format.buf_size, GFP_KERNEL);
	if (map->work_buf == NULL) {
		ret = -ENOMEM;
		goto err_map;
	}

	map->range_tree = RB_ROOT;
	for (i = 0; i < config->n_ranges; i++) {
		const struct regmap_range_cfg *range_cfg = &config->ranges[i];
		struct regmap_range_node *new;

		/* Sanity check */
		if (range_cfg->range_max < range_cfg->range_min ||
		    range_cfg->range_max > map->max_register ||
		    range_cfg->selector_reg > map->max_register ||
		    range_cfg->window_len == 0)
			goto err_range;

		/* Make sure, that this register range has no selector
		   or data window within its boundary */
		for (j = 0; j < config->n_ranges; j++) {
			unsigned sel_reg = config->ranges[j].selector_reg;
			unsigned win_min = config->ranges[j].window_start;
			unsigned win_max = win_min +
					   config->ranges[j].window_len - 1;

			if (range_cfg->range_min <= sel_reg &&
			    sel_reg <= range_cfg->range_max) {
				goto err_range;
			}

			if (!(win_max < range_cfg->range_min ||
			      win_min > range_cfg->range_max)) {
				goto err_range;
			}
		}

		new = kzalloc(sizeof(*new), GFP_KERNEL);
		if (new == NULL) {
			ret = -ENOMEM;
			goto err_range;
		}

		new->range_min = range_cfg->range_min;
		new->range_max = range_cfg->range_max;
		new->selector_reg = range_cfg->selector_reg;
		new->selector_mask = range_cfg->selector_mask;
		new->selector_shift = range_cfg->selector_shift;
		new->window_start = range_cfg->window_start;
		new->window_len = range_cfg->window_len;

		if (_regmap_range_add(map, new) == false) {
			kfree(new);
			goto err_range;
		}

		if (map->selector_work_buf == NULL) {
			map->selector_work_buf =
				kzalloc(map->format.buf_size, GFP_KERNEL);
			if (map->selector_work_buf == NULL) {
				ret = -ENOMEM;
				goto err_range;
			}
		}
	}

	ret = regcache_init(map, config);
	if (ret < 0)
		goto err_range;

	regmap_debugfs_init(map, config->name);

	/* Add a devres resource for dev_get_regmap() */
	m = devres_alloc(dev_get_regmap_release, sizeof(*m), GFP_KERNEL);
	if (!m) {
		ret = -ENOMEM;
		goto err_debugfs;
	}
	*m = map;
	devres_add(dev, m);

	return map;

err_debugfs:
	regmap_debugfs_exit(map);
	regcache_exit(map);
err_range:
	regmap_range_exit(map);
	kfree(map->work_buf);
err_map:
	kfree(map);
err:
	return ERR_PTR(ret);
}
EXPORT_SYMBOL_GPL(regmap_init);

static void devm_regmap_release(struct device *dev, void *res)
{
	regmap_exit(*(struct regmap **)res);
}

/**
 * devm_regmap_init(): Initialise managed register map
 *
 * @dev: Device that will be interacted with
 * @bus: Bus-specific callbacks to use with device
 * @bus_context: Data passed to bus-specific callbacks
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer
 * to a struct regmap.  This function should generally not be called
 * directly, it should be called by bus-specific init functions.  The
 * map will be automatically freed by the device management code.
 */
struct regmap *devm_regmap_init(struct device *dev,
				const struct regmap_bus *bus,
				void *bus_context,
				const struct regmap_config *config)
{
	struct regmap **ptr, *regmap;

	ptr = devres_alloc(devm_regmap_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return ERR_PTR(-ENOMEM);

	regmap = regmap_init(dev, bus, bus_context, config);
	if (!IS_ERR(regmap)) {
		*ptr = regmap;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return regmap;
}
EXPORT_SYMBOL_GPL(devm_regmap_init);

/**
 * regmap_reinit_cache(): Reinitialise the current register cache
 *
 * @map: Register map to operate on.
 * @config: New configuration.  Only the cache data will be used.
 *
 * Discard any existing register cache for the map and initialize a
 * new cache.  This can be used to restore the cache to defaults or to
 * update the cache configuration to reflect runtime discovery of the
 * hardware.
 *
 * No explicit locking is done here, the user needs to ensure that
 * this function will not race with other calls to regmap.
 */
int regmap_reinit_cache(struct regmap *map, const struct regmap_config *config)
{
	regcache_exit(map);
	regmap_debugfs_exit(map);

	map->max_register = config->max_register;
	map->writeable_reg = config->writeable_reg;
	map->readable_reg = config->readable_reg;
	map->volatile_reg = config->volatile_reg;
	map->precious_reg = config->precious_reg;
	map->cache_type = config->cache_type;

	regmap_debugfs_init(map, config->name);

	map->cache_bypass = false;
	map->cache_only = false;

	return regcache_init(map, config);
}
EXPORT_SYMBOL_GPL(regmap_reinit_cache);

/**
 * regmap_exit(): Free a previously allocated register map
 */
void regmap_exit(struct regmap *map)
{
	regcache_exit(map);
	regmap_debugfs_exit(map);
	regmap_range_exit(map);
	if (map->bus->free_context)
		map->bus->free_context(map->bus_context);
	kfree(map->work_buf);
	kfree(map);
}
EXPORT_SYMBOL_GPL(regmap_exit);

static int dev_get_regmap_match(struct device *dev, void *res, void *data)
{
	struct regmap **r = res;
	if (!r || !*r) {
		WARN_ON(!r || !*r);
		return 0;
	}

	/* If the user didn't specify a name match any */
	if (data)
		return (*r)->name == data;
	else
		return 1;
}

/**
 * dev_get_regmap(): Obtain the regmap (if any) for a device
 *
 * @dev: Device to retrieve the map for
 * @name: Optional name for the register map, usually NULL.
 *
 * Returns the regmap for the device if one is present, or NULL.  If
 * name is specified then it must match the name specified when
 * registering the device, if it is NULL then the first regmap found
 * will be used.  Devices with multiple register maps are very rare,
 * generic code should normally not need to specify a name.
 */
struct regmap *dev_get_regmap(struct device *dev, const char *name)
{
	struct regmap **r = devres_find(dev, dev_get_regmap_release,
					dev_get_regmap_match, (void *)name);

	if (!r)
		return NULL;
	return *r;
}
EXPORT_SYMBOL_GPL(dev_get_regmap);

static int _regmap_select_page(struct regmap *map, unsigned int *reg,
			       unsigned int val_num)
{
	struct regmap_range_node *range;
	void *orig_work_buf;
	unsigned int win_offset;
	unsigned int win_page;
	bool page_chg;
	int ret;

	range = _regmap_range_lookup(map, *reg);
	if (range) {
		win_offset = (*reg - range->range_min) % range->window_len;
		win_page = (*reg - range->range_min) / range->window_len;

		if (val_num > 1) {
			/* Bulk write shouldn't cross range boundary */
			if (*reg + val_num - 1 > range->range_max)
				return -EINVAL;

			/* ... or single page boundary */
			if (val_num > range->window_len - win_offset)
				return -EINVAL;
		}

		/* It is possible to have selector register inside data window.
		   In that case, selector register is located on every page and
		   it needs no page switching, when accessed alone. */
		if (val_num > 1 ||
		    range->window_start + win_offset != range->selector_reg) {
			/* Use separate work_buf during page switching */
			orig_work_buf = map->work_buf;
			map->work_buf = map->selector_work_buf;

			ret = _regmap_update_bits(map, range->selector_reg,
					range->selector_mask,
					win_page << range->selector_shift,
					&page_chg);

			map->work_buf = orig_work_buf;

			if (ret < 0)
				return ret;
		}

		*reg = range->window_start + win_offset;
	}

	return 0;
}

static int _regmap_raw_write(struct regmap *map, unsigned int reg,
			     const void *val, size_t val_len)
{
	u8 *u8 = map->work_buf;
	void *buf;
	int ret = -ENOTSUPP;
	size_t len;
	int i;

	/* Check for unwritable registers before we start */
	if (map->writeable_reg)
		for (i = 0; i < val_len / map->format.val_bytes; i++)
			if (!map->writeable_reg(map->dev,
						reg + (i * map->reg_stride)))
				return -EINVAL;

	if (!map->cache_bypass && map->format.parse_val) {
		unsigned int ival;
		int val_bytes = map->format.val_bytes;
		for (i = 0; i < val_len / val_bytes; i++) {
			memcpy(map->work_buf, val + (i * val_bytes), val_bytes);
			ival = map->format.parse_val(map->work_buf);
			ret = regcache_write(map, reg + (i * map->reg_stride),
					     ival);
			if (ret) {
				dev_err(map->dev,
				   "Error in caching of register: %u ret: %d\n",
					reg + i, ret);
				return ret;
			}
		}
		if (map->cache_only) {
			map->cache_dirty = true;
			return 0;
		}
	}

	ret = _regmap_select_page(map, &reg, val_len / map->format.val_bytes);
	if (ret < 0)
		return ret;

	map->format.format_reg(map->work_buf, reg, map->reg_shift);

	u8[0] |= map->write_flag_mask;

	trace_regmap_hw_write_start(map->dev, reg,
				    val_len / map->format.val_bytes);

	/* If we're doing a single register write we can probably just
	 * send the work_buf directly, otherwise try to do a gather
	 * write.
	 */
	if (val == (map->work_buf + map->format.pad_bytes +
		    map->format.reg_bytes))
		ret = map->bus->write(map->bus_context, map->work_buf,
				      map->format.reg_bytes +
				      map->format.pad_bytes +
				      val_len);
	else if (map->bus->gather_write)
		ret = map->bus->gather_write(map->bus_context, map->work_buf,
					     map->format.reg_bytes +
					     map->format.pad_bytes,
					     val, val_len);

	/* If that didn't work fall back on linearising by hand. */
	if (ret == -ENOTSUPP) {
		len = map->format.reg_bytes + map->format.pad_bytes + val_len;
		buf = kzalloc(len, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		memcpy(buf, map->work_buf, map->format.reg_bytes);
		memcpy(buf + map->format.reg_bytes + map->format.pad_bytes,
		       val, val_len);
		ret = map->bus->write(map->bus_context, buf, len);

		kfree(buf);
	}

	trace_regmap_hw_write_done(map->dev, reg,
				   val_len / map->format.val_bytes);

	return ret;
}

int _regmap_write(struct regmap *map, unsigned int reg,
		  unsigned int val)
{
	int ret;
	BUG_ON(!map->format.format_write && !map->format.format_val);

	if (!map->cache_bypass && map->format.format_write) {
		ret = regcache_write(map, reg, val);
		if (ret != 0)
			return ret;
		if (map->cache_only) {
			map->cache_dirty = true;
			return 0;
		}
	}

#ifdef LOG_DEVICE
	if (strcmp(dev_name(map->dev), LOG_DEVICE) == 0)
		dev_info(map->dev, "%x <= %x\n", reg, val);
#endif

	trace_regmap_reg_write(map->dev, reg, val);

	if (map->format.format_write) {
		ret = _regmap_select_page(map, &reg, 1);
		if (ret < 0)
			return ret;

		map->format.format_write(map, reg, val);

		trace_regmap_hw_write_start(map->dev, reg, 1);

		ret = map->bus->write(map->bus_context, map->work_buf,
				      map->format.buf_size);

		trace_regmap_hw_write_done(map->dev, reg, 1);

		return ret;
	} else {
		map->format.format_val(map->work_buf + map->format.reg_bytes
				       + map->format.pad_bytes, val, 0);
		return _regmap_raw_write(map, reg,
					 map->work_buf +
					 map->format.reg_bytes +
					 map->format.pad_bytes,
					 map->format.val_bytes);
	}
}

/**
 * regmap_write(): Write a value to a single register
 *
 * @map: Register map to write to
 * @reg: Register to write to
 * @val: Value to be written
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_write(struct regmap *map, unsigned int reg, unsigned int val)
{
	int ret;

	if (reg % map->reg_stride)
		return -EINVAL;

	map->lock(map);

	ret = _regmap_write(map, reg, val);

	map->unlock(map);

	return ret;
}
EXPORT_SYMBOL_GPL(regmap_write);

/**
 * regmap_raw_write(): Write raw values to one or more registers
 *
 * @map: Register map to write to
 * @reg: Initial register to write to
 * @val: Block of data to be written, laid out for direct transmission to the
 *       device
 * @val_len: Length of data pointed to by val.
 *
 * This function is intended to be used for things like firmware
 * download where a large block of data needs to be transferred to the
 * device.  No formatting will be done on the data provided.
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_raw_write(struct regmap *map, unsigned int reg,
		     const void *val, size_t val_len)
{
	int ret;

	if (val_len % map->format.val_bytes)
		return -EINVAL;
	if (reg % map->reg_stride)
		return -EINVAL;

	map->lock(map);

	ret = _regmap_raw_write(map, reg, val, val_len);

	map->unlock(map);

	return ret;
}
EXPORT_SYMBOL_GPL(regmap_raw_write);

/*
 * regmap_bulk_write(): Write multiple registers to the device
 *
 * @map: Register map to write to
 * @reg: First register to be write from
 * @val: Block of data to be written, in native register size for device
 * @val_count: Number of registers to write
 *
 * This function is intended to be used for writing a large block of
 * data to be device either in single transfer or multiple transfer.
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_bulk_write(struct regmap *map, unsigned int reg, const void *val,
		     size_t val_count)
{
	int ret = 0, i;
	size_t val_bytes = map->format.val_bytes;
	void *wval;

	if (!map->format.parse_val)
		return -EINVAL;
	if (reg % map->reg_stride)
		return -EINVAL;

	map->lock(map);

	/* No formatting is require if val_byte is 1 */
	if (val_bytes == 1) {
		wval = (void *)val;
	} else {
		wval = kmemdup(val, val_count * val_bytes, GFP_KERNEL);
		if (!wval) {
			ret = -ENOMEM;
			dev_err(map->dev, "Error in memory allocation\n");
			goto out;
		}
		for (i = 0; i < val_count * val_bytes; i += val_bytes)
			map->format.parse_val(wval + i);
	}
	/*
	 * Some devices does not support bulk write, for
	 * them we have a series of single write operations.
	 */
	if (map->use_single_rw) {
		for (i = 0; i < val_count; i++) {
			ret = regmap_raw_write(map,
						reg + (i * map->reg_stride),
						val + (i * val_bytes),
						val_bytes);
			if (ret != 0)
				return ret;
		}
	} else {
		ret = _regmap_raw_write(map, reg, wval, val_bytes * val_count);
	}

	if (val_bytes != 1)
		kfree(wval);

out:
	map->unlock(map);
	return ret;
}
EXPORT_SYMBOL_GPL(regmap_bulk_write);

static int _regmap_raw_read(struct regmap *map, unsigned int reg, void *val,
			    unsigned int val_len)
{
	u8 *u8 = map->work_buf;
	int ret;

	ret = _regmap_select_page(map, &reg, val_len / map->format.val_bytes);
	if (ret < 0)
		return ret;

	map->format.format_reg(map->work_buf, reg, map->reg_shift);

	/*
	 * Some buses or devices flag reads by setting the high bits in the
	 * register addresss; since it's always the high bits for all
	 * current formats we can do this here rather than in
	 * formatting.  This may break if we get interesting formats.
	 */
	u8[0] |= map->read_flag_mask;

	trace_regmap_hw_read_start(map->dev, reg,
				   val_len / map->format.val_bytes);

	ret = map->bus->read(map->bus_context, map->work_buf,
			     map->format.reg_bytes + map->format.pad_bytes,
			     val, val_len);

	trace_regmap_hw_read_done(map->dev, reg,
				  val_len / map->format.val_bytes);

	return ret;
}

static int _regmap_read(struct regmap *map, unsigned int reg,
			unsigned int *val)
{
	int ret;

	if (!map->cache_bypass) {
		ret = regcache_read(map, reg, val);
		if (ret == 0)
			return 0;
	}

	if (!map->format.parse_val)
		return -EINVAL;

	if (map->cache_only)
		return -EBUSY;

	ret = _regmap_raw_read(map, reg, map->work_buf, map->format.val_bytes);
	if (ret == 0) {
		*val = map->format.parse_val(map->work_buf);

#ifdef LOG_DEVICE
		if (strcmp(dev_name(map->dev), LOG_DEVICE) == 0)
			dev_info(map->dev, "%x => %x\n", reg, *val);
#endif

		trace_regmap_reg_read(map->dev, reg, *val);
	}

	if (ret == 0 && !map->cache_bypass)
		regcache_write(map, reg, *val);

	return ret;
}

/**
 * regmap_read(): Read a value from a single register
 *
 * @map: Register map to write to
 * @reg: Register to be read from
 * @val: Pointer to store read value
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_read(struct regmap *map, unsigned int reg, unsigned int *val)
{
	int ret;

	if (reg % map->reg_stride)
		return -EINVAL;

	map->lock(map);

	ret = _regmap_read(map, reg, val);

	map->unlock(map);

	return ret;
}
EXPORT_SYMBOL_GPL(regmap_read);

/**
 * regmap_raw_read(): Read raw data from the device
 *
 * @map: Register map to write to
 * @reg: First register to be read from
 * @val: Pointer to store read value
 * @val_len: Size of data to read
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_raw_read(struct regmap *map, unsigned int reg, void *val,
		    size_t val_len)
{
	size_t val_bytes = map->format.val_bytes;
	size_t val_count = val_len / val_bytes;
	unsigned int v;
	int ret, i;

	if (val_len % map->format.val_bytes)
		return -EINVAL;
	if (reg % map->reg_stride)
		return -EINVAL;

	map->lock(map);

	if (regmap_volatile_range(map, reg, val_count) || map->cache_bypass ||
	    map->cache_type == REGCACHE_NONE) {
		/* Physical block read if there's no cache involved */
		ret = _regmap_raw_read(map, reg, val, val_len);

	} else {
		/* Otherwise go word by word for the cache; should be low
		 * cost as we expect to hit the cache.
		 */
		for (i = 0; i < val_count; i++) {
			ret = _regmap_read(map, reg + (i * map->reg_stride),
					   &v);
			if (ret != 0)
				goto out;

			map->format.format_val(val + (i * val_bytes), v, 0);
		}
	}

 out:
	map->unlock(map);

	return ret;
}
EXPORT_SYMBOL_GPL(regmap_raw_read);

/**
 * regmap_bulk_read(): Read multiple registers from the device
 *
 * @map: Register map to write to
 * @reg: First register to be read from
 * @val: Pointer to store read value, in native register size for device
 * @val_count: Number of registers to read
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int regmap_bulk_read(struct regmap *map, unsigned int reg, void *val,
		     size_t val_count)
{
	int ret, i;
	size_t val_bytes = map->format.val_bytes;
	bool vol = regmap_volatile_range(map, reg, val_count);

	if (!map->format.parse_val)
		return -EINVAL;
	if (reg % map->reg_stride)
		return -EINVAL;

	if (vol || map->cache_type == REGCACHE_NONE) {
		/*
		 * Some devices does not support bulk read, for
		 * them we have a series of single read operations.
		 */
		if (map->use_single_rw) {
			for (i = 0; i < val_count; i++) {
				ret = regmap_raw_read(map,
						reg + (i * map->reg_stride),
						val + (i * val_bytes),
						val_bytes);
				if (ret != 0)
					return ret;
			}
		} else {
			ret = regmap_raw_read(map, reg, val,
					      val_bytes * val_count);
			if (ret != 0)
				return ret;
		}

		for (i = 0; i < val_count * val_bytes; i += val_bytes)
			map->format.parse_val(val + i);
	} else {
		for (i = 0; i < val_count; i++) {
			unsigned int ival;
			ret = regmap_read(map, reg + (i * map->reg_stride),
					  &ival);
			if (ret != 0)
				return ret;
			memcpy(val + (i * val_bytes), &ival, val_bytes);
		}
	}

	return 0;
}
EXPORT_SYMBOL_GPL(regmap_bulk_read);

static int _regmap_update_bits(struct regmap *map, unsigned int reg,
			       unsigned int mask, unsigned int val,
			       bool *change)
{
	int ret;
	unsigned int tmp, orig;

	ret = _regmap_read(map, reg, &orig);
	if (ret != 0)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;

	if (tmp != orig) {
		ret = _regmap_write(map, reg, tmp);
		*change = true;
	} else {
		*change = false;
	}

	return ret;
}

/**
 * regmap_update_bits: Perform a read/modify/write cycle on the register map
 *
 * @map: Register map to update
 * @reg: Register to update
 * @mask: Bitmask to change
 * @val: New value for bitmask
 *
 * Returns zero for success, a negative number on error.
 */
int regmap_update_bits(struct regmap *map, unsigned int reg,
		       unsigned int mask, unsigned int val)
{
	bool change;
	int ret;

	map->lock(map);
	ret = _regmap_update_bits(map, reg, mask, val, &change);
	map->unlock(map);

	return ret;
}
EXPORT_SYMBOL_GPL(regmap_update_bits);

/**
 * regmap_update_bits_check: Perform a read/modify/write cycle on the
 *                           register map and report if updated
 *
 * @map: Register map to update
 * @reg: Register to update
 * @mask: Bitmask to change
 * @val: New value for bitmask
 * @change: Boolean indicating if a write was done
 *
 * Returns zero for success, a negative number on error.
 */
int regmap_update_bits_check(struct regmap *map, unsigned int reg,
			     unsigned int mask, unsigned int val,
			     bool *change)
{
	int ret;

	map->lock(map);
	ret = _regmap_update_bits(map, reg, mask, val, change);
	map->unlock(map);
	return ret;
}
EXPORT_SYMBOL_GPL(regmap_update_bits_check);

/**
 * regmap_register_patch: Register and apply register updates to be applied
 *                        on device initialistion
 *
 * @map: Register map to apply updates to.
 * @regs: Values to update.
 * @num_regs: Number of entries in regs.
 *
 * Register a set of register updates to be applied to the device
 * whenever the device registers are synchronised with the cache and
 * apply them immediately.  Typically this is used to apply
 * corrections to be applied to the device defaults on startup, such
 * as the updates some vendors provide to undocumented registers.
 */
int regmap_register_patch(struct regmap *map, const struct reg_default *regs,
			  int num_regs)
{
	int i, ret;
	bool bypass;

	/* If needed the implementation can be extended to support this */
	if (map->patch)
		return -EBUSY;

	map->lock(map);

	bypass = map->cache_bypass;

	map->cache_bypass = true;

	/* Write out first; it's useful to apply even if we fail later. */
	for (i = 0; i < num_regs; i++) {
		ret = _regmap_write(map, regs[i].reg, regs[i].def);
		if (ret != 0) {
			dev_err(map->dev, "Failed to write %x = %x: %d\n",
				regs[i].reg, regs[i].def, ret);
			goto out;
		}
	}

	map->patch = kcalloc(num_regs, sizeof(struct reg_default), GFP_KERNEL);
	if (map->patch != NULL) {
		memcpy(map->patch, regs,
		       num_regs * sizeof(struct reg_default));
		map->patch_regs = num_regs;
	} else {
		ret = -ENOMEM;
	}

out:
	map->cache_bypass = bypass;

	map->unlock(map);

	return ret;
}
EXPORT_SYMBOL_GPL(regmap_register_patch);

/*
 * regmap_get_val_bytes(): Report the size of a register value
 *
 * Report the size of a register value, mainly intended to for use by
 * generic infrastructure built on top of regmap.
 */
int regmap_get_val_bytes(struct regmap *map)
{
	if (map->format.format_write)
		return -EINVAL;

	return map->format.val_bytes;
}
EXPORT_SYMBOL_GPL(regmap_get_val_bytes);

static int __init regmap_initcall(void)
{
	regmap_debugfs_initcall();

	return 0;
}
postcore_initcall(regmap_initcall);
