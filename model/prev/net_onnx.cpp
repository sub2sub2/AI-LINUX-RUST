#include <iostream>
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>

int main() {
  // Initialize the ONNX Runtime environment
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

  // Configure the session options
  Ort::SessionOptions session_options;
  // session_options.SetIntraOpNumThreads(1);

  // Load the model from the ONNX file
  Ort::Session session(env, "/home/hyunsub/workspace/hyunsub/AI-LINUX-RUST/model/weights/iris.onnx", session_options);

  // Get the input and output names
  Ort::AllocatorWithDefaultOptions allocator;
  std::vector<const char*> input_names = {"input"};
  Ort::AllocatedStringPtr pstrOutputName = session.GetOutputNameAllocated(0, allocator.operator OrtAllocator * ());
  auto output_names = pstrOutputName.get(); // get output name

  // Allocate memory for the input and output tensors
  std::vector<int64_t> input_shape = {1, 4};
  size_t input_size = 1 * 4;
  float* input_tensor = reinterpret_cast<float*>(allocator.Alloc(sizeof(float) * input_size));
  std::vector<int64_t> output_shape = {1, 1};
  size_t output_size = 1 * 1;
  float* output_tensor = reinterpret_cast<float*>(allocator.Alloc(sizeof(float) * output_size));

  // Initialize input data (replace this with actual input data)
  std::vector<float> input_data = {{1.0, 2.0, 3.0, 100.0}};
  // Initialize input data with actual values
  // Ort::Value input_tensor = Ort::Value::CreateTensor<float>(Ort::AllocatorWithDefaultOptions(), input_data.data(), input_data.size(), input_shape.data(), input_shape.size());

  // Copy the input data to the input tensor
  memcpy(input_tensor, &input_data, sizeof(float) * input_size);

  // Wrap the input and output tensors in Ort::Value objects
  Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  Ort::Value input_value = Ort::Value::CreateTensor(memory_info, input_tensor, input_size, input_shape.data(), input_shape.size());
  Ort::Value output_value = Ort::Value::CreateTensor(memory_info, output_tensor, output_size, output_shape.data(), output_shape.size());

  // Run the model and get the output
  auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names.data(), &input_value, 1, &output_names, 1);

  // Print the output
  std::cout << "Output: " << std::endl;
  for (const auto& output_tensor : output_tensors) {
      auto tensor_data = static_cast<const float*>(output_tensor.GetTensorRawData());
      size_t tensor_size = output_tensor.GetTensorTypeAndShapeInfo().GetElementCount();
      for (size_t i = 0; i < tensor_size; ++i) {
          std::cout << tensor_data[i] << " ";
      }
  }
  std::cout << std::endl;

  // Free the memory
  allocator.Free(input_tensor);
  allocator.Free(output_tensor);

  return 0;
}
