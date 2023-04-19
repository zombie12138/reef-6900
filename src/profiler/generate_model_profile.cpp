#include "reef/profiler/model_profiler.h"
#include "reef/util/common.h"
#include <fstream>
#include <chrono>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " ../resource/resnet18"
             << std::endl;
        return 1;
    }
    std::string model_name(argv[1], std::string(argv[1]).find_last_of('/') + 1,
             strlen(argv[1]));
    std::cout << "Load " << argv[1] << " " << model_name << std::endl;

    reef::profiler::ModelProfiler pro(argv[1],
        model_name);
    
    std::ifstream df("data.txt");
    std::vector<float> input(3 * 224 * 224);
    int n;
    df >> n;
    for (size_t i = 0; i < n; i++)
        input[i] = 10.;
        // df >> input[i];
    
    df.close();
    pro.set_input((void*)input.data(), input.size() * sizeof(float));

    auto start = std::chrono::system_clock::now();

    pro.profile();
    
    // do something...
    auto end   = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout <<  "takes " 
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den 
        << " sec" << std::endl;

    std::vector<float> out;
    std::cout << "get result" << std::endl;
    pro.get_output(out);
    size_t max_index = 0;
    
    long double result = 1.;
    for (int i = 0; i < out.size(); ++i) {
        // std::cout << out[i] << std::endl;
        // if (out[max_index] < out[i]) {
        //     max_index = i;
        // }
        if (out[i] < 0.1) {
            out[i] += (int)out[i] + 1.;
        }
        result /= out[i];
        if (result > 100000) {
            result /= 1000;
        }
    }
    std::cout << "Result: " << result << " " << out[0] << std::endl;
    std::cout << "Max Value is result[" << max_index << "] = " 
            << out[max_index] << std::endl;
    return 0;
}
