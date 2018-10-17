# Hiredis test
echo "Compiling hiredis_test"
g++ -c -Wall -std=c++11 hiredis_test.cpp -Ihiredis/
g++ hiredis_test.o -Lhiredis/ -lhiredis -pthread -o hiredis_test
echo "Linking hiredis_test"
# Synchronous, verbose
echo "Compiling sync.cpp"
g++ -Wall -pthread -std=c++11 sync.cpp  -lcpp_redis -ltacopie  -o sync -Icpp_redis/includes/ -Icpp_redis/tacopie/includes/ -Lcpp_redis/build/lib -lpthread
# Asynchronous, verbose
echo "Compiling async.cpp"
g++ -Wall -pthread -std=c++11 async.cpp  -lcpp_redis -ltacopie  -o async -Icpp_redis/includes/ -Icpp_redis/tacopie/includes/ -Lcpp_redis/build/lib -lpthread
# Asynchronous, CSV version
echo "Compiling async_m.cpp"
g++ -Wall -pthread -std=c++11 async_m.cpp  -lcpp_redis -ltacopie  -o async_m -Icpp_redis/includes/ -Icpp_redis/tacopie/includes/ -Lcpp_redis/build/lib -lpthread
# Synchronous, CSV version
echo "Compiling sync_m.cpp"
g++ -Wall -pthread -std=c++11 sync_m.cpp  -lcpp_redis -ltacopie  -o sync_m -Icpp_redis/includes/ -Icpp_redis/tacopie/includes/ -Lcpp_redis/build/lib -lpthread
# Test data
echo "Compiling testdata.cpp"
g++ -Wall -pthread -std=c++11 testdata.cpp  -lcpp_redis -ltacopie  -o testdata -Icpp_redis/includes/ -Icpp_redis/tacopie/includes/ -Lcpp_redis/build/lib -lpthread
