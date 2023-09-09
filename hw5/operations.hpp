#pragma once

#include "BDD.hpp"

using index_t = typename BDD::index_t;
using var_t = typename BDD::var_t;

/* Compute ~f */
inline index_t BDD::NOT( index_t f )
{
  assert( f < nodes.size() && "Make sure f exists." );

  /* trivial cases */
  if ( f == constant( false ) )
  {
    return constant( true );
  }
  if ( f == constant( true ) )
  {
    return constant( false );
  }

  Node const& F = nodes[f];
  var_t x = F.v;
  index_t f0 = F.E, f1 = F.T;

  index_t const r0 = NOT( f0 );
  index_t const r1 = NOT( f1 );
  return unique( x, r1, r0 );
}

/* Compute f ^ g */
inline index_t BDD::XOR( index_t f, index_t g )
{
  assert( f < nodes.size() && "Make sure f exists." );
  assert( g < nodes.size() && "Make sure g exists." );

  /* trivial cases */
  if ( f == g )
  {
    return constant( false );
  }
  if ( f == constant( false ) )
  {
    return g;
  }
  if ( g == constant( false ) )
  {
    return f;
  }
  if ( f == constant( true ) )
  {
    return NOT( g );
  }
  if ( g == constant( true ) )
  {
    return NOT( f );
  }
  if ( f == NOT( g ) )
  {
    return constant( true );
  }

  Node const& F = nodes[f];
  Node const& G = nodes[g];
  var_t x;
  index_t f0, f1, g0, g1;
  if ( F.v < G.v ) /* F is on top of G */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = g1 = g;
  }
  else if ( G.v < F.v ) /* G is on top of F */
  {
    x = G.v;
    f0 = f1 = f;
    g0 = G.E;
    g1 = G.T;
  }
  else /* F and G are at the same level */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = G.E;
    g1 = G.T;
  }

  index_t const r0 = XOR( f0, g0 );
  index_t const r1 = XOR( f1, g1 );
  return unique( x, r1, r0 );
}

/* Compute f & g */
inline index_t BDD::AND( index_t f, index_t g )
{
  assert( f < nodes.size() && "Make sure f exists." );
  assert( g < nodes.size() && "Make sure g exists." );
  
  /* trivial cases */
  if ( f == g )
  {
    return f;
  }
  if ( f == constant( false ) || g == constant( false ) )
  {
    return constant( false );
  }
  if ( f == constant( true ) )
  {
    return g;
  }
  if ( g == constant( true ) )
  {
    return f;
  }
  if ( f == NOT( g ) )
  {
    return constant( false );
  }

  Node const& F = nodes[f];
  Node const& G = nodes[g];
  var_t x;
  index_t f0, f1, g0, g1;
  if ( F.v < G.v ) /* F is on top of G */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = g1 = g;
  }
  else if ( G.v < F.v ) /* G is on top of F */
  {
    x = G.v;
    f0 = f1 = f;
    g0 = G.E;
    g1 = G.T;
  }
  else /* F and G are at the same level */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = G.E;
    g1 = G.T;
  }

  index_t const r0 = AND( f0, g0 );
  index_t const r1 = AND( f1, g1 );
  return unique( x, r1, r0 );
}

/* Compute ITE(f, g, h), i.e., f ? g : h */
inline index_t BDD::ITE( index_t f, index_t g, index_t h )
{
  assert( f < nodes.size() && "Make sure f exists." );
  assert( g < nodes.size() && "Make sure g exists." );
  assert( h < nodes.size() && "Make sure h exists." );

  /* trivial cases */
  if ( f == constant( true ) )
  {
    return g;
  }
  if ( f == constant( false ) )
  {
    return h;
  }

  Node const& F = nodes[f];
  Node const& G = nodes[g];
  Node const& H = nodes[h];
  index_t f0, f1, g0, g1, h0, h1;
  var_t x;

  if ( F.v < G.v && F.v < H.v ) /* F is on top of G and H */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = g1 = g;
    h0 = h1 = h;
  }
  else if ( G.v < F.v && G.v < H.v ) /* G is on top of F and H */
  {
    x = G.v;
    f0 = f1 = f;
    g0 = G.E;
    g1 = G.T;
    h0 = h1 = h;
  }
  else if ( H.v < F.v && H.v < G.v ) /* H is on top of F and G */
  {
    x = H.v;
    f0 = f1 = f;
    g0 = g1 = g;
    h0 = H.E;
    h1 = H.T;
  }
  else if ( F.v == G.v && F.v < H.v ) /* F and G are at the same level and above H */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = G.E;
    g1 = G.T;
    h0 = h1 = h;
  }
  else if ( F.v == H.v && F.v < G.v ) /* F and H are at the same level and above G */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = g1 = g;
    h0 = H.E;
    h1 = H.T;
  }
  else if ( G.v == H.v && G.v < F.v ) /* G and H are at the same level and above F */
  {
    x = G.v;
    f0 = f1 = f;
    g0 = G.E;
    g1 = G.T;
    h0 = H.E;
    h1 = H.T;
  }
  else /* F, G, and H are at the same level */
  {
    x = F.v;
    f0 = F.E;
    f1 = F.T;
    g0 = G.E;
    g1 = G.T;
    h0 = H.E;
    h1 = H.T;
  }

  const index_t r0 = ITE( f0, g0, h0 );
  const index_t r1 = ITE( f1, g1, h1 );

  return unique( x, r1, r0 );
}