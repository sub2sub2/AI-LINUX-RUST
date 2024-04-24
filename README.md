# AI-LINUX-RUST
This project consists of `AI App`, `Model Control Agent(MCA)`, and `AI model`.

## Docker environment(Only support Mac M serises)
1. Intall and set docker in Mac
```
$sudo wget -qO- http://get.docker.com/ | sh
```
2. Pull and run the docker image
```
$docker pull ghcr.io/sub2sub2/ai_rust:1.0.0
$docker run -it --name AI_RUST ghcr.io/sub2sub2/ai_rust:1.0.0 bin
```
3. Access into the running container and clone the git
```
$docket attach AI_RUST
--- in docker shell ---
clone the git repo and test
```

## AI App
`AI App` is an app that needs AI inference to anaylze any type of data supported by `MCA`. The AI app can request AI model inference to `MCA` and get a result of the inference.

## Model Control Agent(MCA)
`Model Control Agent (MCA)` is an agent handles AI model reference requested by `AI App`. The agent has predefined AI model information, so it queues the requests coming from AI apps and process them by requesting to the `AI model`. Also, `MCA` has control logic for the inference and each model.

## AI Models
`AI models` are AI models handle inference requested by `MCA`. The models will be launched on demend way.
