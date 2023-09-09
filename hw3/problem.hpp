#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <cassert>

class Problem
{
public:
  struct Resource
  {
    uint32_t delay; /* number of cycles it takes */
    uint32_t num; /* maximum number of concurrent operations */
  };

  struct Operation
  {
    char type; /* name of the resource type it uses */
    std::vector<uint32_t> predecessors; /* IDs of the operations that should be scheduled before it */
  };

public:
  Problem( std::string const& filename )
  {
    std::ifstream fin( filename, std::ifstream::in );
    if ( !fin.is_open() )
    {
      std::cerr << "[e] Error opening the input file." << std::endl;
      return;
    }

    std::string line;
    uint32_t m, n;

    /* first line: m n lambda */
    std::getline( fin, line );
    read_header( line, m, n );

    add_NOP_type();
    for ( auto i = 0u; i < m; ++i )
    {
      std::getline( fin, line );
      add_resource_type( line );
    }

    add_first_NOP();
    for ( auto i = 1u; i <= n; ++i )
    {
      std::getline( fin, line );
      add_operation( line );
    }
  }

  void print() const
  {
    std::cout << "=== resource types ===" << std::endl;
    std::cout << "NOP" << std::endl;
    for ( auto it = resources.begin(); it != resources.end(); ++it )
    {
      if ( it->first == '0' ) { continue; }
      std::cout << it->first << ": d = " << it->second.delay << ", a = " << it->second.num << std::endl;
    }

    std::cout << "=== sequencing graph ===" << std::endl;
    std::cout << "[0(NOP)]" << std::endl;
    for ( auto i = 1u; i < operations.size(); ++i )
    {
      std::cout << "[" << i << "(" << ( operations.at( i ).type == '0' ? "NOP" : std::string( 1, operations.at( i ).type ) ) << ")] <- ";
      for ( auto const& p : operations.at( i ).predecessors )
      {
        std::cout << p << "(" << ( operations.at( p ).type == '0' ? "NOP" : std::string( 1, operations.at( p ).type ) ) << ") ";
      }
      std::cout << std::endl;
    }
  }
  
private:
  void read_header( std::string const& line, uint32_t& m, uint32_t& n )
  {
    std::string str;
    std::stringstream ss( line );
    std::getline( ss, str, ' ' );
    m = std::stoi( str );

    std::getline( ss, str, ' ' );
    n = std::stoi( str );

    std::getline( ss, str, ' ' );
    latency_bound = std::stoi( str );
  }

  void add_NOP_type()
  {
    resources['0'] = Resource( {1, 1} );
  }

  void add_resource_type( std::string const& line )
  {
    std::string str;
    std::stringstream ss( line );

    std::getline( ss, str, ' ' );
    char const name = str[0];

    std::getline( ss, str, ' ' );
    uint32_t const delay = std::stoi( str );

    std::getline( ss, str, ' ' );
    uint32_t const num = std::stoi( str );

    assert( resources.find( name ) == resources.end() );
    resources[name] = Resource( {delay, num} );
  }

  void add_first_NOP()
  {
    operations.emplace_back( Operation( {'0', {}} ) );
  }

  void add_operation( std::string const& line )
  {
    std::string str;
    std::stringstream ss( line );

    std::getline( ss, str, ' ' );
    char const type = str[0];
    assert( resources.find( type ) != resources.end() );

    operations.emplace_back( Operation( {type, {}} ) );
    auto& pred = operations.back().predecessors;

    while ( std::getline( ss, str, ' ' ) )
    {
      pred.emplace_back( std::stoi( str ) );
    }
  }

public:
  std::unordered_map<char, Resource> resources;
  std::vector<Operation> operations;
  uint64_t latency_bound; /* lambda bar */
};
