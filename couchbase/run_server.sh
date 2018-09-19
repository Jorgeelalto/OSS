if [ "$1" = "-s" ]; then
	echo "Stopping Couchbase Docker container"
	docker stop couchbase-server
else
	echo "Running Couchbase server from Docker container. Run this script with"
	echo "the argument -s to stop it."
	docker run -d --name=couchbase-server -p 8091-8094:8091-8094 -p 11210:11210 couchbase
	docker start couchbase-server
fi
