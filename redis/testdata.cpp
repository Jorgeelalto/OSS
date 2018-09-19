#include <iostream>
#include <functional>
#include <cpp_redis/cpp_redis>
#include <iostream>  
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

int main (int argc, char ** argv) {

	//Print the help if needed
	if (argc == 2) {
		std::string arg_1 = argv[1];
		if (arg_1.compare("-h") == 0) {
			std::cout << "Usage: testdata [#INSERTIONS] [INSERTION SIZE] [K/M]" << std::endl;
			return 0;
		}
	}

	//How many blocks (entries) are we going to write to the
	//DB and the size of each block
	int n_blocks, block_size;
	char block_st;
	//If the arguments are provided then use those
	if (argc == 4) {
		n_blocks = std::stoi(argv[1]);
		block_size = std::stoi(argv[2]);
		block_st = argv[3][0];
	}
	else if (argc == 3) {
		n_blocks = 4096;
		block_size = std::stoi(argv[1]);
		block_st = argv[2][0];
		std::cout << argv[2][0] << std::endl;
	}
	//If not provided then use the default values
	else {
		//500MB of data in 1M entries, 128KB each
		n_blocks = 4096;
		block_size = 128;
		block_st = 'k';
	}

	//Creating some objects
        cpp_redis::client client;
        //We connect to the redis server
        client.connect("127.0.0.1", 6379);
        std::cout << "Connected to the database" << std::endl;
	
	//Open the data block that will be stored in all the entries
	std::string path;
	//if (block_st == 'K') path = "../testfiles/" + std::to_string(block_size) + "K";
	/*else*/ path = "../testfiles/" + std::to_string(block_size) + block_st;
	std::cout << path << std::endl;
	char* block[4];
	block[0] = readFileBytes(path + "a");
	block[1] = readFileBytes(path + "b");
	block[2] = readFileBytes(path + "c");
	block[3] = readFileBytes(path + "d");

	int i, block_n = 0;
	for (i = 0; i < n_blocks; i++) {
		//Insert it into the database
		if (block_n > 3) block_n = 0;
	       	client.set(std::to_string(i), block[block_n++]);
	        //Synced commit
        	client.sync_commit();
	        std::cout << "Stored block number " << i << " of size "
			<< block_size << block_st << std::endl;
	}
}
