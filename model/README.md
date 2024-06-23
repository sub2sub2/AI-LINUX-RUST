How to build

```
Precondition
1. onnx c++: https://onnxruntime.ai/docs/install/ (refer. Training Phase - On-Device Training)


cmake build
$ mkdir build & cd build
$ cmake ..
$ cmake --build .

```

1. onnx c++
```
// To train a model and convert it to onnx
$python3 train_model_onnx.py // train simple model and produce a weight file--> net.pt
$python3 convert_to_onnx.py // convert net.pt weight to net.onnx (.pt to .onnx)

// Now we have iris.onnx in `./weights/iris.onnx`.
```

2. Install AI model
```
sudo sh init.sh
sudo sh intall1.sh
```

