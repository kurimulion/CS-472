#pragma once

#include "utils.hpp"
#include "truth_table.hpp"
#include "operators.hpp"

#include <cassert>

inline Truth_Table Truth_Table::positive_cofactor( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  Truth_Table result( this->num_vars() );
  u_int64_t f_var = this->_bits & var_mask_pos[var];
  f_var = f_var | ( f_var >> ( 1 << var ) );
  result.set_bits( f_var );
  return result;
}

inline Truth_Table Truth_Table::negative_cofactor( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  Truth_Table result( this->num_vars() );
  u_int64_t f_var_bar = this->_bits & var_mask_neg[var];
  f_var_bar = f_var_bar | ( f_var_bar << ( 1 << var ) );
  result.set_bits( f_var_bar );
  return result;
}

inline Truth_Table Truth_Table::derivative( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  Truth_Table f_var = this->positive_cofactor( var );
  Truth_Table f_var_bar = this->negative_cofactor( var );
  Truth_Table result = f_var ^ f_var_bar;
  return result;
}

inline Truth_Table Truth_Table::consensus( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  Truth_Table f_var = this->positive_cofactor( var );
  Truth_Table f_var_bar = this->negative_cofactor( var );
  Truth_Table result = f_var & f_var_bar;
  return result;
}

inline Truth_Table Truth_Table::smoothing( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  Truth_Table f_var = this->positive_cofactor( var );
  Truth_Table f_var_bar = this->negative_cofactor( var );
  Truth_Table result = f_var | f_var_bar;
  return result;
}