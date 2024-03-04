import torch
from train_model_onnx import Net
# Save the trained model

device = "cuda" if torch.cuda.is_available() else "cpu"

net = Net().cuda()

net.load_state_dict(torch.load('./net.pt'))
net.to(device)
net.eval()

x = torch.randn((1, 1, 28, 28)).to(device)

torch.onnx.export(net,
                  x,
                  "net.onnx",
                  export_params = True,
                  opset_version = 11,
                  do_constant_folding = True,
                  input_names=['input'],
                  output_names=['output'])