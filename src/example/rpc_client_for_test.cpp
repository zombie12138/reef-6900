#include "reef/client/client.h"
#include <thread>
#include <iostream>
#include <fstream>

struct ClientConfig {
    double average_latency;
    std::string model_dir;
    std::string model_name;
    int client_id;
    int repead;
    int interval;   // ms
    bool is_real_time;
};

void *reef_client(void *data) {
    ClientConfig *cc = reinterpret_cast<ClientConfig *>(data);
    std::cout << "loading '" << cc->model_name << "' from " << "'"<< cc->model_dir << "'\n";

    reef::client::REEFClient client(DEFAULT_REEF_ADDR);
    ASSERT(client.init(cc->is_real_time)); // whether this client send real-time requests?
    auto model = client.load_model(cc->model_dir, cc->model_name);
    ASSERT(model.get() != nullptr);

    std::ofstream out(std::string("log/") + std::to_string(cc->client_id) + std::string(1, '0' + cc->is_real_time) + cc->model_name + std::string(".log"));

    for (int i = 0; i < cc->repead; ++i) {
        auto task = model->infer(); // submit an inference request
        out << std::chrono::duration_cast<std::chrono::microseconds>(task.finish - task.submit).count() / 1000.0 << "\n";
        if (i >= 10 && i + 20 < cc->repead) cc->average_latency += std::chrono::duration_cast<std::chrono::microseconds>(task.finish - task.submit).count() / 1000.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(cc->interval));
    }
    cc->average_latency /= cc->repead - 30;
    std::cout << std::to_string(cc->client_id) << ": " << cc->model_name << " " << std::string(1, '0' + cc->is_real_time) << " " << cc->average_latency << std::endl;
    out.close();
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: " << std::string(argv[0]) << " model_dir model_name real_time sleep_time(ms) repead... \n";
        std::cerr << "Example: " << std::string(argv[0]) << " reef/resource/resnet18 resnet18 1 10 1000\n";
        return -1;
    }
    std::vector<ClientConfig> client_configs;
    
    for (int i = 5; i < argc; i += 5) {
        ClientConfig cc = {
            0.0,                            // averave latency
            std::string(argv[i - 4]),   // dir
            std::string(argv[i - 3]),   // name
            i / 5,                      // client_id
            std::atoi(argv[i]),     // repead
            std::atoi(argv[i - 1]), // interval
            static_cast<bool>(std::atoi(argv[i - 2]))            
        };
        client_configs.push_back(cc);
    }

    std::vector<pthread_t> pids(client_configs.size());
    for (int i = 0; i < pids.size(); ++i) {
        int ret = pthread_create(&pids[i], NULL, reef_client, &client_configs[i]);
        if (ret) {
            std::cout << "Error While Creating Thread" << std::endl;
            return ret;
        }
    }
    
    for (int i = 0; i < pids.size(); ++i) {
        pthread_join(pids[i], NULL);
    }


    // Get or set the input/output data.
    // auto input_blob = model->get_input_blob();
    // model->load_input();
    // auto output_blob = model->get_output_blob();
    // auto output = model->get_output();


    return 0;
}