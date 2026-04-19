#ifndef CONSUMER_H
#define CONSUMER_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_LEAN_AND_MEAN

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <vector>
#include <fstream>
#include <iostream>

using namespace sc_core;
using namespace tlm;

/**
 * @class Consumer
 * @brief SystemC Module modeling destination memory and output display.
 *
 * It accepts pixels over a TLM-2.0 socket, buffers them, and 
 * provides a method to flush the buffered frame to an image file (PPM).
 */
SC_MODULE(Consumer) {
    // TLM-2.0 Target Socket to receive transactions from Producer/Router
    tlm_utils::simple_target_socket<Consumer> target_socket;

    // Internal memory buffering pixels 
    std::vector<uint32_t> frame_buffer;

    // Dimensions constraints
    static const int WIDTH  = 780;
    static const int HEIGHT = 610;

    // Constructor setup
    SC_CTOR(Consumer) {
        // Pre-allocate the memory buffer based on width/height
        frame_buffer.resize(WIDTH * HEIGHT, 0);

        // Register the blocking transport callback function
        target_socket.register_b_transport(this, &Consumer::b_transport);
    }

    /**
     * @brief Blocking transport callback. Overridden to handle memory writes.
     * @param trans Transaction payload carrying command, address, and data pointer.
     * @param delay Reference to the accumulated simulation delay.
     */
    void b_transport(tlm_generic_payload& trans, sc_time& delay) {

        // Validate command type (Consumer only handles writes)
        if (trans.get_command() != TLM_WRITE_COMMAND) {
            trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            return;
        }

        // Validate data pointer to avoid segmentation faults
        unsigned char* ptr = trans.get_data_ptr();
        if (!ptr) {
            trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
            return;
        }

        // Address to index mapping (Addresses are byte aligned, 4 bytes/pixel)
        uint64_t idx = trans.get_address() / 4;

        // Out-of-bounds check simulating memory region protection
        if (idx >= (uint64_t)frame_buffer.size()) {
            trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }

        // Reconstruct 32-bit pixel value from byte payload (Little Endian format extraction)
        uint32_t pixel =
            ((uint32_t)ptr[0] << 16) | // Red
            ((uint32_t)ptr[1] << 8)  | // Green
            ((uint32_t)ptr[2]);        // Blue

        // Store reconstructed pixel
        frame_buffer[idx] = pixel;

        // Annotate the processing time consumed by handling the write 
        delay += sc_time(50, SC_NS);

        // Flag successful transport
        trans.set_response_status(TLM_OK_RESPONSE);
    }

    /**
     * @brief Dumps the local frame buffer to a PPM image format.
     * @param filename Path/name of the generated image file.
     */
    void export_ppm(const std::string & filename) {
        std::ofstream file(filename, std::ios::binary);

        // Standard PPM P6 header definition
        file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

        // Dump pixels natively by extracting RGB components
        for (auto& p : frame_buffer) {
            unsigned char r = (p >> 16) & 0xFF;
            unsigned char g = (p >> 8) & 0xFF;
            unsigned char b = (p >> 0) & 0xFF;

            file.write((char*)&r, 1);
            file.write((char*)&g, 1);
            file.write((char*)&b, 1);
        }

        file.close();
        std::cout << "[" << name() << "] Saved: " << filename << std::endl;
    }
};

#endif