# Removing previous installations of server and lib
rm -rf aerospike-server
rm -rf aerospike-client-c
# Clone both repos
git clone https://github.com/aerospike/aerospike-server.git
git clone https://github.com/aerospike/aerospike-client-c.git
# Enter and compile server
cd aerospike-server
git submodule update --init
make
make init
# Exit, and enter and compile c client library
cd ..
cd aerospike-client-c
git submodule update --init
make

# Some final info
echo "---------------------------------------------"
echo "To start the aerospike server type \"make start\""
echo "To stop it, \"make stop\""
echo "---------------------------------------------"