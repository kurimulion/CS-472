#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

class Checker_Node
{
public:
  Checker_Node() {}

  void add_successor( uint32_t node, int32_t weight )
  {
    for ( auto& p : successors )
    {
      if ( p.first == node )
      {
        p.second = weight;
        return;
      }
    }
    successors.emplace_back( std::make_pair( node, weight ) );
  }

  int32_t successor_distance( uint32_t node ) const
  {
    for ( auto const& p : successors )
    {
      if ( p.first == node )
      {
        return p.second;
      }
    }
    return std::numeric_limits<int32_t>::max();
  }

private:
  std::vector<std::pair<uint32_t, int32_t>> successors;
};

class Checker_Graph
{
public:
  Checker_Graph( uint32_t n )
    : nodes( n )
  {}

  void add_edge( uint32_t from, uint32_t to, int32_t weight )
  {
    if ( from == to )
    {
      return;
    }
    nodes.at( from ).add_successor( to, weight );
  }

  int32_t check_path( std::vector<uint32_t> const& path, uint32_t source, uint32_t sink ) const
  {
    if ( path.size() == 0u )
    {
      return std::numeric_limits<int32_t>::max(); /* no path or has negative loop */
    }
    if ( path.at( 0u ) != source || path.back() != sink )
    {
      return std::numeric_limits<int32_t>::min(); /* wrong path */
    }
    if ( path.size() == 1u && source == sink )
    {
      return 0; /* length is 0 because source == sink */
    }

    int32_t length = 0;
    auto p1 = path.begin();
    while ( p1+1 != path.end() )
    {
      int32_t d = nodes.at( *p1 ).successor_distance( *(p1+1) );
      if ( d == std::numeric_limits<int32_t>::max() )
      {
        return std::numeric_limits<int32_t>::min(); /* wrong path */
      }
      length += d;
      p1++;
    }

    return length;
  }

private:
  std::vector<Checker_Node> nodes;
};