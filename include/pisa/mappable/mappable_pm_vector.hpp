#pragma once

#include <algorithm>
#include <vector>
#include <memkind.h>
#include <memkind_allocator.h>

#include "boost/function.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/range.hpp"
#include "boost/utility.hpp"

#include "util/intrinsics.hpp"

#define PMEM_MAX_SIZE (1024 * 1024 * 64)

namespace pisa { namespace mapper{

    namespace detail {
        class freeze_visitor;
        class map_visitor;
        class sizeof_visitor;
    } // namespace detail

    using deleter_t = boost::function<void()>;
    static char path[PATH_MAX] = "/tmp/";
    static libmemkind::kinds kind = libmemkind::kinds::DEFAULT;

    /**
     * mappable_pm_vector stores vector in persistent memory
     * but it doesn't persistent vector. It just use PM as DRAM
    */
    template <typename T> // T must be a POD
    class mappable_pm_vector {
        public:
        using value_type = T;
        using iterator = const T*;
        using const_iterator = const T*;
        using allocator = libmemkind::static_kind::allocator<T>;


        mappable_vector() : m_data(0), m_size(0), m_deleter() {}
        mappable_vector(mappable_vector const&) = delete;
        mappable_vector(mappable_vector&&) = delete;
        mappable_vector& operator=(mappable_vector const&) = delete;
        mappable_vector& operator=(mappable_vector&&) = delete;

        template <typename Range>
        explicit mappable_vector(Range const& from)
        : m_data(0), m_size(0) {
            size_t size = boost::size(from);
            allocator alc{kind};
            T* data = alc.allocate(size);
            m_deleter = boost::lambda::bind(boost::lambda::delete_array(), data);

            std::copy(boost::begin(from), boost::end(from), data);
            m_data = data;
            m_size = size;
        }

        ~mappable_pm_vector() {
            if (not m_deleter.empty()) {
                m_deleter();
            }
        }

        void swap(mappable_pm_vector& other) {
            using std::swap;
            swap(m_data, other.m_data);
            swap(m_size, other.m_size);
            swap(m_deleter, other.m_deleter);
        }

        void clear() { mappable_pm_vector().swap(*this); }

        void steal(std::vector<T>& vec) {
            clear();
            m_size = vec.size();
            if (m_size > 0) {
                auto* new_vec = new std::vector<T,allocator>(alc);
                new_vec->swap(vec);
                m_deleter = boost::lambda::bind(boost::lambda::delete_ptr(), new_vec);
                m_data = &(*new_vec)[0];
            }
        }

        template <typename Range>
        void assign(Range const& from) {
            clear();
            mappable_pm_vector(from).swap(*this);
        }

        uint64_t size() const { return m_size; }

        inline const_iterator begin() const { return m_data; }

        inline const_iterator end() const { return m_data + m_size; }

        inline T const& operator[](uint64_t i) const {
            assert(i < m_size);
            return m_data[i];
        }

        inline T const* data() const { return m_data; }

        inline void prefetch(size_t i) const { intrinsics::prefetch(m_data + i); }

        friend class detail::freeze_visitor;
        friend class detail::map_visitor;
        friend class detail::sizeof_visitor;
        
        protected:
        const T* m_data;
        uint64_t m_size;
        deleter_t m_deleter;
    };





}}