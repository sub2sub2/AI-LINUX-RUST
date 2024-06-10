#include "Model.h"
#include <iostream>
using namespace AIModel::Model;

ModelInstance::ModelInstance(const std::string &modelPath)
{
    openlog("AI MODEL", LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Model Inatance is created");
    mModelPath = modelPath;
    mSession = new Ort::Session(mEnv, mModelPath.c_str(), mSessionOptions);
    Ort::AllocatedStringPtr pstrOutputName = mSession->GetOutputNameAllocated(0, mAllocators.operator OrtAllocator * ());

    if(!pstrOutputName) {
        syslog(LOG_INFO, "Fail to get output name");
    }

    mOutputName = strdup(pstrOutputName.get());
}

void ModelInstance::set_input_size(const std::vector<int64_t> &inputShape)
{
    if (mSession == nullptr)
        return;
    mInputShape = inputShape;
    size_t size = 0;
    for (auto t: mInputShape)
        size = size + t;
    mInputSize = size;
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

void ModelInstance::set_input_name(std::vector<const char*> &inputName)
{
    mInputName = inputName;
}

void ModelInstance::set_output_name()
{
    Ort::AllocatedStringPtr pstrOutputName = mSession->GetOutputNameAllocated(0, mAllocators.operator OrtAllocator * ());
    mOutputName = strdup(pstrOutputName.get());
}

std::string ModelInstance::infernce(std::vector<float> &inputData)
{
    if (inputData.size() == 0)
        return "";

    // memcpy(mInputTensor, &inputData, sizeof(float) * mInputSize);
    mInputTensor = inputData.data();
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    
    auto inputValue = Ort::Value::CreateTensor(memory_info,
        mInputTensor, mInputSize, mInputShape.data(), mInputShape.size());
    
    auto outputValue = Ort::Value::CreateTensor(memory_info,
        mOutputTensor, mOutputSize, mOutputShape.data(), mOutputShape.size());
  
    auto outputTensor = mSession->Run(Ort::RunOptions{nullptr}, mInputName.data(), &inputValue, 1, &mOutputName, 1);

    std::string result = "";
    for (const auto &output : outputTensor) {
        auto tensor_data = static_cast<const float*>(output.GetTensorRawData());
        size_t tensor_size = output.GetTensorTypeAndShapeInfo().GetElementCount();
        for (size_t i = 0; i < tensor_size; ++i) {
            result = std::to_string(tensor_data[i]);
        }
    }

    return result;
}