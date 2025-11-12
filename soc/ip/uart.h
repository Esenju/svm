// soc/ip/uart.h
#pragma once
#include <systemc>
#include <tlm>
#include <vector>

struct uart : sc_core::sc_module {
  tlm::tlm_target_socket<> reg_tgt;  // register access
  tlm::tlm_initiator_socket<> tx;    // tx data stream (simplified)
  tlm::tlm_target_socket<>    rx;    // rx data stream (simplified)

  SC_HAS_PROCESS(uart);
  uart(sc_core::sc_module_name nm);
};
