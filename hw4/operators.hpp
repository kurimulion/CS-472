#pragma once

#include "truth_table.hpp"

#include <iostream>
#include <cassert>

/* overload std::ostream operator for convenient printing */
inline std::ostream& operator<<( std::ostream& os, Truth_Table const& tt )
{
  for ( int8_t i = ( 1 << tt.num_vars() ) - 1; i >= 0; --i )
  {
    os << ( tt.get_bit( i ) ? '1' : '0' );
  }
  return os;
}

/* bit-wise NOT operation */
inline Truth_Table operator~( Truth_Table const& tt )
{
  return Truth_Table( tt.num_vars(), ~tt._bits );
}

/* bit-wise OR operation */
inline Truth_Table operator|( Truth_Table const& tt1, Truth_Table const& tt2 )
{
  assert( tt1.num_vars() == tt2.num_vars() );
  return Truth_Table( tt1.num_vars(), tt1._bits | tt2._bits );
}

/* bit-wise AND operation */
inline Truth_Table operator&( Truth_Table const& tt1, Truth_Table const& tt2 )
{
  assert( tt1.num_vars() == tt2.num_vars() );
  return Truth_Table( tt1.num_vars(), tt1._bits & tt2._bits );
}

/* bit-wise XOR operation */
inline Truth_Table operator^( Truth_Table const& tt1, Truth_Table const& tt2 )
{
  assert( tt1.num_vars() == tt2.num_vars() );
  return Truth_Table( tt1.num_vars(), tt1._bits ^ tt2._bits );
}

/* check if two truth_tables are the same */
inline bool operator==( Truth_Table const& tt1, Truth_Table const& tt2 )
{
  if ( tt1.num_vars() != tt2.num_vars() )
  {
    return false;
  }
  return tt1._bits == tt2._bits;
}

inline bool operator!=( Truth_Table const& tt1, Truth_Table const& tt2 )
{
  return !( tt1 == tt2 );
}