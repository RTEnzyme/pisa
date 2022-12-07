#pragma once

#include <libpmemobj++/container/segment_vector.hpp>
#include <fstream>

#define PMEM_DIR "/mnt/pmemdir"
#define PMEM_MAX_SIZE (1024 * 1024 * 16)

enum PM_TYPE {
	NO_PM,
	PM_AS_EXTENSION,
	PM_AS_DRAM,
	HYBRID_PM_DRAM
};
