#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <cstdlib>
#include <vector>
#include <rapidcheck.h>
#include "test_common.hpp"
#include "test_rank_select_common.hpp"

#include <libpmemobj++/container/segment_vector.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>

#include "bit_vector_builder.hpp"
#include "mappable/mappable_pm_vector.hpp"
#include "util/pm_utils.hpp"
#include "bit_vector.hpp"

using namespace pmem::obj;
using seg_vector = pmem::obj::segment_vector<p<uint64_t>, pmem::obj::exponential_size_array_policy<>>;
struct root {
    pmem::obj::persistent_ptr<seg_vector> m_data;
    pmem::obj::p<uint64_t> m_size;
};
TEST_CASE("init") {
    rc::check([]() {
        pisa::mapper::mappable_pm_vector<uint64_t> vec(std::string("bit_vector_test"));
        // pool<root> m_pop = pmem::obj::pool<root>::open("/mnt/pmemdir/bit_vector_test", "bit_vector_test");
        // pool<root> m_pop = pool<root>::create("/mnt/pmemdir/bit_vector_test_0", "bit_vector_test", PMEM_MAX_SIZE);
        std::cout<<"success"<<std::endl;
    });
}

TEST_CASE("bit_vector") {
    rc::check([](std::vector<bool> v) 
    {
        // std::vector<uint64_t> t;
        // t.push_back(1);
        // t.push_back(2);
        // t.push_back(3);
        // t.push_back(4);
        // pisa::mapper::mappable_pm_vector<uint64_t> vec(std::string("bit_vector_test"));
        // auto t_copy = std::vector(t);
        // vec.steal(t_copy);
        // for (int i = 0; i < 4; i++) {
        //     MY_REQUIRE_EQUAL(t[i], vec[i], "i= " << i);
        // }

        // {
        //     pisa::bit_vector_builder bvb(v.size());
        //     for (size_t i = 0; i < v.size(); ++i) {
        //         bvb.set(i, v[i]);
        //     }
        //     bvb.push_back(false);
        //     v.push_back(false);
        //     bvb.push_back(true);
        //     v.push_back(true);

        //     pisa::pm_bit_vector bitmap(&bvb);
        //     test_equal_bits(v, bitmap, "Random bits (set)");
        // }

        auto ints = std::array<uint64_t, 15>{uint64_t(-1),
                                             uint64_t(1) << 63u,
                                             1,
                                             1,
                                             1,
                                             3,
                                             5,
                                             7,
                                             0xFFF,
                                             0xF0F,
                                             1,
                                             0xFFFFFF,
                                             0x123456,
                                             uint64_t(1) << 63u,
                                             uint64_t(-1)};
        // {
        //     pisa::bit_vector_builder bvb;
        //     for (uint64_t i: ints) {
        //         uint64_t len = pisa::broadword::msb(i) + 1;
        //         bvb.append_bits(i, len);
        //     }
            
        //     pisa::pm_bit_vector bitmap(&bvb);
        //     std::cout<<"init success"<<std::endl;
        //     uint64_t pos = 0;
        //     for (uint64_t i: ints) {
        //         uint64_t len = pisa::broadword::msb(i) + 1;
        //         std::cout<<len<<std::endl;
        //         REQUIRE(i == bitmap.get_bits(pos, len));
        //         pos += len;
        //     }

        // }

        { 
            using pisa::broadword::msb;
            std::vector<size_t> positions(1);
            for (uint64_t i: ints) {
                positions.push_back(positions.back() + msb(i) + 1);
            }
            pisa::bit_vector_builder bvb(positions.back());

            for (size_t i = 0; i < positions.size() - 1; ++i) {
                uint64_t v = ints[i];
                uint64_t len = positions[i + 1] - positions[i];
                bvb.set_bits(positions[i], v, len);
            }

            pisa::bit_vector bitmap(&bvb, NO_PM);
            for (size_t i = 0; i < positions.size() - 1; ++i) {
                uint64_t v = ints[i];
                uint64_t len = positions[i + 1] - positions[i];
                std::cout<<i<<std::endl;
                REQUIRE(v == bitmap.get_bits(positions[i], len));
            }
        }
});
}