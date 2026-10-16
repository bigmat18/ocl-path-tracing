#include "main.hpp"
#include <core/logging.hpp>

void run() {
    core::logging::info("OpenCL Path Tracing initialized with C++26");
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    run();
    return 0;
}
