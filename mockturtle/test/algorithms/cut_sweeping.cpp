#include <catch.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/algorithms/cut_sweeping.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/utils/debugging_utils.hpp>
#include <mockturtle/views/color_view.hpp>
#include <kitty/static_truth_table.hpp>
#include <lorina/aiger.hpp>
#include <fmt/format.h>

using namespace mockturtle;

TEST_CASE( "cut_sweeping: (ab)c is equivalent to a(bc)", "[cut_sweeping_unit]" )
{
  aig_network aig;
  const auto a = aig.create_pi();
  const auto b = aig.create_pi();
  const auto c = aig.create_pi();
  const auto n1 = aig.create_and( a, b );
  const auto n2 = aig.create_and( n1, c );
  const auto n3 = aig.create_and( b, c );
  const auto n4 = aig.create_and( n3, a );
  aig.create_po( n2 );
  aig.create_po( n4 );

  cut_sweeping( aig );

  /* Check size reduction */
  CHECK( aig.num_gates() == 2 );

  /* Check that the network remains acyclic */
  CHECK( network_is_acyclic( color_view{aig} ) );

  auto vals = simulate<kitty::static_truth_table<3>>( aig );
  /* Check simulation result of the first PO */
  CHECK( vals[0]._bits == 0b10000000 );
  /* Check simulation result of the second PO */
  CHECK( vals[1]._bits == 0b10000000 );
}

TEST_CASE( "cut_sweeping: XOR is antivalent to XNOR", "[cut_sweeping_unit]" )
{
  aig_network aig;
  const auto a = aig.create_pi();
  const auto b = aig.create_pi();
  const auto n1 = aig.create_and( a, !b );
  const auto n2 = aig.create_and( !a, b );
  const auto n3 = aig.create_and( a, b );
  const auto n4 = aig.create_and( !a, !b );
  const auto n5 = aig.create_or( n1, n2 );
  const auto n6 = aig.create_or( n3, n4 );
  aig.create_po( n5 );
  aig.create_po( n6 );

  cut_sweeping( aig );

  /* Check size reduction */
  CHECK( aig.num_gates() == 3 );

  /* Check that the network remains acyclic */
  CHECK( network_is_acyclic( color_view{aig} ) );

  auto vals = simulate<kitty::static_truth_table<2>>( aig );
  /* Check simulation result of the first PO (XOR) */
  CHECK( vals[0]._bits == 0b0110 );
  /* Check simulation result of the second PO (XNOR) */
  CHECK( vals[1]._bits == 0b1001 );
}

TEST_CASE( "cut_sweeping: (ab)(!a) is constant 0", "[cut_sweeping_unit]" )
{
  aig_network aig;
  const auto a = aig.create_pi();
  const auto b = aig.create_pi();
  const auto c = aig.create_pi();
  const auto n1 = aig.create_and( a, b );
  const auto n2 = aig.create_and( n1, !a );
  const auto n3 = aig.create_and( n2, c );
  aig.create_po( n3 );

  cut_sweeping( aig );

  /* Check size reduction */
  CHECK( aig.num_gates() == 0 );

  /* Check that the network remains acyclic */
  CHECK( network_is_acyclic( color_view{aig} ) );

  auto vals = simulate<kitty::static_truth_table<3>>( aig );
  /* Check simulation result of the first PO */
  CHECK( vals[0]._bits == 0b00000000 );
}

TEST_CASE( "cut_sweeping: a!b + !a!b + bc + b!c is constant 1", "[cut_sweeping_unit]" )
{
  aig_network aig;
  const auto a = aig.create_pi();
  const auto b = aig.create_pi();
  const auto c = aig.create_pi();
  const auto n1 = aig.create_and( a, !b );
  const auto n2 = aig.create_and( !a, !b );
  const auto n3 = aig.create_and( b, c );
  const auto n4 = aig.create_and( b, !c );
  const auto n5 = aig.create_or( n1, n4 );
  const auto n6 = aig.create_or( n5, n3 );
  const auto n7 = aig.create_or( n6, n2 );
  aig.create_po( n7 );

  cut_sweeping( aig );

  /* Check size reduction */
  CHECK( aig.num_gates() == 0 );

  /* Check that the network remains acyclic */
  CHECK( network_is_acyclic( color_view{aig} ) );

  auto vals = simulate<kitty::static_truth_table<3>>( aig );
  /* Check simulation result of the PO */
  CHECK( vals[0]._bits == 0b11111111 );
}

