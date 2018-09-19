echo "Removing binaries and cache"
# memcached
cd memcached
rm main
cd ..
# couchbase
cd couchbase
rm main
cd ..
# leveldb
cd leveldb
rm main main.o
rm -rf testdb
cd ..
# redis
cd redis
rm main
rm -r redis/tests/tmp/*
rm dump.rdb
cd ..
#testfiles
cd testfiles
rm *K* *M*
cd ..
# Go back to manage the whole folder
cd ..
echo "Removing previous backup and moving the current one"
rm databases-prev.tar.gz
mv databases.tar.gz databases-prev.tar.gz
echo "Backing up the current state of the project"
tar -czf databases.tar.gz Databases/
