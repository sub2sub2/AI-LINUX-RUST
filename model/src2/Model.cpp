#include "Model.h"
#include <iostream>
#include <algorithm>
#include <vector>
using namespace AIModel::Model;

ModelInstance::ModelInstance(const std::string &modelPath)
{
    openlog("AI MODEL", LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Model Inatance is created");
    mModelPath = modelPath;
    mSession = new Ort::Session(mEnv, mModelPath.c_str(), mSessionOptions);
    set_output_name();
    set_input_name();
}

void ModelInstance::set_input_size(const std::vector<int64_t> &inputShape)
{
    if (mSession == nullptr)
        return;
    mInputShape = inputShape;
    size_t size = 1;
    for (auto t: mInputShape) 
        size = size * t;
    mInputSize = size;
    std::cout << "size: " << mInputSize << std::endl;
    mInputTensor = reinterpret_cast<float*>(mAllocators.Alloc(sizeof(float) * mInputSize));
}

void ModelInstance::set_output_size(const std::vector<int64_t> &outputShape)
{
    if (mSession == nullptr)
        return;
    mOutputShape = outputShape;
    size_t size = 0;
    for (auto t: mOutputShape)
        size = size + t;
    mOutputSize = size;
    mOutputTensor = reinterpret_cast<float*>(mAllocators.Alloc(sizeof(float) * mOutputSize));

}

void ModelInstance::set_input_name()
{
    Ort::AllocatedStringPtr pstrInputName = mSession->GetInputNameAllocated(0, mAllocators.operator OrtAllocator * ());
    mInputName = strdup(pstrInputName.get());
    std::cout << "InputName: " << mInputName << std::endl;
}

void ModelInstance::set_output_name()
{
    Ort::AllocatedStringPtr pstrOutputName = mSession->GetOutputNameAllocated(0, mAllocators.operator OrtAllocator * ());
    mOutputName = strdup(pstrOutputName.get());
    std::cout << "OutputName: " << mOutputName << std::endl;

}

std::string ModelInstance::infernce(std::vector<float> &inputData)
{
    if (inputData.size() == 0)
        return "";

    mInputTensor = inputData.data();

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::cout << "size: " << mInputSize << " " << mInputShape.size() <<  std::endl;

    auto inputValue = Ort::Value::CreateTensor(memory_info,
        mInputTensor, mInputSize, mInputShape.data(), mInputShape.size());

    auto outputTensor = mSession->Run(Ort::RunOptions{nullptr}, &mInputName, &inputValue, 1, &mOutputName, 1);

    float* output_data = outputTensor.front().GetTensorMutableData<float>();
    std::vector<int64_t> output_shape = outputTensor.front().GetTensorTypeAndShapeInfo().GetShape();

    // 결과 출력
    std::cout << "Output shape: ";
    for (auto dim : output_shape) {
        std::cout << dim << " ";
    }
    std::cout << std::endl;

    std::cout << "Output values: " ;
    for (int i = 0; i < output_shape[1]; ++i) {
        std::cout << output_data[i] << " ";
    }
    
    std::cout << std::endl;

    int batch_size_out = output_shape[0];
    int num_classes = output_shape[1];

    for (int64_t i = 0; i < batch_size_out; ++i) {
        auto max_it = std::max_element(output_data + i * num_classes, output_data + (i + 1) * num_classes);
        int max_index = std::distance(output_data + i * num_classes, max_it);
        float max_value = *max_it;
        std::cout << "Sample " << i << ": Class = " << max_index << ", Probability = " << max_value << std::endl;
    }

    std::cout << std::endl;
    return "";
}