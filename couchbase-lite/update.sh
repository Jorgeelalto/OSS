# Clone and compile couchbase lite
git clone https://github.com/couchbase/couchbase-lite-core.git
cd couchbase-lite-core
git submodule update --init --recursive
cd build_cmake/scripts
./build_unix.sh
