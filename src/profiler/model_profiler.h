#pragma once

#include <string>
#include "reef/executor/hip/hip_impl.h"
#include "reef/util/common.h"
#include "reef/executor/executor_base.h"

namespace reef {
namespace profiler{


class ModelProfiler {
public:
    ModelProfiler(std::string model_dir, std::string model_name);
    ~ModelProfiler();
    Status set_input(const void* data, size_t len);
    Status get_output(std::vector<float>& out);
    Status profile(int repead = 20);    // dump to json file
private:

private:
    std::string model_dir_;
    std::string model_name_;
    executor::GPUStream_t stream_;
    executor::ExecutorBase executor_;
    executor::ExecutorBase exe_;

    // std::mutex shm_mtx_;
    // std::shared_ptr<util::SharedMemory> input_;
    // std::shared_ptr<util::SharedMemory> output_;
};


}   // namespace profiler
}   // namespace reef