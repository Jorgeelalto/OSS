# Remove previous versions
rm -rf leveldb
rm -rf snappy-master

# Clone and compile LevelDB
git clone https://github.com/google/leveldb.git
cd leveldb
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
cd ..
cd ..

# Clone and compile
git clone https://github.com/google/snappy.git
cd snappy
mkdir build
cd build
cmake .. && make