TEST_CASE( "cut_sweeping: example in the presentation", "[cut_sweeping_unit]" )
{
  aig_network aig;
  const auto x1 = aig.create_pi();
  const auto x2 = aig.create_pi();
  const auto x3 = aig.create_pi();
  const auto n4 = aig.create_and( x1, x2 );
  const auto n5 = aig.create_and( !x2, x3 );
  const auto n6 = aig.create_and( x1, !x3 );
  const auto n7 = aig.create_and( x1, !n5 );
  const auto n8 = aig.create_and( n4, x3 );
  const auto n9 = aig.create_and( !n8, !n6 );
  aig.create_po( n7 );
  aig.create_po( n9 );

  cut_sweeping( aig );

  /* Check size reduction */
  CHECK( aig.num_gates() < 6 );

  /* Check that the network remains acyclic */
  CHECK( network_is_acyclic( color_view{aig} ) );

  auto vals = simulate<kitty::static_truth_table<3>>( aig );
  /* Check simulation result of the first PO */
  CHECK( vals[0]._bits == 0b10001010 );
  /* Check simulation result of the second PO */
  CHECK( vals[1]._bits == 0b01110101 );
}

TEST_CASE( "cut_sweeping: equivalent nodes in the middle of network", "[cut_sweeping_unit]" )
{
  aig_network aig;
  const auto x1 = aig.create_pi();
  const auto x2 = aig.create_pi();
  const auto x3 = aig.create_pi();
  const auto x4 = aig.create_pi();
  const auto n5 = aig.create_and( x1, x2 );
  const auto n6 = aig.create_and( x2, x3 );
  const auto n7 = aig.create_and( x3, n5 ); // x1x2x3
  const auto n8 = aig.create_and( n7, x1 ); // x1x2x3
  const auto n9 = aig.create_and( n8, n6 ); // x1x2x3
  const auto n10 = aig.create_and( x4, n6 ); // x2x3x4
  const auto n11 = aig.create_and( n9, n10 ); // x1x2x3x4
  aig.create_po( n11 );

  cut_sweeping( aig );

  /* Check size reduction */
  CHECK( aig.num_gates() < 7 );

  /* Check that the network remains acyclic */
  CHECK( network_is_acyclic( color_view{aig} ) );

  auto vals = simulate<kitty::static_truth_table<4>>( aig );
  /* Check simulation result of the PO */
  CHECK( vals[0]._bits == 0b1000000000000000 );
}

TEST_CASE( "cut_sweeping: baseline QoR and efficiency test K=8 N=5", "[cut_sweeping_baseline]" )
{
  cut_sweeping_params ps;
  ps.cut_size = 8;
  ps.max_num_cuts = 5;

  std::vector<uint32_t> benchmark_ids = { 432, 1908, 2670, 3540, 5315, 6288, 7552 };
  std::vector<int32_t> expected_gains = {   4,    9,   14,    3,    7,    0,    0 };
  for ( auto i = 0u; i < benchmark_ids.size(); ++i )
  {
    aig_network ntk;
    auto result = lorina::read_aiger( fmt::format( "{}/c{}.aig", BENCHMARKS_PATH, benchmark_ids[i] ), aiger_reader( ntk ) );
    CHECK( result == lorina::return_code::success );
    fmt::print( "[i] testing c{}: #PIs={}, #POs={}, #gates={}\n", benchmark_ids[i], ntk.num_pis(), ntk.num_pos(), ntk.num_gates() );

    uint32_t const size_before = ntk.num_gates();
    stopwatch<>::duration runtime{0};
    call_with_stopwatch( runtime, [&](){ cut_sweeping( ntk, ps ); } );
    int32_t const gain = size_before - ntk.num_gates();
    fmt::print( "[i] after cut sweeping, #gates={}, gain={}, runtime={:>5.2f} secs\n", ntk.num_gates(), gain, to_seconds( runtime ) );
    CHECK( gain >= expected_gains[i] );
    CHECK( to_seconds( runtime ) < 0.1 );
    CHECK( network_is_acyclic( color_view{ntk} ) );

    aig_network ntk_ori;
    result = lorina::read_aiger( fmt::format( "{}/c{}.aig", BENCHMARKS_PATH, benchmark_ids[i] ), aiger_reader( ntk_ori ) );
    const auto cec = equivalence_checking( *miter<aig_network>( ntk, ntk_ori ) );
    CHECK( *cec );
  }
}

