/*!
  \file cut_sweeping.hpp
  \brief Cut sweeping algorithm

  CS-472 2022 Fall final project
*/

#pragma once

#include "cut_enumeration.hpp"

namespace mockturtle
{

/* Parameters for the cut sweeping algorithm 
   You may add more parameters to experiment with,
   but please keep `cut_size` and `max_num_cuts`.
   Note that 6 and 10 are the default values,
   but they may be set differently in the tests. */
struct cut_sweeping_params
{
  /* Cut size (maximum number of leaves); k in the paper */
  uint32_t cut_size{6};

  /* Maximum number of cuts considered for each node; N in the paper */
  uint32_t max_num_cuts{10};
};

struct data
{
  std::size_t hash{0};
};

template<class Ntk>
void cut_sweeping( Ntk& ntk, cut_sweeping_params const& ps = {} )
{
  // Implement your cut sweeping algorithm here
  // You may assume that `Ntk` is always an `aig_network`
  // Please do not change the interface of this function
  cut_enumeration_params cut_ps;
  cut_ps.cut_size = ps.cut_size;
  cut_ps.cut_limit = ps.max_num_cuts;
  custom_cut_enumeration<Ntk, true, data>( ntk, cut_ps );
}

} // namespace mockturtle
