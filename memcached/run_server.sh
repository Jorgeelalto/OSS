# First we run the server, in ports 4000 for TCP and 4000 for UDP, in daemon form
memcached -p 11211 -U 11211 -d
echo "Running memcached as a daemon. Press any key to stop"
#memcstat --servers=localhost 11211
perl memcache-top
# And exit
