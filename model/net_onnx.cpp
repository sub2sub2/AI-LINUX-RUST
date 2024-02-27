#include <iostream>
#include <onnxruntime_cxx_api.h>

int main() {
  // Initialize the ONNX Runtime environment
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

  // Configure the session options
  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(1);

  // Load the model from the ONNX file
  Ort::Session session(env, "../net.onnx", session_options);

  // Get the input and output names
  std::vector<const char*> input_names = {"input"};
  std::vector<const char*> output_names = {"output"};

  // Allocate memory for the input and output tensors
  Ort::AllocatorWithDefaultOptions allocator;
  std::vector<int64_t> input_shape = {1, 1, 28, 28};
  size_t input_size = 1 * 1 * 28 * 28;
  float* input_tensor = reinterpret_cast<float*>(allocator.Alloc(sizeof(float) * input_size));
  std::vector<int64_t> output_shape = {1, 10};
  size_t output_size = 1 * 10;
  float* output_tensor = reinterpret_cast<float*>(allocator.Alloc(sizeof(float) * output_size));

  // Initialize input data (replace this with actual input data)
  float* input_data = new float[input_size];
  // Initialize input data with actual values

  // Copy the input data to the input tensor
  memcpy(input_tensor, input_data, sizeof(float) * input_size);

  // Wrap the input and output tensors in Ort::Value objects
  Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  Ort::Value input_value = Ort::Value::CreateTensor(memory_info, input_tensor, input_size, input_shape.data(), input_shape.size());
  Ort::Value output_value = Ort::Value::CreateTensor(memory_info, output_tensor, output_size, output_shape.data(), output_shape.size());

  // Run the model and get the output
  auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names.data(), &input_value, 1, output_names.data(), 1);

  // Print the output
  std::cout << "Output: " << std::endl;
  for (int i = 0; i < output_size; i++) {
    std::cout << output_tensor[i] << " ";
  }
  std::cout << std::endl;

  // Free the memory
  allocator.Free(input_tensor);
  allocator.Free(output_tensor);
  delete[] input_data;

  return 0;
}
