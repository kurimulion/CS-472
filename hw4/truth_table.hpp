#pragma once

#include "utils.hpp"

#include <cassert>
#include <string>

class Truth_Table
{
public:
  Truth_Table( uint8_t num_var )
   : _num_vars( num_var ), _bits( 0u )
  {
    assert( num_var <= 6u );
  }

  Truth_Table( uint8_t num_var, uint64_t bits )
   : _num_vars( num_var ), _bits( bits & length_mask[_num_vars] )
  {
    assert( num_var <= 6u );
  }

  Truth_Table( const std::string str )
   : _num_vars( power_two( str.size() ) ), _bits( 0u )
  {
    if ( _num_vars == 0u )
    {
      return;
    }

    for ( auto i = 0u; i < str.size(); ++i )
    {
      if ( str[i] == '1' )
      {
        set_bit( str.size() - 1 - i );
      }
      else
      {
        assert( str[i] == '0' && "Error: Invalid truth table format." );
      }
    }
  }

  bool get_bit( uint8_t const position ) const
  {
    assert( position < ( 1 << _num_vars ) );
    return ( ( _bits >> position ) & 0x1 );
  }

  void set_bit( uint8_t const position )
  {
    assert( position < ( 1 << _num_vars ) );
    _bits |= ( uint64_t( 1 ) << position );
    _bits &= length_mask[_num_vars];
  }

  void set_bits( uint64_t const bits )
  {
    _bits = bits & length_mask[_num_vars];
  }

  uint8_t num_vars() const
  {
    return _num_vars;
  }

  Truth_Table positive_cofactor( uint8_t const var ) const;
  Truth_Table negative_cofactor( uint8_t const var ) const;
  Truth_Table derivative( uint8_t const var ) const;
  Truth_Table consensus( uint8_t const var ) const;
  Truth_Table smoothing( uint8_t const var ) const;

public:
  uint8_t const _num_vars; /* number of variables involved in the function */
  uint64_t _bits; /* the truth table */
};
