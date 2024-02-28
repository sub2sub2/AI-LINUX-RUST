# app









## Installation

To install gRPC, follow the steps below. Detailed information can be found in the gRPC C++ Quick Start Guide.
(Link: https://grpc.io/docs/languages/cpp/quickstart/)


```bash
# Install required dependencies
$ sudo apt install -y build-essential autoconf libtool pkg-config 

# Navigate to your project directory (replace AI-LINUX-RUST with your actual project directory)
$ cd AI-LINUX-RUST/

# Clone the gRPC repository
$ git clone --recurse-submodules -b v1.62.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc

# Install gRPC and Protocol Buffers
$ cd grpc
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
$ make -j 4
$ make install
$ popd

# Check the Protocol Buffers version
$ protoc --version




```

After downloading gRPC and Protocol Buffers, build the app using the following commands:


```bash
$ cd AI-LINUX-RUST/app

# Create a build directory and navigate into it
$ mkdir build && cd build

# Run CMake to configure the build
$ cmake ..

# Build the executable
$ make

# Check the generated executable in the "app/build" directory
$ ./MyExecutable

```

