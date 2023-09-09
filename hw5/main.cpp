#include "truth_table.hpp"
#include "BDD.hpp"
#include "operations.hpp"

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

void check( Truth_Table const& tt, string const& ans )
{
  cout << "  checking function correctness";
  if ( tt == Truth_Table( ans ) )
  {
    cout << "...passed." << endl;
  }
  else
  {
    cout << "...failed. (expect " << ans << ", but get " << tt << ")" << endl;
  }
}

void check( uint64_t dd_size, uint64_t expected )
{
  cout << "  checking BDD size";
  if ( dd_size == expected )
  {
    cout << "...passed." << endl;
  }
  else
  {
    cout << "...failed. (expect " << expected << ", but get " << dd_size << " nodes)" << endl;
  }
}

int main()
{
  {
    cout << "test 00: x0 XOR x1" << endl;
    BDD bdd( 2 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const f = bdd.XOR( x0, x1 );
    auto const tt = bdd.get_tt( f );
    //bdd.print( f );
    //cout << tt << endl;
    check( tt, "0110" );
    check( bdd.num_nodes( f ), 3 );
  }

  {
    cout << "test 01: x0 AND x1" << endl;
    BDD bdd( 2 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const f = bdd.AND( x0, x1 );
    auto const tt = bdd.get_tt( f );
    // bdd.print( f );
    // cout << tt << endl;
    check( tt, "1000" );
    check( bdd.num_nodes( f ), 2 );
  }

  {
    cout << "test 02: ITE(x0, x1, x2)" << endl;
    BDD bdd( 3 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const x2 = bdd.literal( 2 );
    auto const f = bdd.ITE( x0, x1, x2 );
    auto const tt = bdd.get_tt( f );
    // bdd.print( f );
    // cout << tt << endl;
    check( tt, "11011000" );
    check( bdd.num_nodes( f ), 3 );
  }

  /* Feel free to add more tests. */

  {
    cout << "test 03: ITE(x1, x0, x2)" << endl;
    BDD bdd( 3 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const x2 = bdd.literal( 2 );
    auto const f = bdd.ITE( x1, x0, x2 );
    auto const tt = bdd.get_tt( f );
    // bdd.print( f );
    // cout << tt << endl;
    check( tt, "10111000" );
    check( bdd.num_nodes( f ), 4 );
  }

  {
    cout << "test 04: ITE(x1, x0, x0)" << endl;
    BDD bdd( 3 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const x2 = bdd.literal( 2 );
    auto const f = bdd.ITE( x1, x0, x0 );
    auto const tt = bdd.get_tt( f );
    bdd.print( f );
    cout << tt << endl;
    check( tt, "10101010" );
    check( bdd.num_nodes( f ), 1 );
  }

  {
    cout << "test 05: ITE(x0, x0, x0)" << endl;
    BDD bdd( 3 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const x2 = bdd.literal( 2 );
    auto const f = bdd.ITE( x0, x0, x0 );
    auto const tt = bdd.get_tt( f );
    // bdd.print( f );
    // cout << tt << endl;
    check( tt, "10101010" );
    check( bdd.num_nodes( f ), 1 );
  }

  {
    cout << "test 06: ITE(x0, x0, x2)" << endl;
    BDD bdd( 3 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const x2 = bdd.literal( 2 );
    auto const f = bdd.ITE( x0, x0, x2 );
    auto const tt = bdd.get_tt( f );
    // bdd.print( f );
    // cout << tt << endl;
    check( tt, "11111010" );
    check( bdd.num_nodes( f ), 2 );
  }

  {
    cout << "test 07: ITE(x0, ITE(x1, x2, x2), x2)" << endl;
    BDD bdd( 3 );
    auto const x0 = bdd.literal( 0 );
    auto const x1 = bdd.literal( 1 );
    auto const x2 = bdd.literal( 2 );
    auto const f = bdd.ITE( x0, bdd.ITE( x1, x2, x2 ), x2 );
    auto const tt = bdd.get_tt( f );
    // bdd.print( f );
    // cout << tt << endl;
    check( tt, "11110000" );
    check( bdd.num_nodes( f ), 1 );
  }

  return 0;
}
