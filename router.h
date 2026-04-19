#ifndef ROUTER_H
#define ROUTER_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_LEAN_AND_MEAN

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <iostream>
#include "config.h"

using namespace sc_core;
using namespace tlm;

/**
 * @class Router
 * @brief SystemC Interconnect Module handling memory-mapped address decoding.
 *
 * It accepts transactions from the Producer and forwards them to the appropriate 
 * Consumer based on the memory address ranges defined in config.h.
 */
SC_MODULE(Router) {
    // Single target socket to receive transactions from the initiator (Producer)
    tlm_utils::simple_target_socket<Router> target_socket;

    // First initiator socket to forward to Consumer 1
    tlm_utils::simple_initiator_socket<Router> initiator_socket1;

#if MODE == 2
    // Second initiator socket to forward to Consumer 2 (Only active in Mode 2)
    tlm_utils::simple_initiator_socket<Router> initiator_socket2;
#endif

    // Constructor setup
    SC_CTOR(Router) {
        // Register the blocking transport callback for routing logic
        target_socket.register_b_transport(this, &Router::b_transport);
    }

    /**
     * @brief Routing intercept function for incoming transactions.
     * @param trans The incoming TLM payload.
     * @param delay Standard synchronization delay accumulator.
     */
    void b_transport(tlm_generic_payload & trans, sc_time & delay) {

        uint64_t addr = trans.get_address();
        
        // Annotate the simulated delay of passing through the router logic
        delay += sc_time(10, SC_NS);

#if MODE == 2
        // Interconnect logic for Dual-Consumer Memory Map
        if (addr >= BASE1 && addr < END1) {
            // Translate address to local offset before passing
            trans.set_address(addr - BASE1);
            initiator_socket1->b_transport(trans, delay);
            // Restore original absolute address after transport finishes
            trans.set_address(addr);
        }
        else if (addr >= BASE2 && addr < END2) {
            trans.set_address(addr - BASE2);
            initiator_socket2->b_transport(trans, delay);
            trans.set_address(addr);
        }
        else {
            // Signal a segmentation/address violation
            trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
        }
#else
        // Standard interconnect logic for Part B (Single Consumer)
        if (addr >= BASE1 && addr < END1) {
            trans.set_address(addr - BASE1);
            initiator_socket1->b_transport(trans, delay);
            trans.set_address(addr);
        }
        else {
            trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
        }
#endif
    }
};

#endif