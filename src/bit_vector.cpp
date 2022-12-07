#include "bit_vector.hpp"
#include "bit_vector_builder.hpp"
#include "util/pm_utils.hpp"

namespace pisa {

bit_vector::bit_vector(bit_vector_builder* from, PM_TYPE pm_type)
: bit_vector::bit_vector(pm_type)
{
    std::cout<<"enter the right function"<<std::endl;
    m_size = from->size();
    (*m_bits).steal(from->move_bits());
}

}  // namespace pisa
