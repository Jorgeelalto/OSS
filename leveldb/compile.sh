echo "Compiling"
g++ -c -Wall -std=c++11 sync.cpp -Ileveldb/include/ -Isnappy/ -Isnappy/build
g++ -c -Wall -std=c++11 async.cpp -Ileveldb/include/ -Isnappy/ -Isnappy/build
g++ -c -Wall -std=c++11 sync_batch.cpp -Ileveldb/include/ -Isnappy/ -Isnappy/build
g++ -c -Wall -std=c++11 async_batch.cpp -Ileveldb/include/ -Isnappy/ -Isnappy/build
g++ -c -Wall -std=c++11 sync_m.cpp -Ileveldb/include/ -Isnappy/ -Isnappy/build
g++ -c -Wall -std=c++11 async_m.cpp -Ileveldb/include/ -Isnappy/ -Isnappy/build


echo "Linking"
g++ sync.o -Lleveldb/build/ -Lsnappy-master/build/ -lleveldb -lsnappy -pthread -o sync
g++ async.o -Lleveldb/build/ -Lsnappy-master/build/ -lleveldb -lsnappy -pthread -o async
g++ sync_batch.o -Lleveldb/build/ -Lsnappy-master/build/ -lleveldb -lsnappy -pthread -o sync_batch
g++ async_batch.o -Lleveldb/build/ -Lsnappy-master/build/ -lleveldb -lsnappy -pthread -o async_batch
g++ sync_m.o -Lleveldb/build/ -Lsnappy-master/build/ -lleveldb -lsnappy -pthread -o sync_m
g++ async_m.o -Lleveldb/build/ -Lsnappy-master/build/ -lleveldb -lsnappy -pthread -o async_m



echo "Removing cache"
rm *.o
