cd src1
mkdir -p build && cd build
cmake ..
cmake --build .



cp MnistModel /usr/local/bin
cp ../../data/org.mca.Model.service /usr/share/dbus-1/services