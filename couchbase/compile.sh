g++ -Wall -std=c++11 -pthread sync.cpp -lcouchbase -o sync -Ilibcouchbase/include -Ilibcouchbase/build/generated/ -Llibcouchbase/build/lib -lpthread
g++ -Wall -std=c++11 -pthread sync_m.cpp -lcouchbase -Llibcouchbase/build/lib -lpthread -o sync_m -Ilibcouchbase/include -Ilibcouchbase/build/generated/
echo "Adding the library to the path"
export LD_LIBRARY_PATH=/home/jmarcos/Documentos/Databases/couchbase/libcouchbase/build/lib