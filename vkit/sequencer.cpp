// vkit/sequencer.cpp (excerpt)
#include "sequencer.hpp"
#include "agents/uart_agent.hpp"
#include "agents/spi_agent.hpp"
#include "agents/axi_dma_agent.hpp"
#include "agents/timer_agent.hpp"

using nlohmann::json;

std::unique_ptr<vkit::sequence_item>
vkit::sequencer::ag_deserialize(const std::string& ip, const json& v) {
  if (ip.rfind("uart",0)==0) {
    auto p = std::make_unique<items::uart_tx>();
    p->baud   = v.value("baud",115200);
    p->parity = v.value("parity",false);
    for (auto b : v["payload"]) p->payload.push_back(uint8_t(b));
    return p;
  } else if (ip.rfind("spi",0)==0) {
    auto p = std::make_unique<items::spi_xfer>();
    p->mode = v.value("mode",0);
    for (auto b : v["tx"]) p->tx.push_back(uint8_t(b));
    return p;
  } else if (ip.rfind("dma",0)==0) {
    auto p = std::make_unique<items::dma_burst>();
    p->len = v.value("len",0); p->src=v.at("src"); p->dst=v.at("dst");
    return p;
  } else if (ip.rfind("timer",0)==0) {
    auto p = std::make_unique<items::timer_cmd>();
    p->start = (v.value("op",std::string("start")) == "start");
    p->period_us = v.value("period_us",10);
    return p;
  }
  return {};
}
