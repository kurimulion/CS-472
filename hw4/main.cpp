#include "truth_table.hpp"
#include "operators.hpp"
#include "cofactors.hpp"

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main( int argc, char* argv[] )
{
  /* Example for bit-wise operation */
  if ( argc == 1 )
  {
    Truth_Table f( 3, 0xd1 );
    Truth_Table g( 3, 0xcc );
    cout << "===== Example 1 =====" << endl;
    cout << "truth table of  f : " << f << endl;
    cout << "truth table of  g : " << g << endl;
    cout << "truth table of  f': " << ( ~f ) << endl;
    cout << "truth table of f+g: " << ( f | g ) << endl;
    cout << "truth table of f.g: " << ( f & g ) << endl;
    cout << "truth table of f^g: " << ( f ^ g ) << endl;
  }
  else if ( argc == 2 )
  {
    string str( argv[1] );
    str.erase( std::remove( str.begin(), str.end(), '_' ), str.end() );
    Truth_Table tt( str );
    if ( tt.num_vars() == 0u )
    {
      cout << "Error: Invalid truth table (the length should be a power of 2)." << endl;
      cout << "usage: ./hw4 <binary_truth_table>" << endl;
      return -1;
    }

    for ( auto i = 0u; i < tt.num_vars(); ++i )
    {
      cout << "positive cofactor w.r.t. variable " << i << ": " << tt.positive_cofactor( i ) << endl;
      cout << "negative cofactor w.r.t. variable " << i << ": " << tt.negative_cofactor( i ) << endl;
      cout << "       derivative w.r.t. variable " << i << ": " << tt.derivative( i ) << endl;
      cout << "        consensus w.r.t. variable " << i << ": " << tt.consensus( i ) << endl;
      cout << "        smoothing w.r.t. variable " << i << ": " << tt.smoothing( i ) << endl << endl;
    }
  }
  else
  {
    cout << "Error: Too many arguments." << endl;
    cout << "usage: ./hw4 <binary_truth_table>" << endl;
  }

  return 0;
}
