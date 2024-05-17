#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

const int BLOCK_FACTOR = 3;
const int MAX_DEPTH = 3;

struct Record {
	int key;
	string value;
};

struct Bucket {
	int localDepth;
	int size;
	Record record[BLOCK_FACTOR];
	int nextBucket;

	Bucket(){
		localDepth = 1;
		size = 0;
		nextBucket = -1;
	}
};

struct Directory {
	char binaryIndex[MAX_DEPTH];
	Bucket bucket;

	Directory(){}

	Directory(int binaryIndex, int bucketIndex){
		bitset<3> bit(binaryIndex);
		string nbit = bit.to_string();

		for(int i = 0; i < MAX_DEPTH; i++){
			this->binaryIndex[i] = nbit[i];
		}

		bucket = Bucket();
	}
};

class ExtendibleHashing {
private:
	int globalDepth;
	vector<Directory> directories;

public:
	// Generates by default two Directory as base and its globalDepth is at the beginning 1
	ExtendibleHashing(){
		globalDepth = 1;
		directories.push_back(Directory(10, 0));
		directories.push_back(Directory(12, 1));
	}

	int getDirectoryIndex(int key){
		int mask = (1 << globalDepth) - 1;
		int lsb_k = key & mask;
		return lsb_k;
	}

	/*
	Obtains a Record and inserts it into the corresponding Bucket
	*/
	void insertRecord(Record record){
		int directory_index  = getDirectoryIndex(record.key);
		auto bucket = directories[directory_index].bucket;
		if(directories[directory_index].bucket.size < BLOCK_FACTOR) {
			directories[directory_index].bucket.record[directories[directory_index].bucket.size++] = record;
		}
		else if (bucket.localDepth == globalDepth){
			 vector<Directory> new_directories(2*directories.size());
			 for (int i = 0 ; i < directories.size() ; i++) {
				 if( i == directory_index) {
					 new_directories[i].bucket = Bucket();
					 new_directories[f(i)].bucket = Bucket();// en memoria secundaria apunta a last_bucket + 1 , y luego last_bucket++
				 	 new_directories[i].bucket.localDepth = globalDepth + 1;
				 	 new_directories[f(i)].bucket.localDepth = globalDepth + 1;
				 }else{
					 new_directories[i].bucket = directories[i].bucket;
					 new_directories[f(i)].bucket = directories[f(i)].bucket;
				 }
			 }

			auto record_array = bucket.record;
			globalDepth++;
			for (int i  = 0 ; i < BLOCK_FACTOR ; i++) {
				auto key = record_array[i].key;
				int pos = getDirectoryIndex(key);
				new_directories[pos].bucket.record[new_directories[pos].bucket.size++] = record_array[i];
			}

			this->directories = new_directories;

		}
	}

	void printAll(){
		for(int i = 0; i < directories.size(); i++){
			cout << "Bucket " << i << ": ";

			for (int j = 0; j < directories[i].bucket.size; j++){
				cout << directories[i].bucket.record[j].key << " " << directories[i].bucket.record[j].value << " | ";
			}
			cout << endl;
		}

	}
};

int main(){
	ExtendibleHashing eh;

	Record r1 = {4, "hello"};
	Record r2 = {10, "hello2"};
	Record r3 = {9, "hello3"};

	eh.insertRecord(r1);
	eh.insertRecord(r2);
	eh.insertRecord(r3);


	eh.printAll();

	return 0;
}
