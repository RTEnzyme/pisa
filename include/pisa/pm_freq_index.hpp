#pragma once

#include<exception>


#include<fmt/fmt.h>
#include "memkind.h"

#include "memkind_allocated.hpp"
#include "bitvector_collection.hpp"
#include "codec/compact_elias_fano.hpp"
#include "codec/integer_codes.hpp"
#include "global_parameters.hpp"
#include "mappable/mapper.hpp"
#include "memory_source.hpp"

namespace pisa {

/**
 * Used as a tag for PM index layout
*/
struct PMIndexTag;

/**
 * PM frequency inverted index.
 * 
 * Each posting list is part of two bit vectors: one for document IDS,
 * and the other for frequencies (or scores)
 * 
 * \tparam PMCodec  Block-wise codec type.
 * \tparam Profile  If true, enables performance profiling.
*/
template <typename DocsSequence, typename FreqsSequence>
class alignas(128) pm_freq_index 
    : public memkind_allocated<pm_freq_index>{
public:
    using index_layout_tag = PMIndexTag;

    static memkind_t getClassKind() {
        return MEMKIND_DEFAULT;
    }

    pm_freq_index() = default;

    /**
     * Constructs a view over the index encoded in the given memory source.
     * 
     * 
    */
   explicit pm_freq_index(MemorySource source) : m_source(std::move(source))
   {
        mapper::map(*this, m_source.data(), mapper::map_flags::warmup);
   }

   /**
    * Constructs a PM view over the index stored in the file
    * 
   */
  explicit pm_freq_index(const std::string& index_filename) {

  }

private:
    global_parameters m_params;
    uint64_t n_num_docs = 0;
    bitvector_collection m_docs_sequences;
    bitvector_collection m_freqs_sequences;
    MemorySource m_source;
};

}