#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <libmemcached/memcached.h>
#include <mutex>

std::mutex m;

//Function to insert items to the cache
int insert (memcached_st *client, std::atomic<unsigned long>& ctr, int n) {

	int i;
	int expire = 9999;
	uint32_t flags = 0;
	//Define the key and data storage variables
	std::string key;
	size_t key_l;
	std::string data;
	size_t data_l;

	//While there are still elements to be inserted
	for (i = 0; i < n; i++) {
		//Get the new key and data
		key = std::to_string(i);
		data = std::to_string(i + 1);
		key_l = key.length();
		data_l = data.length();
		//Insert it into the cache
		m.lock();
		memcached_set(client, key.c_str(), key_l, data.c_str(), data_l, expire, flags);
		m.unlock();
		//Atomically add one to the petition ctr
		ctr.fetch_add(1);
	}
	return 0;
}


//Function to retrieve data from the cache
int read (memcached_st *client, std::atomic<unsigned long>& ctr, int n) {
	
	int i;
	uint32_t flags;
	std::string key;
	size_t key_l;

	//While there are still elements to be read
	for (i = 0; i < n; i++) {
		//Prepare the key
		key = std::to_string(i);
		//Make a petition, without saving the data, length of the retrieved data nor the
		//server return
		m.lock();
		memcached_get(client, key.c_str(), key_l, NULL, &flags, NULL);
		m.unlock();
		//Atomically add one to the petition ctr
		ctr.fetch_add(1);
	}
	return 0;
}


int main (int argc, const char ** argv) {

	//Argument variables
	int n_threads, n_insertions, n_reads;

	if (argc == 4) {
		n_threads = std::stoi(argv[1]);
		n_insertions = std::stoi(argv[2]);
		n_reads = std::stoi(argv[3]);
	}

	else {
		n_threads = 16;
		n_insertions = 25000;
		n_reads = 75000;
	}
	
	//Create some stuff
	memcached_st *client = NULL;
	memcached_server_st *server = NULL;
	memcached_return cache_return;
	//Initialize the client and add the server to the server list
	client = memcached_create(NULL);
	server = memcached_server_list_append(server, "127.0.0.1", 11211, &cache_return);
	cache_return = memcached_server_push(client, server);
	//If the connection is not successful
	if (MEMCACHED_SUCCESS != cache_return) {
		std::cout << "Memcached server push failed, cache return: " << cache_return << std::endl;
		return -1;
	}

	//Defining the atomic double used as a petition ctr
	std::atomic<unsigned long> ctr(0);
	//Array of insertion threads
	std::thread ti[n_threads];
	//Array of read threads
	std::thread tr[n_threads];
	//Auxiliary variables for the next loops
	int i;
	//Get the start time
	auto start = std::chrono::system_clock::now();

	//Create as many insertion threads as specified in the arguments
	if (n_insertions > 0) {
		for (i = 0; i < n_threads; i++) {
			ti[i] = std::thread(insert, client, std::ref(ctr), n_insertions);
			std::cout << "Created the thread " << i << "I with " << n_insertions << " pending insertions" << std::endl;
		}
	}

	//Create as many read threads as specified in the arguments
	if (n_reads > 0) {
		for (i = 0; i < n_threads; i++) {
			tr[i] = std::thread(read, client, std::ref(ctr), n_reads);
			std::cout << "Created the thread " << i << "R with " << n_reads << " pending reads" << std::endl;
		}
	}

	//Join all the threads
	for (i = 0; i < n_threads; i++) {
		ti[i].join();
		tr[i].join();
		std::cout << "Joined " << i << "I and " << i << "R" << std::endl;
	}

	//Get the end time
	auto end = std::chrono::system_clock::now();
	//and calculate the time difference (execution time)
	std::chrono::duration<double> difference = end - start;
	
	//Print the statistics
	std::cout << "Total number of requests made to the cache: " << ctr << std::endl;
	std::cout << "Total time elapsed: " << difference.count() << std::endl;
	std::cout << "Petitions per second: " << (long int) (ctr / difference.count()) << std::endl;
	//And exit
	return 0;
}
