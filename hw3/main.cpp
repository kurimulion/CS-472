#include "problem.hpp"
#include "ILP.hpp"
#include "checker.hpp"

#include <iostream>
#include <cstdlib>

//using namespace std;

int main( int argc, char* argv[] )
{
  if ( argc != 2 )
  {
    std::cerr << "[e] Missing input filename." << std::endl;
    std::cout << "Usage: ./hw3 <filename>" << std::endl;
    return -1;
  }

  Problem problem( argv[1] );
  problem.print(); /* Uncomment this line to see the sequencing graph in a (maybe) more comprehensive way. */
  ILP lp( problem );
  lp.dump_lp();
  //std::system( "lp_solve scheduling.lp" ); /* Uncomment this line to solve with lp_solve. */

#if 1 /* Change this line to #if 1 to run the grading system. */
  std::system( "/Users/chiaochuo/Downloads/lp_solve_5.5/lp_solve/bin/osx64/lp_solve scheduling.lp &> solution.txt" );
  Checker checker( problem );
  checker.check( "solution.txt" );
  //checker.print_schedule(); /* Uncomment this line to see the scheduling result in a (maybe) more comprehensive way. */
#endif

  return 0;
}
