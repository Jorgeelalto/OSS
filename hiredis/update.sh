echo "Cloning the latest Redis source code"
rm -rf redis/
git clone https://github.com/antirez/redis.git
echo "Cloning the latest hiredis source code"
rm -rf hiredis/
git clone https://github.com/redis/hiredis.git

cd redis
make

cd ..
cd hiredis
make
cd ..
