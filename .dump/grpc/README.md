# Getting Started

## Proto

In order to communicate with gPRC,
a server and a client have to share same proto file,
which defines service and data types as the following:

```
syntax = "proto3";
package agent;

service IrisInference {
    rpc Inference (IrisInferenceRequest) returns (IrisInferenceResponse);
}

message IrisInferenceRequest {
    string id = 1;

    float sepal_length = 2;
    float sepal_width = 3;
    float petal_length = 4;
    float petal_width = 5;
}

message IrisInferenceResponse {
    string species = 1;
}

```



## 1. Run gRPC server
```bash
$ cd ~/rust_grpc_server
$ cargo run
```

## 2. Run gRPC client
```bash
$ cd ~/rust_grpc_client
$ cargo run

```
Then, the following message will be shown:
```
#0

Please input features for the model, 4 floats, space delimiter 
(wrong input: 2.3 4.5 6.7 8.9 ): 

```

You can put 4 floating numbers (corressponding to features of iris),
or just type the "enter" key (default: 2.3 4.5 6.7 8.9).







