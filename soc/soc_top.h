// soc/soc_top.h
#pragma once
#include <systemc>
#include <tlm>
#include <string>
#include <memory>
#include "ip/uart.h"
#include "ip/spi.h"
#include "ip/axi_dma.h"
#include "ip/timer.h"

struct soc_top : sc_core::sc_module {
  // Control bus (e.g., simple TLM target for register access)
  tlm::tlm_initiator_socket<> ctrl_init; // from testbench/agents to SoC

  // IP instances
  std::unique_ptr<uart>     u_uart;
  std::unique_ptr<spi>      u_spi;
  std::unique_ptr<axi_dma>  u_dma;
  std::unique_ptr<timer>    u_timer;

  SC_HAS_PROCESS(soc_top);
  soc_top(sc_core::sc_module_name nm);
};
