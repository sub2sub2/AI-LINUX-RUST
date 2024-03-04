How to build

```
Precondition
1. pytorch c++ frontend: https://pytorch.org/cppdocs/installing.html
2. onnx c++: https://onnxruntime.ai/docs/install/ (refer. Training Phase - On-Device Training)


cmake build
$ mkdir build & cd build
$ cmake -DCMAKE_PREFIX_PATH=/home/hyunsub/workspace/libtorch .. (use your path for libtorch)
$ cmake --build .

```

1. onnx c++
```
$python3 train_model_onnx.py // train simple model and produce a weight file--> net.pt
$python3 convert_to_onnx.py // convert net.pt weight to net.onnx (.pt to .onnx)
$cd build
$./net_onnx // load the net.onnx and inference sample data

```

2. pytorch c++ library
```
$cd build
$./dcgan
```

