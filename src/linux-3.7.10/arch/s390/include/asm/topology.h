#ifndef _ASM_S390_TOPOLOGY_H
#define _ASM_S390_TOPOLOGY_H

#include <linux/cpumask.h>

struct sysinfo_15_1_x;
struct cpu;

#ifdef CONFIG_SCHED_BOOK

extern unsigned char cpu_socket_id[NR_CPUS];
#define topology_physical_package_id(cpu) (cpu_socket_id[cpu])

extern unsigned char cpu_core_id[NR_CPUS];
extern cpumask_t cpu_core_map[NR_CPUS];

static inline const struct cpumask *cpu_coregroup_mask(int cpu)
{
	return &cpu_core_map[cpu];
}

#define topology_core_id(cpu)		(cpu_core_id[cpu])
#define topology_core_cpumask(cpu)	(&cpu_core_map[cpu])
#define mc_capable()			(1)

extern unsigned char cpu_book_id[NR_CPUS];
extern cpumask_t cpu_book_map[NR_CPUS];

static inline const struct cpumask *cpu_book_mask(int cpu)
{
	return &cpu_book_map[cpu];
}

#define topology_book_id(cpu)		(cpu_book_id[cpu])
#define topology_book_cpumask(cpu)	(&cpu_book_map[cpu])

int topology_cpu_init(struct cpu *);
int topology_set_cpu_management(int fc);
void topology_schedule_update(void);
void store_topology(struct sysinfo_15_1_x *info);
void topology_expect_change(void);

#else /* CONFIG_SCHED_BOOK */

static inline void topology_schedule_update(void) { }
static inline int topology_cpu_init(struct cpu *cpu) { return 0; }
static inline void topology_expect_change(void) { }

#endif /* CONFIG_SCHED_BOOK */

#define POLARIZATION_UNKNOWN	(-1)
#define POLARIZATION_HRZ	(0)
#define POLARIZATION_VL		(1)
#define POLARIZATION_VM		(2)
#define POLARIZATION_VH		(3)

#ifdef CONFIG_SCHED_BOOK
void s390_init_cpu_topology(void);
#else
static inline void s390_init_cpu_topology(void)
{
};
#endif

#define SD_BOOK_INIT	SD_CPU_INIT

#include <asm-generic/topology.h>

#endif /* _ASM_S390_TOPOLOGY_H */
