mkdir -p build && cd build
cmake ..
cmake --build .



sudo cp model_process /usr/local/bin
sudo cp ../data/org.mca.Model.service /usr/share/dbus-1/services