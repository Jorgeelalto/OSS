# First we run the server
./redis/src/redis-server &
# We save its PID
serverpid=$!
# And sleep for a bit
sleep 1s

# Now we run the client, in monitor mode if specified
echo $1
if [ "$1" = "-m" ]; then
	echo "Running redis-cli in monitor mode. You will need to close it with Ctrl+C"
	./redis/src/redis-cli monitor
else
	echo "Running redis-cli in regular mode. Type \"exit\" to close it"
	./redis/src/redis-cli
fi

# When the previous process exits we
# will need to shut down the server
kill -s SIGTERM $serverpid
# Sleep for a bit
sleep 1s
# And exit
