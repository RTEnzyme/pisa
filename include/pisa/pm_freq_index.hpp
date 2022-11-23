#pragma once

#include<exception>


#include<fmt/fmt.h>

#include "bit_vector.hpp"
#include "block_posting_list.hpp"
#include "codec/compact_elias_fano.hpp"
#include "mappable/mappable_vector.hpp"
#include "mappable/mapper.hpp"
#include "memory_source.hpp"
#include "temporary_directory.hpp"

namespace pisa {

/**
 * Used as a tag for PM index layout
*/
struct PMIndexTag;

/**
 * PM frequency inverted index.
 * 
 * Each posting list is serialized into PM blocks.
 * One block contains both document IDs and frequencies 
 * 
 * \tparam PMCodec  Block-wise codec type.
 * \tparam Profile  If true, enables performance profiling.
*/
template <typename PMCodec, bool Profile = false>
class pm_freq_index {
public:
    using index_layout_tag = PMIndexTag;

    pm_freq_index() = default;

    /**
     * Constructs a view over the index encoded in the given memory source.
     * 
     * 
    */
}

};