/usr/local/hadoop/sbin/stop-dfs.sh
rm -rf /usr/local/hadoop_store/hdfs/datanode/*
rm -rf /usr/local/hadoop_store/hdfs/namenode/*
rm -rf /app/hadoop/tmp/*
rm -f /home/jmarcos/Documentos/Databases/hs*
hdfs datanode -format
hdfs namenode -format

if [ $1 == "-r" ]; then
	/usr/local/hadoop/sbin/start-dfs.sh
else
	echo "Use the argument -r to restart the server too"
