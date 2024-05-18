#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

const int BLOCK_FACTOR = 3;
const int MAX_DEPTH = 3;

struct Record {
	int key;
//	string value;
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

	/*
	Obtains the key of the record and returns the index of the directory where it should be inserted
	It means, it returns the D least significant bits of the key
	*/
	int getDirectoryIndex(int key){
		int mask = (1 << globalDepth) - 1;
		int lsb_k = key & mask;
		return lsb_k;
	}

	/*
	Obtains a Record and inserts it into the corresponding Bucket
	*/
	void insertRecord(Record record){
		int directoryIndex  = getDirectoryIndex(record.key);
		auto bucket = directories[directoryIndex].bucket;

		// If the bucket is not full, insert the record (trivial)
		if(bucket.size < BLOCK_FACTOR) {
			bucket.record[bucket.size++] = record;
		}
		/* If not, we split in two situations
		 1. The localDepth of the bucket is less than the globalDepth (Just split the bucket)
		 	a. Store the element of the overflowed bucket in a temporary array
		 	b. Split the buckets: Create a new bucket at the end of the file
		 	c. Update the directory pointers:
		 		- The directoryIndex where its bucket overflowed, stil poiting to the old bucket
		 		- We apply a function to the current directoryIndex that adds a 1 to the left of its binary
		 		  representation and the resultant directoryIndex now points to the new bucket
			d. Reinsert the elements of the temporary array applying the getDirectoryIndex function

		 2. The localDepth of the bucket is equal to the globalDepth (Split the directory)
		 	a. Create a new vector of directories with double the size
		 	b. Store the elements of the overflowed bucket in a temporary array
		 	c. Copy the elements of the old directories with a loop to the new directories as follows:
		 		- If the directory not is the directoryIndex that overflowed, copy the bucket
		 		- We apply a function to the current directoryIndex that adds a 1 to the left of its binary
		 		  representation and the resultant directoryIndex now points to the same bucket
				- If the directory is the directoryIndex that overflowed, create two new buckets
				  the current directoryIndex still poiting to the old bucket. Then apply the function to the current
				  directoryIndex and points to the new bucket
		 	d. Update the globalDepth
		 	e. Reinsert the elements of the temporary array applying the getDirectoryIndex function
		*/
		else if (bucket.localDepth == globalDepth) {
//			vector<Directory> new_directories(2*directories.size());
//			for (int i = 0 ; i < directories.size() ; i++) {
//				if(i == directoryIndex) {
//					new_directories[i].bucket = Bucket();
//					new_directories[f(i)].bucket = Bucket();// en memoria secundaria apunta a last_bucket + 1 , y luego last_bucket++
//					new_directories[i].bucket.localDepth = globalDepth + 1;
//					new_directories[f(i)].bucket.localDepth = globalDepth + 1;
//				}else{
//					new_directories[i].bucket = directories[i].bucket;
//					new_directories[f(i)].bucket = directories[f(i)].bucket;
//				}
//			}
//
//			auto record_array = bucket.record;
//			globalDepth++;
//			for (int i  = 0 ; i < BLOCK_FACTOR ; i++) {
//				auto key = record_array[i].key;
//				int pos = getDirectoryIndex(key);
//				new_directories[pos].bucket.record[new_directories[pos].bucket.size++] = record_array[i];
//			}
//
//			this->directories = new_directories;

		}
	}

	void printAll(){
		for(int i = 0; i < directories.size(); i++){
			cout << "Bucket " << i << ": ";

			for (int j = 0; j < directories[i].bucket.size; j++){
				cout << directories[i].bucket.record[j].key << " | ";
			}
			cout << endl;
		}

	}
};

// Función
int addOneToLeft(int num) {
	auto numOriginalBits = (sizeof(num) * 8) - __builtin_clz(num);
	int result = (1 << numOriginalBits) | num;
	return result;
}


int main(){
	ExtendibleHashing eh;

	Record r1 = {2};
	Record r2 = {4};
	Record r3 = {3};
	Record r4 = {7};
	Record r5 = {9};
	Record r6 = {11};

	eh.insertRecord(r1);
	eh.insertRecord(r2);
	eh.insertRecord(r3);
	eh.insertRecord(r4);
	eh.insertRecord(r5);
	eh.insertRecord(r6);


	cout << addOneToLeft(0) << endl;
	cout << addOneToLeft(1) << endl;
	cout << addOneToLeft(2) << endl;
	cout << addOneToLeft(3) << endl;
	cout << addOneToLeft(4) << endl;

	eh.printAll();

	return 0;
}




//int main() {
//	int numero = 13; // Número de ejemplo
//	int resultado = agregarUnoALaIzquierda(numero);
//	std::cout << "Numero original: " << numero << " (" << std::bitset<8>(numero) << ")" << std::endl;
//	std::cout << "Resultado: " << resultado << " (" << std::bitset<8>(resultado) << ")" << std::endl;
//	return 0;
//}