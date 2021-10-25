/*
 * builtin-diff.c
 *
 * Builtin diff command: Analyze two perf.data input files, look up and read
 * DSOs and symbol information, sort them and produce a diff.
 */
#include "builtin.h"

#include "util/debug.h"
#include "util/event.h"
#include "util/hist.h"
#include "util/evsel.h"
#include "util/evlist.h"
#include "util/session.h"
#include "util/tool.h"
#include "util/sort.h"
#include "util/symbol.h"
#include "util/util.h"

#include <stdlib.h>

static char const *input_old = "perf.data.old",
		  *input_new = "perf.data";
static char	  diff__default_sort_order[] = "dso,symbol";
static bool  force;
static bool show_displacement;

static int hists__add_entry(struct hists *self,
			    struct addr_location *al, u64 period)
{
	if (__hists__add_entry(self, al, NULL, period) != NULL)
		return 0;
	return -ENOMEM;
}

static int diff__process_sample_event(struct perf_tool *tool __maybe_unused,
				      union perf_event *event,
				      struct perf_sample *sample,
				      struct perf_evsel *evsel,
				      struct machine *machine)
{
	struct addr_location al;

	if (perf_event__preprocess_sample(event, machine, &al, sample, NULL) < 0) {
		pr_warning("problem processing %d event, skipping it.\n",
			   event->header.type);
		return -1;
	}

	if (al.filtered || al.sym == NULL)
		return 0;

	if (hists__add_entry(&evsel->hists, &al, sample->period)) {
		pr_warning("problem incrementing symbol period, skipping event\n");
		return -1;
	}

	evsel->hists.stats.total_period += sample->period;
	return 0;
}

static struct perf_tool tool = {
	.sample	= diff__process_sample_event,
	.mmap	= perf_event__process_mmap,
	.comm	= perf_event__process_comm,
	.exit	= perf_event__process_task,
	.fork	= perf_event__process_task,
	.lost	= perf_event__process_lost,
	.ordered_samples = true,
	.ordering_requires_timestamps = true,
};

static void insert_hist_entry_by_name(struct rb_root *root,
				      struct hist_entry *he)
{
	struct rb_node **p = &root->rb_node;
	struct rb_node *parent = NULL;
	struct hist_entry *iter;

	while (*p != NULL) {
		parent = *p;
		iter = rb_entry(parent, struct hist_entry, rb_node);
		if (hist_entry__cmp(he, iter) < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}

	rb_link_node(&he->rb_node, parent, p);
	rb_insert_color(&he->rb_node, root);
}

static void hists__name_resort(struct hists *self, bool sort)
{
	unsigned long position = 1;
	struct rb_root tmp = RB_ROOT;
	struct rb_node *next = rb_first(&self->entries);

	while (next != NULL) {
		struct hist_entry *n = rb_entry(next, struct hist_entry, rb_node);

		next = rb_next(&n->rb_node);
		n->position = position++;

		if (sort) {
			rb_erase(&n->rb_node, &self->entries);
			insert_hist_entry_by_name(&tmp, n);
		}
	}

	if (sort)
		self->entries = tmp;
}

static struct hist_entry *hists__find_entry(struct hists *self,
					    struct hist_entry *he)
{
	struct rb_node *n = self->entries.rb_node;

	while (n) {
		struct hist_entry *iter = rb_entry(n, struct hist_entry, rb_node);
		int64_t cmp = hist_entry__cmp(he, iter);

		if (cmp < 0)
			n = n->rb_left;
		else if (cmp > 0)
			n = n->rb_right;
		else
			return iter;
	}

	return NULL;
}

static void hists__match(struct hists *older, struct hists *newer)
{
	struct rb_node *nd;

	for (nd = rb_first(&newer->entries); nd; nd = rb_next(nd)) {
		struct hist_entry *pos = rb_entry(nd, struct hist_entry, rb_node);
		pos->pair = hists__find_entry(older, pos);
	}
}

static struct perf_evsel *evsel_match(struct perf_evsel *evsel,
				      struct perf_evlist *evlist)
{
	struct perf_evsel *e;

	list_for_each_entry(e, &evlist->entries, node)
		if (perf_evsel__match2(evsel, e))
			return e;

