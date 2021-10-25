#ifndef __PERF_CPUMAP_H
#define __PERF_CPUMAP_H

#include <stdio.h>
#include <stdbool.h>

struct cpu_map {
	int nr;
	int map[];
};

struct cpu_map *cpu_map__new(const char *cpu_list);
struct cpu_map *cpu_map__dummy_new(void);
void cpu_map__delete(struct cpu_map *map);
struct cpu_map *cpu_map__read(FILE *file);
size_t cpu_map__fprintf(struct cpu_map *map, FILE *fp);

static inline int cpu_map__nr(const struct cpu_map *map)
{
	return map ? map->nr : 1;
}

static inline bool cpu_map__all(const struct cpu_map *map)
{
	return map ? map->map[0] == -1 : true;
}

#endif /* __PERF_CPUMAP_H */
