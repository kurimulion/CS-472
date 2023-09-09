#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

class Node
{
public:
  Node() {} /* An empty constructor */

  /* Store the information of an out-going edge from this node. */
  void add_successor( uint32_t node, int32_t weight )
  {
    /* Manually check if this edge already exists. */
    for ( auto& p : successors )
    {
      if ( p.first == node )
      {
        /* If so, update its weight. */
        p.second = weight;
        return;
      }
    }
    /* If not, insert a new edge. */
    successors.emplace_back( std::make_pair( node, weight ) );
  }

  /* Print all the out-edges of this node. Just to demonstrate how you can use the data structure. */
  void print_out_edges() const
  {
    for ( uint32_t i = 0u; i < successors.size(); ++i )
    {
      /* Use `successors.at( i ).first` to get the successor node.
         Use `successors.at( i ).second` to get the edge weight.
       */
      std::cout << "this -- " << successors.at( i ).second << " --> " << successors.at( i ).first << std::endl;
    }
    /* You can also use the following to loop through all successors:
       `for ( auto& p : successors )`
       where `p` is equivalent to `successors.at( i )` in the above code.
     */
  }

  /* For Dijkstra's */
  void visit( std::vector<int32_t>& dist, std::vector<bool>& visited, std::vector<uint32_t>& prev, uint32_t u ) const
  {
    visited.at( u ) = true;
    for ( auto const& p : successors )
    {
      uint32_t const& v = p.first;
      int32_t const& w = p.second;

      if ( visited.at( v ) )
      {
        continue;
      }

      int32_t new_dist = dist.at( u ) + w;
      if ( new_dist < dist.at( v ) )
      {
        dist.at( v ) = new_dist;
        prev.at( v ) = u;
      }
    }
  }

  void relax_node( std::vector<int32_t>& dist, std::vector<uint32_t>& prev, uint32_t u ) const
  {
    for ( auto const& p : successors )
    {
      uint32_t const& v = p.first;
      int32_t const& w = p.second;

      int32_t new_dist = dist.at( u ) + w;
      if ( new_dist < dist.at( v ) )
      {
        dist.at( v ) = new_dist;
        prev.at( v ) = u;
      }
    }
  }

  uint32_t successor_size() const {
    return successors.size();
  }

private:
  /* List of the successors of this node. Can also be interpreted as the list of out-going edges.
     The first element of the pair is the successor node index, and the second element is the edge weight.
   */
  std::vector<std::pair<uint32_t, int32_t>> successors;
};

class Graph
{
public:
  /* The constructor fills the vector `nodes` with `n` elements. */
  Graph( uint32_t n )
    : nodes( n )
  {}

  void add_edge( uint32_t from, uint32_t to, int32_t weight )
  {
    if ( from == to )
    {
      return;
    }
    nodes.at( from ).add_successor( to, weight );
    /* Use `nodes.at( from )` to access the `Node` object of index `from`.
       Note the difference between data types: We use `uint64_t` (a number) to
       represent nodes, but when you want to do something with the `Node` class
       defined above, you have to get the corresponding object with `nodes.at( index )`.
     */
  }

  std::vector<uint32_t> dijkstra_shortest_path( uint32_t source, uint32_t sink )
  {
    if ( source == sink )
    {
      return std::vector<uint32_t>( 1, source ); /* Returns a vector of a single element, which is `source`. */
    }

    /* Initialize with a large value. Shifting to prevent overflow. */
    std::vector<int32_t> dist( nodes.size(), std::numeric_limits<int32_t>::max() >> 2 );
    std::vector<bool> visited( nodes.size(), false );
    std::vector<uint32_t> prev( nodes.size(), std::numeric_limits<uint32_t>::max() );

    dist.at( source ) = 0;

    uint32_t u = source;
    while ( u != std::numeric_limits<uint32_t>::max() )
    {
      nodes.at( u ).visit( dist, visited, prev, u );

      /* Choose the next node to visit. If all nodes are visited, 
         `u` will remain `std::numeric_limits<uint32_t>::max()` and exit the loop. */
      u = std::numeric_limits<uint32_t>::max();
      int32_t min_dist = std::numeric_limits<int32_t>::max();
      for ( uint32_t i = 0u; i < nodes.size(); ++i )
      {
        if ( visited.at( i ) )
        {
          continue;
        }
        if ( dist.at( i ) < min_dist )
        {
          min_dist = dist.at( i );
          u = i;
        }
      }
    }

    /* Reconstruct the path. */
    if ( prev.at( sink ) == std::numeric_limits<uint32_t>::max() ) /* No path found. */
    {
      return std::vector<uint32_t>();
    }
    else
    {
      std::vector<uint32_t> path;
      uint32_t previous_node = sink;
      while ( previous_node != source )
      {
        path.emplace_back( previous_node );
        previous_node = prev.at( previous_node );
      }
      path.emplace_back( source );
      std::reverse( path.begin(), path.end() );
      return path;
    }
  }

  std::vector<uint32_t> bellman_ford_shortest_path( uint32_t source, uint32_t sink )
  {
    if ( source == sink )
    {
      return std::vector<uint32_t>( 1, source ); /* Returns a vector of a single element, which is `source`. */
    }

    /* TODO: Please implement Bellman-Ford Algorithm here. You are free to modify 
     * the data structure in this file, as long as you keep the interfaces 
     * (Graph::Graph, Graph::add_edge, Graph::bellman_ford_shortest_path). 
     */

    /* Initialize with a large value. Shifting to prevent overflow. */
    std::vector<int32_t> dist( nodes.size(), std::numeric_limits<int32_t>::max() >> 2 );
    std::vector<uint32_t> prev( nodes.size(), std::numeric_limits<uint32_t>::max() );

    dist.at( source ) = 0;

    /* Relax |V||E| times to calculate shortest path */
    for ( uint32_t i = 0; i < nodes.size(); i++) {
      /* Each iteration relax: |E| times */
      for ( uint32_t j = 0; j < nodes.size(); j++) {
        nodes.at( j ).relax_node( dist, prev, j );
      }
    }

    /* 
     * Check if there is any negative loop
     * After |V||E| relaxation distance should be not change
     * iff there is no loop
    */
    std::vector<int32_t> const prev_dist(dist);
    for ( int i = 0; i < nodes.size(); i++) {
      nodes.at( i ).relax_node( dist, prev, i );
    }

    if ( prev.at( sink ) == std::numeric_limits<uint32_t>::max() || prev_dist != dist ) /* No path found or negative loop detected. */
    {
      return std::vector<uint32_t>();
    }
    else
    {
      /* Back track to find the path from source to sink */
      std::vector<uint32_t> path;
      uint32_t previous_node = sink;
      while ( previous_node != source )
      {
        path.emplace_back( previous_node );
        previous_node = prev.at( previous_node );
      }
      path.emplace_back( source );
      std::reverse( path.begin(), path.end() );
      return path;
    }
  }

private:
  std::vector<Node> nodes;
};