	return NULL;
}

static void perf_evlist__resort_hists(struct perf_evlist *evlist, bool name)
{
	struct perf_evsel *evsel;

	list_for_each_entry(evsel, &evlist->entries, node) {
		struct hists *hists = &evsel->hists;

		hists__output_resort(hists);

		/*
		 * The hists__name_resort only sets possition
		 * if name is false.
		 */
		if (name || ((!name) && show_displacement))
			hists__name_resort(hists, name);
	}
}

static int __cmd_diff(void)
{
	int ret, i;
#define older (session[0])
#define newer (session[1])
	struct perf_session *session[2];
	struct perf_evlist *evlist_new, *evlist_old;
	struct perf_evsel *evsel;
	bool first = true;

	older = perf_session__new(input_old, O_RDONLY, force, false,
				  &tool);
	newer = perf_session__new(input_new, O_RDONLY, force, false,
				  &tool);
	if (session[0] == NULL || session[1] == NULL)
		return -ENOMEM;

	for (i = 0; i < 2; ++i) {
		ret = perf_session__process_events(session[i], &tool);
		if (ret)
			goto out_delete;
	}

	evlist_old = older->evlist;
	evlist_new = newer->evlist;

	perf_evlist__resort_hists(evlist_old, true);
	perf_evlist__resort_hists(evlist_new, false);

	list_for_each_entry(evsel, &evlist_new->entries, node) {
		struct perf_evsel *evsel_old;

		evsel_old = evsel_match(evsel, evlist_old);
		if (!evsel_old)
			continue;

		fprintf(stdout, "%s# Event '%s'\n#\n", first ? "" : "\n",
			perf_evsel__name(evsel));

		first = false;

		hists__match(&evsel_old->hists, &evsel->hists);
		hists__fprintf(&evsel->hists, true, 0, 0, stdout);
	}

out_delete:
	for (i = 0; i < 2; ++i)
		perf_session__delete(session[i]);
	return ret;
#undef older
#undef newer
}

static const char * const diff_usage[] = {
	"perf diff [<options>] [old_file] [new_file]",
	NULL,
};

static const struct option options[] = {
	OPT_INCR('v', "verbose", &verbose,
		    "be more verbose (show symbol address, etc)"),
	OPT_BOOLEAN('M', "displacement", &show_displacement,
		    "Show position displacement relative to baseline"),
	OPT_BOOLEAN('D', "dump-raw-trace", &dump_trace,
		    "dump raw trace in ASCII"),
	OPT_BOOLEAN('f', "force", &force, "don't complain, do it"),
	OPT_BOOLEAN('m', "modules", &symbol_conf.use_modules,
		    "load module symbols - WARNING: use only with -k and LIVE kernel"),
	OPT_STRING('d', "dsos", &symbol_conf.dso_list_str, "dso[,dso...]",
		   "only consider symbols in these dsos"),
	OPT_STRING('C', "comms", &symbol_conf.comm_list_str, "comm[,comm...]",
		   "only consider symbols in these comms"),
	OPT_STRING('S', "symbols", &symbol_conf.sym_list_str, "symbol[,symbol...]",
		   "only consider these symbols"),
	OPT_STRING('s', "sort", &sort_order, "key[,key2...]",
		   "sort by key(s): pid, comm, dso, symbol, parent"),
	OPT_STRING('t', "field-separator", &symbol_conf.field_sep, "separator",
		   "separator for columns, no spaces will be added between "
		   "columns '.' is reserved."),
	OPT_STRING(0, "symfs", &symbol_conf.symfs, "directory",
		    "Look for files with symbols relative to this directory"),
	OPT_END()
};

static void ui_init(void)
{
	perf_hpp__init();

	/* No overhead column. */
	perf_hpp__column_enable(PERF_HPP__OVERHEAD, false);

	/* Display baseline/delta/displacement columns. */
	perf_hpp__column_enable(PERF_HPP__BASELINE, true);
	perf_hpp__column_enable(PERF_HPP__DELTA, true);

	if (show_displacement)
		perf_hpp__column_enable(PERF_HPP__DISPL, true);
}

int cmd_diff(int argc, const char **argv, const char *prefix __maybe_unused)
{
	sort_order = diff__default_sort_order;
	argc = parse_options(argc, argv, options, diff_usage, 0);
	if (argc) {
		if (argc > 2)
			usage_with_options(diff_usage, options);
		if (argc == 2) {
			input_old = argv[0];
			input_new = argv[1];
		} else
			input_new = argv[0];
	} else if (symbol_conf.default_guest_vmlinux_name ||
		   symbol_conf.default_guest_kallsyms) {
		input_old = "perf.data.host";
		input_new = "perf.data.guest";
	}

	symbol_conf.exclude_other = false;
	if (symbol__init() < 0)
		return -1;

	ui_init();

	setup_sorting(diff_usage, options);
	setup_pager();

	sort_entry__setup_elide(&sort_dso, symbol_conf.dso_list, "dso", NULL);
	sort_entry__setup_elide(&sort_comm, symbol_conf.comm_list, "comm", NULL);
	sort_entry__setup_elide(&sort_sym, symbol_conf.sym_list, "symbol", NULL);

	return __cmd_diff();
}
