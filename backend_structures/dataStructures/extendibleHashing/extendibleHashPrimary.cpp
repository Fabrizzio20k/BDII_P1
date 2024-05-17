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

	int getBucketIndex(int key){
		int mask = (1 << globalDepth) - 1;
		int lsb_k = key & mask;
		return lsb_k;
	}

	/*
	Obtains a Record and inserts it into the corresponding Bucket
	*/
	void insertRecord(Record record){
		int bucketIndex = getBucketIndex(record.key);

		if(directories[bucketIndex].bucket.size < BLOCK_FACTOR){
			directories[bucketIndex].bucket.record[directories[bucketIndex].bucket.size++] = record;
		} else {
			// Split
			// 1. Incrementar el globalDepth
			// 2. Duplicar los directorios
			// 3. Reasignar los registros
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

