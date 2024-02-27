# AI-LINUX-RUST
This project consists of `AI App`, `Model Control Agent(MCA)`, and `AI model`.

## AI App
`AI App` is an app that needs AI inference to anaylze any type of data supported by `MCA`. The AI app can request AI model inference to `MCA` and get a result of the inference.

## Model Control Agent(MCA)
`Model Control Agent (MCA)` is an agent handles AI model reference requested by `AI App`. The agent has predefined AI model information, so it queues the requests coming from AI apps and process them by requesting to the `AI model`. Also, `MCA` has control logic for the inference and each model.

## AI Models
`AI models` are AI models handle inference requested by `MCA`. The models will be launched on demend way.