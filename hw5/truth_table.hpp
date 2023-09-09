#pragma once

#include <iostream>
#include <cassert>
#include <string>

/* masks used to filter out unused bits */
static const uint64_t length_mask[] = {
  0x0000000000000001,
  0x0000000000000003,
  0x000000000000000f,
  0x00000000000000ff,
  0x000000000000ffff,
  0x00000000ffffffff,
  0xffffffffffffffff};

/* masks used to get the bits where a certain variable is 1 */
static const uint64_t var_mask_pos[] = {
  0xaaaaaaaaaaaaaaaa,
  0xcccccccccccccccc,
  0xf0f0f0f0f0f0f0f0,
  0xff00ff00ff00ff00,
  0xffff0000ffff0000,
  0xffffffff00000000};

/* masks used to get the bits where a certain variable is 0 */
static const uint64_t var_mask_neg[] = {
  0x5555555555555555,
  0x3333333333333333,
  0x0f0f0f0f0f0f0f0f,
  0x00ff00ff00ff00ff,
  0x0000ffff0000ffff,
  0x00000000ffffffff};

/* return i if n == 2^i and i <= 6, 0 otherwise */
inline uint8_t power_two( const uint32_t n )
{
  switch( n )
  {
    case 2u: return 1u;
    case 4u: return 2u;
    case 8u: return 3u;
    case 16u: return 4u;
    case 32u: return 5u;
    case 64u: return 6u;
    default: return 0u;
  }
}

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

inline Truth_Table Truth_Table::positive_cofactor( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  return Truth_Table( this->num_vars(), ( _bits & var_mask_pos[var] ) | ( ( _bits & var_mask_pos[var] ) >> ( 1 << var ) ) );
}

inline Truth_Table Truth_Table::negative_cofactor( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  return Truth_Table( this->num_vars(), ( _bits & var_mask_neg[var] ) | ( ( _bits & var_mask_neg[var] ) << ( 1 << var ) ) );
}

inline Truth_Table Truth_Table::derivative( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  return positive_cofactor( var ) ^ negative_cofactor( var );
}

inline Truth_Table Truth_Table::consensus( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  return positive_cofactor( var ) & negative_cofactor( var );
}

inline Truth_Table Truth_Table::smoothing( uint8_t const var ) const
{
  assert( var < this->num_vars() );
  return positive_cofactor( var ) | negative_cofactor( var );
}

/* Returns the truth table of f(x_0, ..., x_num_var) = x_var (or its complement). */
inline Truth_Table create_tt_nth_var( uint8_t const num_var, uint8_t const var, bool const polarity = true )
{
  assert ( num_var <= 6u && var < num_var );
  return Truth_Table( num_var, polarity ? var_mask_pos[var] : var_mask_neg[var] );
}