#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <fstream>
#include <string.h>
#include "hdfs.h"
#include <mutex>

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

//Function to insert items to the database from an array
int insert (hdfsFS& fs, std::atomic<unsigned long>& ctr, int n, char*& block) {
	int i;
	hdfsFile file;
	//While there are still elements to be inserted
	for (i = 0; i < n; i++) {
		m.lock();
		//Open the file in HDFS
		file = hdfsOpenFile(fs, std::to_string(i).c_str(), O_WRONLY|O_CREAT, 0, 0, 0);
		//And write the block to it
		hdfsWrite(fs, file, (void*)block, strlen(block) + 1);
		//Close the file
		hdfsCloseFile(fs, file);
		m.unlock();
		//Atomically add one to the petition counter
		ctr.fetch_add(1);
	}
	return 0;
}

int read (hdfsFS& fs, std::atomic<unsigned long>& ctr, int n, tSize s) {
	int i, size;
	hdfsFile file;
	char* block;
	//While there are still elements to be read
	for (i = 0; i < n; i++) {
		//Try to read only if the file exists, if not it would raise and exception
		if (!hdfsExists(fs, std::to_string(i).c_str())) {
			m.lock();
			file = hdfsOpenFile(fs, std::to_string(i).c_str(), O_RDONLY, 0, 0, 0);
			//Get the number of bytes we can read from here
			size = hdfsAvailable(fs, file);
			//If we have more available bytes than we should read, then we only read the
			//specified amount
			if (size > s) size = s;
			//Allocate the memory for the buffer
			block = new char[size];
			//And read from it to the block
			hdfsRead(fs, file, block, size);
			//Close the file
			hdfsCloseFile(fs, file);
			m.unlock();
		}
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
	std::cout << std::endl << "--- Test parameters ---" << std::endl;
	std::cout << n_threads << " ithread(s), to insert a total number of " << n_insertions << " blocks of size "
		<< block_size << block_st << "B" << std::endl;
	std::cout << n_threads << " rthread(s), to read a total number of " << n_reads << " entries" << std::endl;
	//Calculate the minimum memory consumption
	int tsize = block_size * n_insertions * n_threads;
	if (block_st == 'M') tsize *= 1024;
	std::cout << "/!\\ The minimum memory consumption will be " << (tsize / 1024) << "MB /!\\" << std::endl << std::endl;
	tSize s = tsize;

	//Open the data block that will be stored in all the entries
        std::string path = "../testfiles/" + std::to_string(block_size) + block_st;
	char* block = readFileBytes(path + "a");
	//Connecting to the server
	hdfsFS fs = hdfsConnect("localhost", 9000);

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
			ti[i] = std::thread(insert, std::ref(fs), std::ref(ctr), n_insertions, std::ref(block));
		}
	}

	//Create as many read threads as specified in the arguments
	if (n_reads > 0) {
		for (i = 0; i < n_threads; i++) {
			tr[i] = std::thread(read, std::ref(fs), std::ref(ctr), n_reads, s);
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
	//std::cout << "Disconnected from the DB" << std::endl;
	//Print the statistics
	std::cout << "Total number of requests made to the database: " << ctr << std::endl;
	std::cout << "Total time elapsed: " << difference.count() << std::endl;
	std::cout << "Petitions per second: " << (long int) (ctr / difference.count()) << std::endl;
	std::cout << std::endl << "If the files were big (MBs) the DB is probably still inserting elements!" << std::endl;
	//And exit
	return 0;
}

