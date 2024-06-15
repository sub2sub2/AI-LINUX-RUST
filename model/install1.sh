cd src1
mkdir -p build && cd build
cmake ..
cmake --build .



cp IrisModel /usr/local/bin
cp ../../data/org.mca.Model1.service /usr/share/dbus-1/services