TEST_CASE( "cut_sweeping: baseline QoR and efficiency test K=12 N=10", "[cut_sweeping_baseline]" )
{
  cut_sweeping_params ps;
  ps.cut_size = 12;
  ps.max_num_cuts = 10;

  std::vector<uint32_t> benchmark_ids = { 432, 1908, 2670, 3540, 5315, 6288, 7552 };
  std::vector<int32_t> expected_gains = {  16,    9,   18,   22,    7,    0,    3 };
  for ( auto i = 0u; i < benchmark_ids.size(); ++i )
  {
    aig_network ntk;
    auto result = lorina::read_aiger( fmt::format( "{}/c{}.aig", BENCHMARKS_PATH, benchmark_ids[i] ), aiger_reader( ntk ) );
    CHECK( result == lorina::return_code::success );
    fmt::print( "[i] testing c{}: #PIs={}, #POs={}, #gates={}\n", benchmark_ids[i], ntk.num_pis(), ntk.num_pos(), ntk.num_gates() );

    uint32_t const size_before = ntk.num_gates();
    stopwatch<>::duration runtime{0};
    call_with_stopwatch( runtime, [&](){ cut_sweeping( ntk, ps ); } );
    int32_t const gain = size_before - ntk.num_gates();
    fmt::print( "[i] after cut sweeping, #gates={}, gain={}, runtime={:>5.2f} secs\n", ntk.num_gates(), gain, to_seconds( runtime ) );
    CHECK( gain >= expected_gains[i] );
    CHECK( to_seconds( runtime ) < 0.5 );
    CHECK( network_is_acyclic( color_view{ntk} ) );

    aig_network ntk_ori;
    result = lorina::read_aiger( fmt::format( "{}/c{}.aig", BENCHMARKS_PATH, benchmark_ids[i] ), aiger_reader( ntk_ori ) );
    const auto cec = equivalence_checking( *miter<aig_network>( ntk, ntk_ori ) );
    CHECK( *cec );
  }
}

struct result_t
{
  std::string name;
  int32_t gain;
  double runtime;
};

TEST_CASE( "alpha test", "[quality]" )
{
  cut_sweeping_params ps;
  std::vector<result_t> results;
  std::vector<std::string> benchmarks = {"i10", "tv80"};

  for ( auto benchmark : benchmarks )
  {
    aig_network ntk;
    auto read = lorina::read_aiger( fmt::format( "{}/{}.aig", BENCHMARKS_PATH, benchmark ), aiger_reader( ntk ) );
    (void)read;
    uint32_t const size_before = ntk.num_gates();
    if ( size_before < 2000 )
    {
      ps.cut_size = 12;
      ps.max_num_cuts = 10;
    }
    else
    {
      ps.cut_size = 8;
      ps.max_num_cuts = 5;
    }
    stopwatch<>::duration runtime{0};
    call_with_stopwatch( runtime, [&](){ cut_sweeping( ntk, ps ); } );
    int32_t const gain = size_before - ntk.num_gates();

    if ( !network_is_acyclic( color_view{ntk} ) )
    {
      std::cout << "[e] Network becomes cyclic after cut sweeping\n";
      continue;
    }
    if ( size_before < 2000 )
    {
      aig_network ntk_ori;
      read = lorina::read_aiger( fmt::format( "{}/i10.aig", BENCHMARKS_PATH ), aiger_reader( ntk_ori ) );
      if ( !*equivalence_checking( *miter<aig_network>( ntk, ntk_ori ) ) )
      {
        std::cout << "[e] Network is not functionally equivalent to the original one after cut sweeping\n";
        continue;
      }
    }
    results.emplace_back( result_t{benchmark, gain, to_seconds( runtime )} );
  }

  std::cout << "========= Alpha test report =========\n";
  for ( auto const& r : results )
  {
    fmt::print( "{:>5}: gain = {:>4}, runtime = {:>7.3f}\n", r.name, r.gain, r.runtime );
  }
}

