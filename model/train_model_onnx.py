import torch
import torch.nn as nn
import torch.optim as optim
import torchvision
import torchvision.transforms as transforms
from torch.utils.data import DataLoader

# Define the neural network architecture
class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.fc1 = nn.Linear(784, 64)
        self.fc2 = nn.Linear(64, 32)
        self.fc3 = nn.Linear(32, 10)
        self.dropout = nn.Dropout(0.5)

    def forward(self, x):
        x = torch.relu(self.fc1(x.view(-1, 784)))
        x = self.dropout(x)
        x = torch.relu(self.fc2(x))
        x = torch.log_softmax(self.fc3(x), dim=1)
        return x


if __name__ == '__main__':
    # Load the MNIST dataset
    transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize((0.5,), (0.5,))])
    train_dataset = torchvision.datasets.MNIST(root='./build/mnist', train=True, transform=transform, download=True)
    train_loader = DataLoader(train_dataset, batch_size=256, shuffle=True)

    # Initialize the neural network, loss function, and optimizer
    net = Net()
    criterion = nn.NLLLoss()
    optimizer = optim.SGD(net.parameters(), lr=0.01)
    net.train()
    net.cuda()
    # Train the neural network
    for epoch in range(1, 11):
        running_loss = 0.0
        for i, data in enumerate(train_loader, 0):
            inputs, labels = data
            optimizer.zero_grad()
            outputs = net(inputs.cuda())
            loss = criterion(outputs, labels.cuda())
            loss.backward()
            optimizer.step()
            running_loss += loss.item()
            if i % 100 == 99:    # Print every 100 mini-batches
                print(f'Epoch: {epoch}, Batch: {i+1}, Loss: {running_loss / 100}')
                running_loss = 0.0
    torch.save(net.state_dict(), './net.pt')


