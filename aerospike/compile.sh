# Synchronous, CSV version
echo "Compiling sync_m.cpp"
g++ --std=c++11 sync_m.cpp -lpthread -laerospike -Iaerospike-client-c/target/Linux-x86_64/include -Laerospike-client-c/target/Linux-x86_64/lib
#g++ -Wall -pthread --std=c++11 sync_m.cpp -laerospike -o sync_m -Iaerospike-client-c/target/Linux-x86_64/include -Laerospike-client-c/target/Linux-x86_64/lib -lpthread
