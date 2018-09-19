//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/api3.h>
#include <string.h> //strlen
#include <inttypes.h>


std::mutex m;

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

//Function to insert items to the database
int insert (lcb_t& instance, std::atomic<unsigned long>& ctr, int n, char*& block) {

        int i;
	lcb_error_t err;
	lcb_CMDSTORE scmd = { 0 };
	std::string key;

        //While there are still elements to be inserted
        for (i = 0; i < n; i++) {
                //Schedule the storage operation
		//Set the data that will be stored
		key = std::to_string(i);
		LCB_CMD_SET_KEY(&scmd, key.c_str(), key.length());
		LCB_CMD_SET_VALUE(&scmd, block, strlen(block));
		scmd.operation = LCB_SET;
		m.lock();
		err = lcb_store3(instance, NULL, &scmd);
		if (err != LCB_SUCCESS) std::cout << "Could not schedule storage operation" << std::endl;
		//and wait for the process to be synced
		lcb_wait(instance);
		m.unlock();
                //Atomically add one to the petition counter
                ctr.fetch_add(1);
        }
        return 0;
}

//Function to retrieve items from the database
int read (lcb_t& instance, std::atomic<unsigned long>& ctr, int n) {

        int i = 0; 
	lcb_error_t err;
 	lcb_CMDGET gcmd = { 0 };
	std::string key;
	//While there are still elements to be read
        for (i = 0; i < n; i++) {
		//Schedule the retrieval operation
		key = std::to_string(i);
		LCB_CMD_SET_KEY(&gcmd, key.c_str(), key.length());
		m.lock();
		err = lcb_get3(instance, NULL, &gcmd);
		if (err != LCB_SUCCESS) std::cout << "Could not schedule retrieval operation" << std::endl;
		//and wait for the process to be synced
		lcb_wait(instance);
		m.unlock();	
                //Atomically add one to the petition counter
                ctr.fetch_add(1);
        }
        return 0;
}


static void op_callback(lcb_t instance, int cbtype, const lcb_RESPBASE *rb) {

	/*
	fprintf(stderr, "=== %s ===\n", lcb_strcbtype(cbtype));
	if (rb->rc == LCB_SUCCESS) {

		fprintf(stderr, "KEY: %.*s\n", (int)rb->nkey, rb->key);
		fprintf(stderr, "CAS: 0x%"PRIx64"\n", rb->cas);
		if (cbtype == LCB_CALLBACK_GET) {

			const lcb_RESPGET *rg = (const lcb_RESPGET *)rb;
			fprintf(stderr, "VALUE: %.*s\n", (int)rg->nvalue, rg->value);
			fprintf(stderr, "FLAGS: 0x%x\n", rg->itmflags);
		}
	}

	else std::cout << "Error" << std::endl;
	*/
}

int main (int argc, char ** argv) {

	//Print the help
        if (argc == 2) {
                std::string arg_1 = argv[1];
                if (arg_1.compare("-h") == 0) {
                        std::cout << "Usage: ./sync [#THREADS] [#INS PER ITHREAD] "
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

	//Define error, instance, arguments and store/get data types
	lcb_error_t err;
	lcb_t instance;
	struct lcb_create_st arguments = { 0 };
	//Get the arguments for the login and the bucket of the db
	arguments.version = 3;
	arguments.v.v3.connstr = "couchbase://localhost/bucket";
	arguments.v.v3.passwd = "admin65";
	arguments.v.v3.username = "admin";
	//Create the DB client instance with the provided arguments
	err = lcb_create(&instance, &arguments);
	if (err != LCB_SUCCESS) std::cout << "Couldn't create couchbase handle" << std::endl;
	//and connect to the database
	err = lcb_connect(instance);
	if (err != LCB_SUCCESS) std::cout << "Couldn't schedule connection" << std::endl; 
	//Wait for the instance
	lcb_wait(instance);
	err = lcb_get_bootstrap_status(instance);
	if (err != LCB_SUCCESS) std::cout << "Couldn't bootstrap from cluster" << std::endl;
	//Assign the handlers to be called for the operation types
	lcb_install_callback3(instance, LCB_CALLBACK_GET, op_callback);
	lcb_install_callback3(instance, LCB_CALLBACK_STORE, op_callback);

	//Defining the atomic double used as a petition counter
	std::atomic<unsigned long> ctr;
	ctr = 0;
	//Array of insertion threads
	std::thread ti[n_threads];
	//Array of reads threads
	std::thread tr[n_threads];
	//Auxiliary variable for the next loops
	int i;
	//Get the start time
        auto start = std::chrono::system_clock::now();

        //Create as many insertion threads as specified in the arguments
	if (n_insertions > 0) {
		for (i = 0; i < n_threads; i++) {
			ti[i] = std::thread(insert, std::ref(instance), std::ref(ctr), n_insertions, std::ref(block));
		}
	}

        //Create as many read threads as specified in the arguments     
        if (n_reads > 0) {
		for (i = 0; i < n_threads; i++) {
			tr[i] = std::thread(read, std::ref(instance), std::ref(ctr), n_reads);
		}
	}

        //Join all the threads
        for (i = 0; i < n_threads; i++) {
		//Only join if they exist. If in the arguments no insertion or read threads were specified then we don't join them
                if (n_insertions > 0) ti[i].join();
                if (n_reads > 0)      tr[i].join();
                //std::cout << "Joined " << i << "I and " << i << "R" << std::endl;
        }


	//Get the end time
        auto end = std::chrono::system_clock::now();
        //and calculate the time difference (execution time)
        std::chrono::duration<double> difference = end - start;
	//Now that we're all done, close down the connection handle
	lcb_destroy(instance);
	//Print the statistics
        std::cout << ctr << ", " <<
        difference.count() << ", " <<
        (long int) (ctr / difference.count()) << std::endl;
	//And exit
	return 0;
}
