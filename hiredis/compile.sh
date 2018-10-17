# Hiredis test
echo "Compiling hiredis_test"
g++ -c -Wall -std=c++11 sync.cpp -Ihiredis/
g++ sync.o -Lhiredis/ -lhiredis -pthread -o hiredis_sync
echo "Linking hiredis_test"
# Synchronous, verbose
echo "Compiling sync.cpp"
#g++ -Wall -pthread -std=c++11 sync.cpp  -lcpp_redis -ltacopie  -o sync -Icpp_redis/includes/ -Icpp_redis/tacopie/includes/ -Lcpp_redis/build/lib -lpthread
