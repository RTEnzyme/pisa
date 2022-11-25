#include "bit_vector.hpp"
#include "bit_vector_builder.hpp"

namespace pisa {

bit_vector::bit_vector(bit_vector_builder* from)
{
    std::cout<<"init from bit_vector.cpp"<<std::endl;
    m_size = from->size();
    m_bits.set_pm(true);
    m_bits.steal(from->move_bits());
}

}  // namespace pisa
