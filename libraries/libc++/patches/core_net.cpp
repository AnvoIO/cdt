#pragma once

/**
 * Define specific things for the core_net system
 */

extern "C" {
   void core_net_assert(unsigned int, const char*);
   void __cxa_pure_virtual() { core_net_assert(false, "pure virtual method called"); }
}
