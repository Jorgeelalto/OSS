echo "Cloning the latest Redis source code"
rm -rf redis/
git clone https://github.com/antirez/redis.git
echo "Cloning the latest cpp_redis source code"
rm -rf cpp_redis/
git clone https://github.com/Cylix/cpp_redis.git

cd redis
make

cd ..
cd cpp_redis
echo "Cloning the latest tacopie source code"
git clone https://github.com/Cylix/tacopie.git 
mkdir build
cd build
cmake ..
make
