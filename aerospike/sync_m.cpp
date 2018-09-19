#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <aerospike/aerospike.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/as_error.h>
#include <aerospike/as_record.h>
#include <aerospike/as_status.h>
#include <string>
#include <fstream>


//Read a file and return it as an array of bytes
char* readFileBytes(std::string name) {
        //Open a file and get the fp
        std::fstream file(name);
        //Move the fp to the end of the file
        file.seekg(0, std::ios::end);
        //Get the size
        size_t len = file.tellg();
        //Create a char array with that length
        char *a = new char[len];
        //Go to the beginning of the file,
        file.seekg(0, std::ios::beg);
        //store "len" bytes to a,
        file.read(a, len);
        //close the file
        file.close();
        //and return it
        return a;
}

//Function to insert items to the database from an array
int insert (aerospike& as, as_error& err, std::atomic<unsigned long>& ctr, int n, char*& block) {
	int i;
	//Create the record object
	as_record rec;
	as_key key;
	//Creating a record with one bin
	as_record_inita(&rec, 1);
	//While there are still elements to be inserted
	for (i = 0; i < n; i++) {
		//Add the element to the record
		as_record_set_str(&rec, "block-bin", block);
		//Set the key
		as_key_init_str(&key, "test", "test-set", std::to_string(i).c_str());
		//Insert it into the database
		aerospike_key_put(&as, &err, NULL, &key, &rec);
		//Atomically add one to the petition counter
		ctr.fetch_add(1);
	}
	return 0;
}

int read (aerospike& as, as_error& err, std::atomic<unsigned long>& ctr, int n) {
	int i;
	//Create the record object pointer
	as_record* p_rec = NULL;
	char * value;
	as_key key;
	//Initialize the key
	as_key_init(&key, "test", "test-set", std::to_string(i).c_str());
	//While there are still elements to be read
	for (i = 0; i < n; i++) {
		//Get the pointer of the record
		aerospike_key_select(&as, &err, NULL, &key, select, &p_rec)
		//Get the value
		value = as_record_get_str(p_rec, bin);
		as_record_destroy(p_rec);
		//Atomically add one to the petition counter
		ctr.fetch_add(1);
	}
	return 0;
}


int main(int argc, char** argv) {


	//Print the help
	if (argc == 2) {
		std::string arg_1 = argv[1];
		if (arg_1.compare("-h") == 0) {
			std::cout << "Usage: ./async [#THREADS] [#INS PER ITHREAD] "
				<< "[#READS PER RTHREAD] [INSERTION SIZE] [K/M]" << std::endl;
			return 0;
		}
	}

	//Argument variables
	int n_threads, n_insertions, n_reads;
	//Other variables to read from the testfiles
	int block_size;
	char block_st;

	//If the number of arguments provided is correct then we use them,
	if (argc == 6) {
		n_threads = std::stoi(argv[1]);
		//Distribute the insertions among the threads
		n_insertions = (int) (std::stoi(argv[2]) / n_threads);
		//Distribute the reads among the threads
		n_reads = (int) (std::stoi(argv[3]) / n_threads);
		block_size = std::stoi(argv[4]);
		block_st = argv[5][0];
	}
	//if not, just use the default ones
	else {
		n_threads = 4;
		n_insertions = 256;
		n_reads = 128;
		block_size = 128;
		block_st = 'K';
	}

	//Test parameters summary, with confirmation from the user
	std::cout << n_threads << ", " <<
	n_insertions << ", " <<
	n_reads << ", " <<
	block_size << ", " << 
	block_st << "B, ";
	//Calculate the minimum memory consumption
	int tsize = block_size * n_insertions * n_threads;
	if (block_st == 'M') tsize *= 1024;
	std::cout << (tsize / 1024) << "MB, ";

	//Open the data block that will be stored in all the entries
        std::string path = "../testfiles/" + std::to_string(block_size) + block_st;
        //std::cout << path << std::endl;
	char* block = readFileBytes(path + "a");
	//Creating some objects
	as_config config;
	as_config_init(&config);
	aerospike as;
	as_error err;
	//We connect to the aerospike server
	config.hosts[0] = { .addr = "127.0.0.1", .port = 3000 };
	aerospike_init(&as, &config);
	if (aerospike_connect(&as, &err) != AEROSPIKE_OK) {
		fprintf(stderr, "err(%d) %s at [%s:%d]\n",
			err.code, err.message, err.file, err.line);
	}


	//Defining the atomic double used as a petition counter
	std::atomic<unsigned long> ctr;
	ctr = 0;
	//Array of insertion threads
	std::thread ti[n_threads];
	//Array of read threads
	std::thread tr[n_threads];
	//Auxiliary variable for the next loops
	int i;
	//Get the start time
	auto start = std::chrono::system_clock::now();

	//Create as many insertion threads as specified in the arguments
	if (n_insertions > 0) {
		for (i = 0; i < n_threads; i++) {
			ti[i] = std::thread(insert, std::ref(as), std::ref(err), std::ref(ctr),
				n_insertions, std::ref(block));
		}
	}

	//Create as many read threads as specified in the arguments
	if (n_reads > 0) {
		for (i = 0; i < n_threads; i++) {
			tr[i] = std::thread(read, std::ref(as), std::ref(err), std::ref(ctr), n_reads);
		}
	}

	//Join all the threads
	for (i = 0; i < n_threads; i++) {
		//Only join if they exist. If in the arguments no insertion or read threads
		//were specified then we don't join them
		if (n_insertions > 0) ti[i].join();
		if (n_reads > 0)      tr[i].join();
		//std::cout << "Joined " << i << "I and " << i << "R" << std::endl;
	}


	//Get the end time
	auto end = std::chrono::system_clock::now();
	//and calculate the time difference (execution time)
	std::chrono::duration<double> difference = end - start;
	//Disconnect from the database
	if (aerospike_close(&as, &err) != AEROSPIKE_OK) {
    		fprintf(stderr, "err(%d) %s at [%s:%d]\n",
			err.code, err.message, err.file, err.line);
	}
	aerospike_destroy(&as);
	//std::cout << "Disconnected from the DB" << std::endl;
	//Print the statistics
	std::cout << ctr << ", " <<
	difference.count() << ", " <<
	(long int) (ctr / difference.count()) << std::endl;
	//And exit
	return 0;
}
