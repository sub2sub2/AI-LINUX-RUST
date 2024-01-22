#How to build and test
1. build rust library. The lib is shared lib(.so) working with c/c++
```
In "./AI-LINUX_RUST/mca"

cargo build --release // This will build the lib file.
```

2. Produce c header file with `cbindgen` and move the header.
```
In "./AI-LINUX_RUST/mca"

cbindgen --config cbindgen.toml --crate mca --output target/api.h // This will create api.h file into `./target` dir

move ./target/api.h ./
```

3. Build test app and test
```
In "./AI-LINUX_RUST/mca"

g++ -o test test.cpp -I target -L target/release -lmca
./test
```

#Reference
1. Rust lib build
- https://doc.rust-lang.org/std/ffi/struct.CStr.html
- https://docs.rust-embedded.org/book/interoperability/rust-with-c.html
- https://dev.to/dandyvica/how-to-call-rust-functions-from-c-on-linux-h37

2. c header
- https://github.com/mozilla/cbindgen
- https://michael-f-bryan.github.io/rust-ffi-guide/cbindgen.html

