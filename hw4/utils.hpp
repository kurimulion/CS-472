#pragma once

#include <cstdint>

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