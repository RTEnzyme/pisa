#pragma once

#include <algorithm>
#include <vector>
#include <atomic>

#include <libpmemobj++/container/segment_vector.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>

#include "boost/function.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/range.hpp"
#include "boost/utility.hpp"

#include "util/intrinsics.hpp"
#include "util/pm_utils.hpp"
#include "mappable/mappable_vector.hpp"


namespace pisa { namespace mapper {
    
    namespace detail {
        class freeze_visitor;
        class map_visitor;
        class sizeof_visitor;
    } // namespace detail

    static bool file_exists(std::string& path) {
        std::ifstream f(path.c_str());
        return f.good();
    }

    using deleter_t = boost::function<void()>;
    
    template <typename T>
    class mappable_pm_vector : public mappable_vector<T>{
    public:
        using value_type = T;
        using p_type = pmem::obj::p<T>;
        using iterator = const T*;
        using const_iterator = const T*;
        using seg_vector = pmem::obj::segment_vector<p_type, pmem::obj::exponential_size_array_policy<>>;
        
        

        explicit mappable_pm_vector(std::string layout_name) 
        : mappable_pm_vector()
        {
            std::cout<<"enter the right construction"<<std::endl;
            try {
                auto suffix = ++m_suffix;
                std::string file_path = std::string(PMEM_DIR) + "/" + layout_name + "_" + std::to_string(suffix);
                std::cout<<file_path<<std::endl;
                if (file_exists(file_path)) {
                    m_pop = pmem::obj::pool<root>::open(file_path, layout_name);
                } else {
                    m_pop = pmem::obj::pool<root>::create(file_path, layout_name, PMEM_MAX_SIZE);
                }
                r = m_pop.root();
                if (r->m_data != nullptr) {
                    // if there has the old data, delete them before re-init
                    pmem::obj::transaction::run(m_pop, [&] {
                        pmem::obj::delete_persistent<seg_vector>(r->m_data);
                        pmem::obj::delete_persistent<uint64_t>(r->m_size);
                    });
                }
                pmem::obj::transaction::run(m_pop, [&] {
                    r->m_data = pmem::obj::make_persistent<seg_vector>();
                    r->m_size = pmem::obj::make_persistent<uint64_t>(0);
                });
            } catch (const std::exception &e) {
                std::cerr << "can't init mappable_pm_vector"<< e.what() << std::endl;
                exit(1);
            }
        }

        mappable_pm_vector() = default;

        mappable_pm_vector(mappable_pm_vector const&) = delete;
        mappable_pm_vector(mappable_pm_vector&&) = delete;
        mappable_pm_vector& operator=(mappable_pm_vector const&) = delete;
        mappable_pm_vector& operator=(mappable_pm_vector&&) = delete;

        /**
         * Load pm_vector data from other volatile data structure
         * 
        */
        template <typename Range>
        explicit mappable_pm_vector(Range const& from) : mappable_pm_vector() {

            (*r->m_data).assign(boost::begin(from), boost::end(from));
            *r->m_size = boost::size(from);
        }

        ~mappable_pm_vector() {
            if (r->m_data != nullptr) {
                pmem::obj::transaction::run(m_pop, [&] {
                    pmem::obj::delete_persistent<seg_vector>(r->m_data);
                    pmem::obj::delete_persistent<uint64_t>(r->m_size);
                });
            }
        }

        // void swap(mappable_vector& other) {
        //     std::vector<value_type> tmp_vec(*r->m_data.cbegin(), *r->m_data.cend());
        //     std::copy(*other.m_data, *other.m_data+sizeof(value_type)*other.m_size, r->m_data);
        //     other.steal(tmp_vec);
        //     std::swap(*r->m_size, other.m_size);
        // }

        void swap(mappable_pm_vector& other) {
            using std::swap;
            swap(m_pop, other.m_pop);
            swap(r, other.r);
        }

        void clear() {
            (*r->m_data).clear();
        }

        void steal(std::vector<value_type>& vec) {
            std::cout<<"start stealing"<<std::endl;
            clear();
            *r->m_size = vec.size();
            for (value_type ele: vec) {
                (*r->m_data).push_back(ele);
            }
        }

        template <typename Range>
        void assign(Range const& from) {
            clear();
            mappable_pm_vector(from).swap(*this);
        }

        uint64_t size() const {return *r->m_size;}

        inline const_iterator begin() const {return r->m_data.begin();}

        inline const_iterator end() const {return r->m_data.end();}

        inline T const& operator[](uint64_t i) const {
            assert(i < r->m_size);
            return (*r->m_data)[i].get_rw();
        }

        inline T const* data() const {return &(*r->m_data)[0].get_rw();}
        

        inline void prefetch(size_t ) const {}

        friend class detail::freeze_visitor;
        friend class detail::map_visitor;
        friend class detail::sizeof_visitor;
    
    protected:
        struct root {
            pmem::obj::persistent_ptr<seg_vector> m_data;
            pmem::obj::persistent_ptr<uint64_t> m_size;
        };
        pmem::obj::pool<root> m_pop;
        pmem::obj::persistent_ptr<root> r;
    private:
        static std::atomic_uint m_suffix;
    };

    template<typename T> std::atomic_uint mappable_pm_vector<T>::m_suffix = 0;
}}