TEST_CASE( "beta test", "[quality]" )
{
  cut_sweeping_params ps;
  std::vector<result_t> results;
  std::vector<std::string> benchmarks = {"i10", "tv80", "rnd11", "rnd20"};

  for ( auto benchmark : benchmarks )
  {
    aig_network ntk;
    auto read = lorina::read_aiger( fmt::format( "{}/{}.aig", BENCHMARKS_PATH, benchmark ), aiger_reader( ntk ) );
    (void)read;
    uint32_t const size_before = ntk.num_gates();
    if ( size_before < 5000 ) // i10
    {
      ps.cut_size = 12;
      ps.max_num_cuts = 10;
    }
    else if ( size_before < 15000 ) // tv80, rnd11
    {
      ps.cut_size = 8;
      ps.max_num_cuts = 5;
    }
    else // rnd20
    {
      ps.cut_size = 4;
      ps.max_num_cuts = 3;
    }
    stopwatch<>::duration runtime{0};
    call_with_stopwatch( runtime, [&](){ cut_sweeping( ntk, ps ); } );
    int32_t const gain = size_before - ntk.num_gates();

    if ( !network_is_acyclic( color_view{ntk} ) )
    {
      fmt::print( "[e] benchmark {}: Network becomes cyclic after cut sweeping\n", benchmark );
      continue;
    }

    aig_network ntk_ori;
    read = lorina::read_aiger( fmt::format( "{}/{}.aig", BENCHMARKS_PATH, benchmark ), aiger_reader( ntk_ori ) );
    if ( !*equivalence_checking( *miter<aig_network>( ntk, ntk_ori ) ) )
    {
      fmt::print( "[e] benchmark {}: Network is not functionally equivalent to the original one after cut sweeping\n", benchmark );
      continue;
    }
    results.emplace_back( result_t{benchmark, gain, to_seconds( runtime )} );
  }

  std::cout << "========= Beta test report =========\n";
  for ( auto const& r : results )
  {
    fmt::print( "{:>5}: gain = {:>4}, runtime = {:>7.3f}\n", r.name, r.gain, r.runtime );
  }
}

TEST_CASE( "final test", "[quality]" )
{
  cut_sweeping_params ps;
  std::vector<result_t> results;
  std::vector<std::string> benchmarks = {"i10", "tv80", "rnd20", "miter", "topo"};

  for ( auto benchmark : benchmarks )
  {
    /* The last 2 "advanced" benchmarks are only run if
       1. The results of the first 3 "basic" benchmarks are all valid AND
       2. The total runtime of the basic benchmarks is smaller than 5 seconds */
    if ( benchmark == "miter" )
    {
      if ( results.size() < 3 )
        break;

      double total_runtime{0.0};
      for ( auto const& r : results )
        total_runtime += r.runtime;
      if ( total_runtime > 5.0 )
        break;
    }

    aig_network ntk;
    auto read = lorina::read_aiger( fmt::format( "{}/{}.aig", BENCHMARKS_PATH, benchmark ), aiger_reader( ntk ) );
    (void)read;
    uint32_t const size_before = ntk.num_gates();
    if ( size_before < 5000 ) // i10
    {
      ps.cut_size = 12;
      ps.max_num_cuts = 10;
    }
    else if ( size_before < 15000 ) // tv80
    {
      ps.cut_size = 8;
      ps.max_num_cuts = 5;
    }
    else if ( size_before < 25000) // rnd20
    {
      ps.cut_size = 4;
      ps.max_num_cuts = 3;
    }
    else // "advanced"
    {
      ps.cut_size = 3;
      ps.max_num_cuts = 5;
    }
    stopwatch<>::duration runtime{0};
    call_with_stopwatch( runtime, [&](){ cut_sweeping( ntk, ps ); } );
    int32_t const gain = size_before - ntk.num_gates();

    if ( !network_is_acyclic( color_view{ntk} ) )
    {
      fmt::print( "[e] benchmark {}: Network becomes cyclic after cut sweeping\n", benchmark );
      continue;
    }

    aig_network ntk_ori;
    read = lorina::read_aiger( fmt::format( "{}/{}.aig", BENCHMARKS_PATH, benchmark ), aiger_reader( ntk_ori ) );
    if ( !*equivalence_checking( *miter<aig_network>( ntk, ntk_ori ) ) )
    {
      fmt::print( "[e] benchmark {}: Network is not functionally equivalent to the original one after cut sweeping\n", benchmark );
      continue;
    }
    results.emplace_back( result_t{benchmark, gain, to_seconds( runtime )} );
  }

  std::cout << "========= Final test report =========\n";
  for ( auto const& r : results )
  {
    fmt::print( "{:>5}: gain = {:>5}, runtime = {:>7.3f}\n", r.name, r.gain, r.runtime );
  }
}
