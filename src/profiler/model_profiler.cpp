#include "reef/profiler/model_profiler.h"


namespace reef {
namespace profiler{

ModelProfiler::ModelProfiler(std::string model_dir, std::string model_name) :
    model_dir_(model_dir),
    model_name_(model_name)
{
    ASSERT_GPU_ERROR(GPUStreamCreate(&stream_));
    std::string raw_co_path = model_dir + "/" + model_name + ".raw.co";
    std::string param_path = model_dir + "/" + model_name + ".param";
    std::string json_path = model_dir + "/" + model_name + ".json";

    executor::GPUModule_t mod;
    ASSERT_GPU_ERROR(GPUModuleLoad(&mod, raw_co_path.c_str()));

    exe_.load_model_from_GPU_module(json_path.c_str(), mod);
    exe_.load_param_from_file(param_path.c_str());

}

ModelProfiler::~ModelProfiler() {
}

Status ModelProfiler::set_input(const void* data, size_t len) {
    // copy
    exe_.set_input(std::string("data"), data, len);
    return Status::Succ;
}

Status ModelProfiler::get_output(std::vector<float>& out) {
    exe_.get_output(out);
    return Status::Succ;
}

Status ModelProfiler::profile(int repead) {
    for (int i = 0; i < exe_.num_kernels(); i++) {
        assert(exe_.execute_kernel(i, stream_) == Status::Succ);
        // if (!rt_queue->task_queue.empty()) {
        //     if (mode == ScheduleMode::REEF || mode == ScheduleMode::Reset) {
        //         // LOG(INFO) << "preempt during launch";
        //         return; // preempt
        //     }   
        // }
        // std::string s;
        // exe_.get_kernel_name(i, s);
        // std::cout << s << std::endl;
    }
    return Status::Succ;
}


}   // namespace profiler
}   // namespace reef