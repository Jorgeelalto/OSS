g++ sync.cpp --std=c++11 -Wall -pthread -Ihadoop/include/ -Lhadoop/lib/native/ -o sync -lhadoop -lhdfs
echo ""
jps
echo ""
# You will have to run these lines by hand before using hadoop
#export LD_LIBRARY_PATH=/home/jmarcos/Documentos/Databases/hadoop/hadoop/lib/native
#export CLASSPATH=$(hadoop classpath)
#export HADOOP_CLASSPATH=$(hadoop classpath)
