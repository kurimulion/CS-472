#include "checker.hpp"
#include "graph.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

int main( int argc, char* argv[] )
{
  if ( argc != 5 || ( argv[4][0] != 'd' && argv[4][0] != 'b' ) )
  {
    cerr << "[e] Please provide input file name, the source and sink nodes, and the algorithm to use." << endl;
    cerr << "Usage: ./hw1 <filename> <source> <sink> <d|b>" << endl;
    return -1;
  }

  ifstream fin( argv[1], ifstream::in );
  if ( !fin.is_open() )
  {
    cerr << "[e] Error opening the input file." << endl;
    return -1;
  }

  string line, str;
  uint32_t from, to;
  int32_t weight;

  /* the first line contains the number of nodes (0 <= node_index <= num_nodes - 1) */
  getline( fin, line );
  Graph g( stoi( line ) );
  Checker_Graph checker( stoi( line ) );
  
  /* the remaining lines encode the edges of the graph */
  while ( getline( fin, line ) )
  {
    stringstream ss( line );

    /* the first number in each line is the "from" node */
    getline( ss, str, ' ' );
    from = stoi( str );

    /* the second number in each line is the "to" node */
    getline( ss, str, ' ' );
    to = stoi( str );

    /* the third number in each line is the weight */
    getline( ss, str, ' ' );
    weight = stoi( str );

    g.add_edge( from, to, weight );
    checker.add_edge( from, to, weight );
  }

  vector<uint32_t> const answer = argv[4][0] == 'd' ? g.dijkstra_shortest_path( atoi( argv[2] ), atoi( argv[3] ) )
                                                    : g.bellman_ford_shortest_path( atoi( argv[2] ), atoi( argv[3] ) );

  /* check correctness & compute length */
  int32_t length = checker.check_path( answer, atoi( argv[2] ), atoi( argv[3] ) );
  
  if ( length == numeric_limits<int32_t>::min() )
  {
    cout << "[e] Wrong answer: incorrect path." << endl;
    return -1;
  }
  else if ( length == numeric_limits<int32_t>::max() )
  {
    cout << "[ans] Found that no path exists, or there is a negative-weight loop." << endl;
    return 0;
  }

  cout << "[ans] Found a path of length " << length << ": ";
  for ( uint32_t const& n : answer )
  {
    cout << n << " ";
  }
  cout << endl;

  return 0;
}
