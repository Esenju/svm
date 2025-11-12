// vkit/env.hpp
#pragma once
#include "factory.hpp"
#include "sequencer.hpp"
#include <fstream>

struct env : vkit::component {
  vkit::sequencer* seq{};
  factory<vkit::agent> agent_factory;
  std::string manifest_path;
  sc_core::sc_module* soc{}; // provided by tb_top

  env(sc_core::sc_module_name nm, const std::string& manifest)
  : vkit::component(nm), manifest_path(manifest) {}

  void build_phase() override {
    // Register known agents
    agent_factory.reg("uart",    [](auto n){ return new uart_agent(n.c_str()); });
    agent_factory.reg("spi",     [](auto n){ return new spi_agent(n.c_str()); });
    agent_factory.reg("axi_dma", [](auto n){ return new axi_dma_agent(n.c_str()); });
    agent_factory.reg("timer",   [](auto n){ return new timer_agent(n.c_str()); });
  }

  void connect_phase() override {
    // Parse manifest, instantiate agents, connect sockets
    nlohmann::json j; std::ifstream(manifest_path) >> j;
    for (auto& ip : j["ips"]) {
      auto* ag = agent_factory.make(ip["type"], (ip["name"].get<std::string>() + "_agent"));
      if (!ag) { SC_REPORT_ERROR(name(), "Unknown IP type"); continue; }
      // TODO: find sockets on SoC by naming convention and bind to agent’s driver/monitor
      seq->register_agent(ip["name"], ag);
    }
  }
};
