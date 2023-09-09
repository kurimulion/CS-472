#pragma once

#include "problem.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cassert>

class ILP
{
  using Resource = Problem::Resource;
  using Operation = Problem::Operation;

  enum Constraint_Type { GE = 1, LE, EQ };
                      /* >=      <=  == */

  struct Variable
  {
    uint32_t i; /* operation id */
    uint32_t l; /* timeframe id */
  };

  struct Constraint
  {
    /* Assume `variables` contains v[0], v[1], ...
     * `coefficients` contains c[0], c[1], ...
     * The object encodes a linear inequality of 
     * c[0] x_v[0] + c[1] x_v[1] + ... [>= | <= | ==] constant
     * where the relation type is defined by `type`
     */
    std::vector<Variable> variables;
    std::vector<int64_t> coefficients;
    Constraint_Type type;
    uint64_t constant; /* the right-hand side constant */
  };

public:
  ILP( Problem const& p )
    : prob( p ), num_operations( p.operations.size() - 1u ), num_timeframes( p.latency_bound + 1u )
  {
    /* construct the three categories of constraints */
    operations_starts_once();
    sequencing_relations();
    resource_bounds();
  }

  void print_lp( std::ostream& os = std::cout ) const
  {
    /* the objective function */
    os << "min:";
    for ( auto l = 1u; l <= num_timeframes; ++l )
    {
      os << " +" << l << " x" << num_operations << "_" << l;
    }
    os << " -1;" << std::endl;

    /* the constraints */
    for ( auto const& con : constraints )
    {
      assert( con.variables.size() == con.coefficients.size() );
      for ( auto v = 0u; v < con.variables.size(); ++v )
      {
        auto& var = con.variables.at( v );
        auto& cof = con.coefficients.at( v );
        assert( 1 <= var.i && var.i <= num_operations );
        assert( 1 <= var.l && var.l <= num_timeframes );
        if ( cof == 0 ) { continue; }
        if ( cof > 0 ) { os << "+"; }
        if ( cof > 1 || cof < -1 ) { os << cof; }
        else if ( cof == -1 ) { os << "-"; }
        os << " x" << var.i << "_" << var.l << " ";
      }
      switch ( con.type )
      {
        case GE: { os << ">="; break; }
        case LE: { os << "<="; break; }
        case EQ: { os << "="; break; }
        default: assert( false );
      }
      os << " " << con.constant << ";" << std::endl;
    }

    /* variable type declaration */
    os << "binary ";
    for ( auto i = 1u; i <= num_operations; ++i )
    {
      for ( auto l = 1u; l <= num_timeframes; ++l )
      {
        os << "x" << i << "_" << l;
        if ( i != num_operations || l != num_timeframes )
        {
          os << ", ";
        }
      }
    }
    os << ";" << std::endl;
  }

  void dump_lp( std::string const& filename = "scheduling.lp" ) const
  {
    std::ofstream fout( filename, std::ofstream::out );
    print_lp( fout );
  }

private:
  void operations_starts_once()
  {
    /* Encoding of the first category of constraints is implemented for you as an example. */
    for ( auto i = 1u; i <= num_operations; ++i )
    {
      Constraint constraint;
      constraint.type = EQ;
      constraint.constant = 1;
      for ( auto l = 1u; l <= num_timeframes; ++l )
      {
        constraint.variables.emplace_back( Variable( {i, l} ) );
        constraint.coefficients.emplace_back( 1 );
      }
      constraints.emplace_back( constraint );
    }
  }

  void sequencing_relations()
  {
    /* Sequencing relationship is encoded as \sum(l * x_{i, l}) - \sum(l * x{j, l}) >= delay */
    for ( auto i = 1u; i <= num_operations; ++i )
    {
      Operation const& op = prob.operations.at(i);

      /* If the predecessor is NOP then skip */
      if ( op.predecessors.size() == 1 && op.predecessors.at(0) == 0 )
      {
        continue;
      }
      for ( auto const& pre: op.predecessors )
      {
        char pre_type = prob.operations.at( pre ).type;

        Constraint constraint;
        constraint.type = GE;
        constraint.constant = prob.resources.at( pre_type ).delay;

        for ( auto l = 1u; l <= num_timeframes; ++l )
        {
            /* calculating timeframe of this operation */
            constraint.variables.emplace_back( Variable( {i, l} ) );
            constraint.coefficients.emplace_back( l );

            /* calculating timeframe of predecessor operation */
            if ( pre != 0 )
            {
              constraint.variables.emplace_back( Variable( {pre, l} ) );
              constraint.coefficients.emplace_back( -(int)l );
            }
        }
        constraints.emplace_back( constraint );
      }
    }
  }

  void resource_bounds()
  {
    /* For each type of resource: for every timeframe 
     * sum of the resource type should be LE # of resources 
     */
    for ( auto const& e: prob.resources )
    {
      char type = e.first;
      Resource r = e.second;

      if ( type == '0' )
      {
        continue;
      }
      for ( auto l = 1u; l <= num_timeframes; ++l )
      {
        Constraint constraint;
        constraint.type = LE;
        constraint.constant = r.num;
        for ( auto d = 0u; d < r.delay; ++d)
        {
          if ( l > d )
          {
            for ( auto i = 1u; i <= num_operations; ++i )
            {
              Operation const& op = prob.operations.at(i);
              if ( type == op.type )
              {
                  constraint.variables.emplace_back( Variable( {i, l - d} ) );
                  constraint.coefficients.emplace_back( 1 );
              }
            }
          }
        }
        constraints.emplace_back( constraint );
      }
    }
  }

  /* Feel free to declare more helper functions if needed. */

private:
  Problem const& prob;
  uint64_t num_operations; /* n */
  uint64_t num_timeframes; /* lambda bar + 1 */
  std::vector<Constraint> constraints;
};
