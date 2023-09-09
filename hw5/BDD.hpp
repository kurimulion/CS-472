#pragma once

#include "truth_table.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>

/* These are just some hacks to hash std::pair (for the unique table).
 * You don't need to understand this part. */
namespace std
{
template<class T>
inline void hash_combine( size_t& seed, T const& v )
{
  seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<>
struct hash<pair<uint32_t, uint32_t>>
{
  using argument_type = pair<uint32_t, uint32_t>;
  using result_type = size_t;
  result_type operator() ( argument_type const& in ) const
  {
    result_type seed = 0;
    hash_combine( seed, in.first );
    hash_combine( seed, in.second );
    return seed;
  }
};
} // namespace std

class BDD
{
public:
  using index_t = uint32_t;
  /* Declaring `index_t` as an alias for an unsigned integer.
   * This is just for easier understanding of the code.
   * This datatype will be used for node indices. */

  using var_t = uint32_t;
  /* Similarly, declare `var_t` also as an alias for an unsigned integer.
   * This datatype will be used for representing variables. */

private:
  struct Node
  {
    var_t v; /* corresponding variable */
    index_t T; /* index of THEN child */
    index_t E; /* index of ELSE child */
  };

public:
  explicit BDD( uint32_t num_vars )
    : unique_table( num_vars )
  {
    nodes.emplace_back( Node({num_vars, 0, 0}) ); /* constant 0 */
    nodes.emplace_back( Node({num_vars, 1, 1}) ); /* constant 1 */
    /* `nodes` is initialized with two `Node`s representing the terminal (constant) nodes.
     * Their `v` is `num_vars` and their indices are 0 and 1.
     * (Note that the real variables range from 0 to `num_vars - 1`.)
     * Both of their children point to themselves, just for convenient representation.
     *
     * `unique_table` is initialized with `num_vars` empty maps. */
  }

#pragma region Basic Building Blocks
  uint32_t num_vars() const
  {
    return unique_table.size();
  }

  /* Get the (index of) constant node. */
  index_t constant( bool value ) const
  {
    return value ? 1 : 0;
  }

  /* Look up (if exist) or build (if not) the node with variable `var`,
   * THEN child `T`, and ELSE child `E`. */
  index_t unique( var_t var, index_t T, index_t E )
  {
    assert( var < num_vars() && "Variables range from 0 to `num_vars - 1`." );
    assert( T < nodes.size() && "Make sure the children exist." );
    assert( E < nodes.size() && "Make sure the children exist." );
    assert( nodes[T].v > var && "With static variable order, children can only be below the node." );
    assert( nodes[E].v > var && "With static variable order, children can only be below the node." );

    /* Reduction rule: Identical children */
    if ( T == E )
    {
      return T;
    }

    /* Look up in the unique table. */
    const auto it = unique_table[var].find( {T, E} );
    if ( it != unique_table[var].end() )
    {
      /* The required node already exists. Return it. */
      return it->second;
    }
    else
    {
      /* Create a new node and insert it to the unique table. */
      index_t const new_index = nodes.size();
      nodes.emplace_back( Node({var, T, E}) );
      unique_table[var][{T, E}] = new_index;
      return new_index;
    }
  }

  /* Return a node (represented with its index) of function F = x_var or F = ~x_var. */
  index_t literal( var_t var, bool complement = false )
  {
    return unique( var, constant( !complement ), constant( complement ) );
  }
#pragma endregion

#pragma region BDD Operations // implemented in `operations.hpp`
  /* Compute ~f */
  index_t NOT( index_t f );

  /* Compute f ^ g */
  index_t XOR( index_t f, index_t g );

  /* Compute f & g */
  index_t AND( index_t f, index_t g );

  /* Compute ITE(f, g, h), i.e., f ? g : h */
  index_t ITE( index_t f, index_t g, index_t h );
#pragma endregion

#pragma region Printing and Evaluating
  /* Print the BDD rooted at node `f`. */
  void print( index_t f, std::ostream& os = std::cout ) const
  {
    for ( auto i = 0u; i < nodes[f].v; ++i )
    {
      os << "  ";
    }
    if ( f <= 1 )
    {
      os << "node " << f << ": constant " << f << std::endl;
    }
    else
    {
      os << "node " << f << ": var = " << nodes[f].v << ", T = " << nodes[f].T 
         << ", E = " << nodes[f].E << std::endl;
      for ( auto i = 0u; i < nodes[f].v; ++i )
      {
        os << "  ";
      }
      os << "> THEN branch" << std::endl;
      print( nodes[f].T, os );
      for ( auto i = 0u; i < nodes[f].v; ++i )
      {
        os << "  ";
      }
      os << "> ELSE branch" << std::endl;
      print( nodes[f].E, os );
    }
  }

  /* Get the truth table of the BDD rooted at node f. */
  Truth_Table get_tt( index_t f ) const
  {
    assert( f < nodes.size() && "Make sure f exists." );
    assert( num_vars() <= 6 && "Truth_Table only supports functions of no greater than 6 variables." );

    if ( f == constant( false ) )
    {
      return Truth_Table( num_vars() );
    }
    else if ( f == constant( true ) )
    {
      return ~Truth_Table( num_vars() );
    }
    
    /* Shannon expansion: f = x f_x + x' f_x' */
    var_t const x = nodes[f].v;
    index_t const fx = nodes[f].T;
    index_t const fnx = nodes[f].E;
    Truth_Table const tt_x = create_tt_nth_var( num_vars(), x );
    Truth_Table const tt_nx = create_tt_nth_var( num_vars(), x, false );
    return ( tt_x & get_tt( fx ) ) | ( tt_nx & get_tt( fnx ) );
  }

  /* Get the number of nodes in the whole package (whether used or not), excluding constants. */
  uint64_t num_nodes() const
  {
    return nodes.size() - 2;
  }

  /* Get the number of nodes in the sub-graph rooted at node f, excluding constants. */
  uint64_t num_nodes( index_t f ) const
  {
    assert( f < nodes.size() && "Make sure f exists." );

    if ( f == constant( false ) || f == constant( true ) )
    {
      return 0u;
    }

    std::vector<bool> visited( nodes.size(), false );
    visited[0] = true;
    visited[1] = true;

    return num_nodes_rec( f, visited );
  }
#pragma endregion

private:
#pragma region Helper Functions
  uint64_t num_nodes_rec( index_t f, std::vector<bool>& visited ) const
  {
    assert( f < nodes.size() && "Make sure f exists." );
    

    uint64_t n = 0u;
    Node const& F = nodes[f];
    assert( F.T < nodes.size() && "Make sure the children exist." );
    assert( F.E < nodes.size() && "Make sure the children exist." );
    if ( !visited[F.T] )
    {
      n += num_nodes_rec( F.T, visited );
      visited[F.T] = true;
    }
    if ( !visited[F.E] )
    {
      n += num_nodes_rec( F.E, visited );
      visited[F.E] = true;
    }
    return n + 1u;
  }
#pragma endregion

private:
  std::vector<Node> nodes;
  std::vector<std::unordered_map<std::pair<index_t, index_t>, index_t>> unique_table;
  /* `unique_table` is a vector of `num_vars` maps storing the built nodes of each variable.
   * Each map maps from a pair of node indices (T, E) to a node index, if it exists.
   * See the implementation of `unique` for example usage. */
};
