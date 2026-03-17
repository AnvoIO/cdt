#include <core_net/system.h>
#include <stddef.h>

void test_system( void ) {
   core_net_assert(0, NULL);
   core_net_assert_message(0, NULL, 0);
   core_net_assert_code(0, 0);
   core_net_exit(0);
   current_time();
   is_feature_activated(NULL);
   get_sender();
}
