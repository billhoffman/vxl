#include <testlib/testlib_register.h>

DECLARE( test_graph_rep1 );
DECLARE( test_dp_solver );
DECLARE( test_order_cost );

void
register_tests()
{
  REGISTER( test_graph_rep1 );
  REGISTER( test_dp_solver );
  REGISTER( test_order_cost );
}

DEFINE_MAIN;
