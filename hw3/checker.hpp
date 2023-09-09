#pragma once

#include "problem.hpp"

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

class Checker
{
  using Resource = Problem::Resource;
  using Operation = Problem::Operation;

public:
  Checker( Problem const& p )
    : prob( p ), start_times( p.operations.size(), 0u )
  { }

  void check( std::string const& filename, std::ostream& os = std::cout )
  {
    std::ifstream fin( filename, std::ifstream::in );
    if ( !fin.is_open() )
    {
      os << "[e] Error opening the solution file (dump of lp_solve print-out)." << std::endl;
      return;
    }

    /* parsing */
    std::string line, obj;
    std::getline( fin, line ); /* first line is empty */
    std::getline( fin, obj ); /* second line is the value of objective function */
    std::getline( fin, line ); /* third line is empty */
    std::getline( fin, line ); /* fourth line is useless */
    while ( std::getline( fin, line ) )
    {
      std::string var_name, value = "";
      std::stringstream ss( line );
      std::getline( ss, var_name, ' ' );
      while ( value.size() == 0u && std::getline( ss, value, ' ' ) ) { }
      if ( value[0] == '1' )
      {
        std::string str_i, str_l;
        std::stringstream ss2( var_name.erase( 0, 1 ) );
        std::getline( ss2, str_i, '_' );
        std::getline( ss2, str_l );
        assert( std::stoi( str_i ) < start_times.size() );
        start_times.at( std::stoi( str_i ) ) = std::stoi( str_l );
      }
    }

    /* check that every operations are assigned */
    assert( start_times.at( 0u ) == 0u );
    for ( uint32_t op_id = 1u; op_id < prob.operations.size() - 1; ++op_id )
    {
      if ( start_times.at( op_id ) == 0u || start_times.at( op_id ) > prob.latency_bound )
      {
        std::cerr << "[e] Invalid scheduling solution: starting time out of bounds." << std::endl;
        return;
      }
    }

    /* check sequencing relations */
    for ( uint32_t op_id = 1u; op_id < prob.operations.size(); ++op_id )
    {
      Operation const& op = prob.operations.at( op_id );
      for ( uint32_t const& pre : op.predecessors )
      {
        uint32_t delay = prob.resources.at( prob.operations.at( pre ).type ).delay;
        if ( start_times.at( pre ) + delay > start_times.at( op_id ) )
        {
          os << "[e] Invalid scheduling solution: sequencing relation does not hold." << std::endl;
          return;
        }
      }
    }
    
    /* check resource constraints */
    for ( uint32_t l = 1u; l <= prob.latency_bound; ++l )
    {
      std::vector<uint32_t> operating;
      for ( uint32_t op_id = 1u; op_id < prob.operations.size() - 1; ++op_id )
      {
        Operation const& op = prob.operations.at( op_id );
        if ( start_times.at( op_id ) <= l && start_times.at( op_id ) + prob.resources.at( op.type ).delay > l )
        {
          operating.emplace_back( op_id );
        }
      }
      for ( auto it = prob.resources.begin(); it != prob.resources.end(); ++it )
      {
        uint32_t count = 0u;
        for ( uint32_t const& op_id : operating )
        {
          Operation const& op = prob.operations.at( op_id );
          if ( op.type == it->first )
          {
            ++count;
          }
        }
        if ( count > it->second.num )
        {
          os << "[e] Invalid scheduling solution: resource constraint does not meet." << std::endl;
          return;
        }
      }
    }

    os << obj << std::endl;
  }

  void print_schedule() const
  {
    std::cout << "=== start time ===" << std::endl;
    for ( auto i = 0u; i < prob.operations.size(); ++i )
    {
      Operation const& op = prob.operations.at( i );
      std::cout << i << "(" << ( op.type == '0' ? "NOP" : std::string( 1, op.type ) ) 
                << "): timeframe " << start_times.at( i ) << std::endl;
    }

    std::cout << "=== scheduled sequencing graph ===" << std::endl;
    for ( uint32_t l = 0u; l <= prob.latency_bound + 1; ++l )
    {
      std::vector<uint32_t> operating;
      for ( uint32_t op_id = 0u; op_id < prob.operations.size(); ++op_id )
      {
        Operation const& op = prob.operations.at( op_id );
        if ( start_times.at( op_id ) <= l && start_times.at( op_id ) + prob.resources.at( op.type ).delay > l )
        {
          operating.emplace_back( op_id );
        }
      }
      std::cout << "timeframe " << std::setw(2) << l << ": ";
      for ( auto const& op_id : operating )
      {
        Operation const& op = prob.operations.at( op_id );
        std::cout << op_id << "(" << ( op.type == '0' ? "NOP" : std::string( 1, op.type ) ) << ") ";
      }
      std::cout << std::endl;
    }
    std::cout << "=== latency: " << start_times.back() - 1 << " ===" << std::endl; 
  }

public:
  Problem const& prob;
  std::vector<uint32_t> start_times;
};
