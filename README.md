# AI-LINUX-RUST
This project consists of `AI App`, `Model Control Agent(MCA)`, and `AI model`.

## Docker environment(for Mac, not suppored from 2024.06.18)
1. Intall and set docker in Mac (refer. https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry#authenticating-to-the-container-registry)

2. Pull and run the docker image
```
$docker pull ghcr.io/sub2sub2/ai_rust:1.0.0
$docker run -it --name AI_RUST ghcr.io/sub2sub2/ai_rust:1.0.0 /bash/bin
```
3. Access into the running container and clone the git
```
$docket attach AI_RUST
--- in docker shell ---
clone the git repo and test
```

## AI App
`AI App` is an app that needs AI inference to anaylze any type of data supported by `MCA`. The AI app can request AI model inference to `MCA` and get a result of the inference. App developers can access to `MCA` through on gRPC and request an inference to `MCA`.

## Model Control Agent(MCA)
`Model Control Agent (MCA)` is an agent that handles AI model inference requested by `AI App`. The agent has predefined AI model information, so it connects `AI App` to `AI Model` by `D-bus` which is the linux IPC system. Also, `MCA` has control logic for the inference and each model. Server maintainers, app developers, and model engineers can register their own model easily with `MCA`.

## AI Models
`AI Models` are AI models handle inference requested by `MCA`. The models will be launched on demend way. `AI Models` may get file path that indicates the coming file from `AI app` or actual data with json string form. You need to train a model and convert it to onnx model.
