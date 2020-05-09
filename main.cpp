#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;


// Structure of process
struct process {

	int memory_num;
	int start_time;
	int end_time;
	int block_num;
	vector<int> memory_blocks;

};

// Structure of Memory Block 
struct memory_block {

	int start_block;
	int end_block;
	int page_num;
	int process_num;
	bool block_available;
};



// Intialize
void readFile(string, vector<process> &);
void printMemory(vector<memory_block> &memory_blocks, int &memory_size, int &page_size);
void print(vector<int> &);
void printResult(vector<process> &, vector<int> &, int &, int &);
void find(vector<process> &, vector<int> &);
bool add_memory(vector<memory_block> &, int &, int &, process &);
void create_memory(vector<memory_block> &, int &, int &);
void remove_memory(vector<memory_block> &, int &, int &, int &);
int check(vector<memory_block> &, int &, int &, int &);



// Functions
void readFile(string input, vector<process> &processing) {

	int processes;
	int blocks;
	vector<int> temp_block;

	// Open file
	ifstream file;
	file.open(input);

	process p;
	file >> processes;

	// Read each data
	for(int i = 0; i < processes; i++) {

		file >> p.memory_num;
		file >> p.start_time;
		file >> p.end_time;
		file >> p.block_num;

		// Read all memory_blocks data
		for(int j = 0; j < p.block_num; j++) {

			file >> blocks;
			temp_block.push_back(blocks);

		}

		p.memory_blocks = temp_block;
		processing.push_back(p);


	}

	// Close file
	file.close();

}

void printMemory(vector<memory_block> &memory_blocks, int &memory_size, int &page_size) {

	int start = 0;
	int end = 0;
	int count_memory = 0;

	while(count_memory < (memory_size / page_size)) {

		if(!memory_blocks[count_memory].block_available) {

			if(start != 0) {

				end = count_memory - 1;
				cout << "       " << start * page_size << "-" << ((end + 1) * page_size) - 1 << ": Free frame(s)" <<endl;
				start = 0;

			}

			cout << "                  " << memory_blocks[count_memory].start_block << "-" << memory_blocks[count_memory].end_block << ": Process " << memory_blocks[count_memory].process_num << " , Page " << memory_blocks[count_memory].page_num << endl;
		
		} else if(start == 0) {

			start = count_memory;

		}

		count_memory++;

	}
}

void print(vector<int> &printing) {

	for(int i = 0; i < printing.size(); i++) {

		cout << printing[i] << " ";

	}

}

void printResult(vector<process> &list, vector<int> &timing, int &memory_size, int &page_size) {

	vector<int> input;
	vector<memory_block> memory_map;
	bool line;
	process p;

	create_memory(memory_map, memory_size, page_size);

	for (int i = 0; i < timing.size(); i++) {

		line = true;

		for (int j = 0; j < list.size(); j++) {

			if(list[j].start_time == timing[i]) {

				input.push_back(list[j].memory_num);

				if(!line) {

					cout << "       ";

				}

				cout << "Process " << list[j].memory_num << " arrives" << endl;

				cout << "Input: [ ";
				print(input);
				cout << "]" << endl;

				line = false;

			} else if(list[j].start_time + list[j].end_time == timing[i]) {

				if(!line) {

					cout << "       ";

				}

				cout << "Process " << list[j].memory_num << " completes" << endl;

				remove_memory(memory_map, memory_size, page_size, list[j].memory_num);
				printMemory(memory_map, memory_size, page_size);

				line = false;

			}

		}

		while(input.size() != 0) {

			p = list[input.front() - 1];

			if(add_memory(memory_map, memory_size, page_size, p)) {

				cout << "Memory Map moves Process " << p.memory_num << " to memory" << endl;
				input.erase(input.begin());

				cout << " Input: [ ";
				print(input);
				cout << "]" << endl;

				printMemory(memory_map, memory_size, page_size);

			} else {

				cout << "\nNo free space!\n\n";
				break;

			}

		}

	}


}

void find(vector<process> &listing, vector<int> &time) {

	bool start_find;
	bool end_find;

	for (int i = 0; i < listing.size(); i++) {

		start_find = false;
		end_find = false;

		for (int j = 0; j < time.size(); j++) {

			if (listing[i].start_time == time[j]) {

				start_find = true;

			}

			if (listing[i].start_time + listing[i].end_time == time[j]) {

				end_find = true;
			}

		}

		if(!start_find) {

			time.push_back(listing[i].start_time);

		}

		if(!end_find) {

			time.push_back(listing[i].start_time + listing[i].end_time);

		}

	}

	sort(time.begin(), time.end());

}

bool add_memory(vector<memory_block> &mapping, int &memory_size, int &page_size, process &input) {

	double block_size;
	bool adding;
	int insize;
	int start_block = -1;
	int numPage = 1;

	for(int i = 0; i < input.block_num; i++) {

		block_size = input.memory_blocks[i] / page_size;

		for(int j = i; j < input.block_num; j++) {

			block_size += (input.memory_blocks[j] / page_size);

		}

		insize = (int) block_size;
		start_block = check(mapping, memory_size, page_size, insize);

		if(start_block == -1) {

			insize = (int) block_size;
			start_block = check(mapping, memory_size, page_size, insize);

		}

		adding = false;

		if(start_block > -1) {

			adding = true;

			for (int j = start_block; j < (start_block + ((input.memory_blocks[i] + 99) / page_size)); j++) {

				mapping[j].process_num = input.memory_num;
				mapping[j].page_num = numPage;
				mapping[j].block_available = false;
				numPage++;

			}

		}

	}

	return adding;

}

void create_memory(vector<memory_block> &mapping, int &memory_size, int &page_size) {

	memory_block t;

	for(int i = 0; i < (memory_size / page_size); i++) {

		t.start_block = i * page_size;
		t.end_block = ((i + 1) * page_size) - 1;
		t.process_num = -1;
		t.page_num = -1;
		t.block_available = true;
		mapping.push_back(t);

	}

}

void remove_memory(vector<memory_block> &mapping, int &memory_size, int &page_size, int &input) {

	for(int i = 0; i < mapping.size(); i++) {

		if(mapping[i].process_num == input) {

			mapping[i].block_available = true;

		}

	}

}

int check(vector<memory_block> &mapping, int &memory_size, int &page_size, int &block_size) {

	int free = 0;

	for(int i = 0; i < mapping.size(); i++) {

		if(mapping[i].block_available) {

			free++;

		} else {

			free = 0;

		}

		if(free == block_size) {

			return (i + 1 - block_size);

		}

	}

	return -1;

}


// Main code
int main(int argc, char** argv) {


	vector<process> process_list;
	vector<int> memory_time;
	int page_size = 0;
	int memory_size = 0;


	if(argc == 3) {

		memory_size = atoi(argv[1]);
		page_size = atoi(argv[2]) * 100;

	} else {

		cout << "Memory Size (Kbytes): " << endl;
		cin >> memory_size;
		cout<< "Page Size (1:100, 2:200, 3:400): ";
		cin >> page_size;
		page_size = page_size * 100;

	}


	if(page_size == 300) {

		page_size += 100;

	}

	readFile("in1.txt", process_list);
	find(process_list, memory_time);
	printResult(process_list, memory_time, memory_size, page_size);

	return 0;


}