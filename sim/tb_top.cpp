// sim/tb_top.cpp
#include <systemc>
#include "soc/soc_top.h"
#include "vkit/env.hpp"

struct tb_top : sc_core::sc_module {
  soc_top* soc{}; env* e{}; vkit::sequencer* seq{};
  SC_CTOR(tb_top) {
    soc = new soc_top("dut");
    seq = new vkit::sequencer("sequencer", std::filesystem::path("tests/generated"));
    e   = new env("env", "soc/manifest.json");
    e->seq = seq; e->soc = soc;
  }
};
