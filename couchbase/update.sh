# Delete previous installation
rm -rf libcouchbase
rm -rf libevent
# Clone, compile and install libevent
#git clone --single-branch -b 2.0.23-stable-rc https://github.com/libevent/libevent.git
git clone https://github.com/libevent/libevent.git
cd libevent
mkdir build
cd build
cmake ..
make
sudo make install
cd ..
cd ..
# Clone and compile libcouchbase
git clone https://github.com/couchbase/libcouchbase.git
cd libcouchbase
mkdir build
cd build
../cmake/configure
make
