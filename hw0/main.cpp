#include "graph.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace hw0;

int main( int argc, char* argv[] )
{
  if ( argc != 3 )
  {
    cerr << "[e] please provide input file name and the requested k." << endl;
    return -1;
  }

  ifstream fin( argv[1], ifstream::in );
  if ( !fin.is_open() )
  {
    cerr << "[e] error opening the input file." << endl;
    return -1;
  }

  string line, str;
  uint32_t root, node;

  /* the first line contains the number of nodes (0 <= node_index <= num_nodes - 1) */
  getline( fin, line );
  Graph g( stoi( line ) );
  
  /* the remaining lines encode the edges of the graph */
  while ( getline( fin, line ) )
  {
    stringstream ss( line );

    /* the first number in each line is the root node */
    getline( ss, str, ' ' );
    root = stoi( str );

    /* the following numbers are the nodes `root` is connected to */
    while ( getline( ss, str, ' ' ) )
    {
      node = stoi( str );
      g.add_edge( root, node );
    }
  }

  fin.close();

  const uint32_t answer = g.num_degree_k( atoi( argv[2] ) );
  cout << "[ans] number of nodes of degree " << atoi( argv[2] ) << " is: " << answer << endl;

  return 0;
}