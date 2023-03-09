#include "reef/server/server.h"

int main(int argc, char** argv) {
    reef::server::REEFServer server(DEFAULT_REEF_ADDR);
    std::cout << "test" << std::endl;
    server.run();
    server.wait();
    return 0;
}