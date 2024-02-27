import torch
from train_model_onnx import Net
# Save the trained model

net = Net().cuda()

net.load_state_dict(torch.load('./net.pt'))
net.eval()

x = torch.randn((1, 1, 28, 28)).cuda()

torch.onnx.export(net,
                  x,
                  "net.onnx",
                  export_params = True,
                  opset_version = 11,
                  do_constant_folding = True,
                  input_names=['input'],
                  output_names=['output'])