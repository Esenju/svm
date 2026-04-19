#define WIN32_LEAN_AND_MEAN

// Include config.h FIRST so MODE is defined before any other header uses it
#include "config.h"

#include <systemc>
#include <iostream>
#include "producer.h"
#include "consumer.h"
#include "router.h"

using namespace sc_core;

/**
 * @brief Top-level SystemC main function.
 * Initializes the SoC components, binds their TLM sockets, and starts the simulation.
 */
int sc_main(int argc, char* argv[]) {

#if MODE == 0
    std::cout << "Mode: Part A (Direct Producer to Consumer)" << std::endl;
#elif MODE == 1
    std::cout << "Mode: Part B (Producer -> Router -> 1 Consumer)" << std::endl;
#else
    std::cout << "Mode: Advanced (Producer -> Router -> 2 Consumers)" << std::endl;
#endif

    // Instantiate the producer (Test Pattern Generator)
    Producer producer("producer");

#if MODE == 0
    // Instantiate a single consumer and bind directly to producer
    Consumer consumer("consumer");
    producer.initiator_socket.bind(consumer.target_socket);

#elif MODE == 1
    // Instantiate router and consumer. Bind Producer -> Router -> Consumer
    Consumer consumer("consumer");
    Router router("router");

    producer.initiator_socket.bind(router.target_socket);
    router.initiator_socket1.bind(consumer.target_socket);

#else
    // Instantiate router and two consumers. Bind Producer -> Router -> {Consumer1, Consumer2}
    Consumer consumer1("consumer1");
    Consumer consumer2("consumer2");
    Router router("router");

    producer.initiator_socket.bind(router.target_socket);

    router.initiator_socket1.bind(consumer1.target_socket);
    router.initiator_socket2.bind(consumer2.target_socket);
#endif

    // Start simulation cycle
    sc_start();
    std::cout << "Simulation completed at: " << sc_time_stamp() << std::endl;

    // Export the finalized image buffer(s) to PPM
#if MODE == 0
    consumer.export_ppm("output_frame.ppm"); // Fixed to match deliverable name
#elif MODE == 1
    consumer.export_ppm("output_frame.ppm"); // Fixed to match deliverable name
#else
    consumer1.export_ppm("output_frame_1.ppm");
    consumer2.export_ppm("output_frame_2.ppm");
#endif

    return 0;
}