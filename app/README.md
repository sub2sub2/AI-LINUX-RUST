# app


## Installation

Install the shared libraray. 
After the installation, you will get 
- /usr/local/lib/grpc.so
- /usr/local/include/grpc.h

```bash
$ cd app

# at project/app,
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install
```

Compile via the CMakefile 
```bash
# at project/app,
$ cd app/build
$ cmake ..

# you can check myapp
$ ./myapp
```

## Example
