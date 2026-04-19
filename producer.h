#ifndef PRODUCER_H
#define PRODUCER_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_LEAN_AND_MEAN

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <iostream>
#include "config.h"

using namespace sc_core;
using namespace tlm;

// Image dimensions for the generated pattern
static const int IMG_WIDTH = 780;
static const int IMG_HEIGHT = 610;

/**
 * @class Producer
 * @brief SystemC Module representing a Test Pattern Generator (TPG).
 *
 * This module generates a checkerboard pattern image and sends it 
 * pixel by pixel over a TLM-2.0 socket to a consumer (Memory/Display).
 */
SC_MODULE(Producer) {
    // TLM-2.0 Initiator Socket to send transactions out
    tlm_utils::simple_initiator_socket<Producer> initiator_socket;

    // We store the payload and data buffer as class members rather than 
    // local variables in the coroutine process. This prevents stack overflows 
    // caused by the large payload sizes in modern SystemC versions on Windows.
    unsigned char data_buf[4];
    tlm_generic_payload trans;

    // Constructor setup
    SC_CTOR(Producer) {
        // Initialize the reusable TLM transaction once to optimize simulation time
        trans.set_command(TLM_WRITE_COMMAND);
        trans.set_data_ptr(data_buf);
        trans.set_data_length(4);
        trans.set_streaming_width(4);

        // Register the main processing thread
        SC_THREAD(process);
    }

    /**
     * @brief The main simulation thread that generates pixels.
     * It iterates through a 2D image matrix, computes a checkerboard pattern, 
     * packs it into 32-bit ARGB, and fires a TLM payload.
     */
    void process() {
        sc_time delay = SC_ZERO_TIME;

        size_t total_pixels = (size_t)IMG_WIDTH * IMG_HEIGHT;
        size_t half = total_pixels / 2;

        for (int y = 0; y < IMG_HEIGHT; y++) {
            for (int x = 0; x < IMG_WIDTH; x++) {

                size_t pixel_index = (size_t)y * IMG_WIDTH + x;

                // Generator logic: creates a 20x20 checkerboard pattern
                bool white = ((x / 20) + (y / 20)) % 2 == 0;
                uint32_t pix = white ? 0x00FFFFFF : 0x00FF6600;

                // Little-endian byte extraction (R, G, B, Alpha)
                data_buf[0] = (pix >> 16) & 0xFF; // R
                data_buf[1] = (pix >> 8)  & 0xFF; // G
                data_buf[2] = (pix >> 0)  & 0xFF; // B
                data_buf[3] = 0x00;               // A

                uint64_t addr;

#if MODE == 2
                // Mode 2: Split processing across two separate base addresses (2 Consumers)
                if (pixel_index < half)
                    addr = BASE1 + pixel_index * 4;
                else
                    addr = BASE2 + (pixel_index - half) * 4;
#else
                // Standard mode (Part A / Part B): contiguous flat addressing 
                addr = pixel_index * 4;
#endif

                // Attach dynamically calculated address and reset response status
                trans.set_address(addr);
                trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

                // Send the transaction (blocking transport)
                initiator_socket->b_transport(trans, delay);

                // Check for errors from the router or target memory
                if (trans.get_response_status() != TLM_OK_RESPONSE) {
                    SC_REPORT_WARNING("Producer",
                        (std::string("Transaction failed at addr=0x") +
                         std::to_string(addr)).c_str());
                }

                // Advance simulation time according to accumulated delays
                wait(delay);
                delay = SC_ZERO_TIME;
            }
        }

        std::cout << "[Producer] Frame complete at " << sc_time_stamp() << std::endl;
    }
};

#endif