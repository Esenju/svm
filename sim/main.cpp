// sim/main.cpp
#include <systemc>
#include "tb_top.cpp"
int sc_main(int argc, char* argv[]) {
  tb_top tb("tb");
  sc_core::sc_start();
  return 0;
}
