#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstring>

const int globalDepth = 3;
const int blockFactor = 3;

using namespace std;


void SAFE_FILE_OPEN(){
}

struct Record {
	int key;
	char nombre[12];
};

struct Bucket {
	int bucketId{};
	Record records[blockFactor]{};
	int locald;
	int size;
	int nextBucket;

	Bucket(){
		locald = 1;
		size = 0;
		nextBucket = -1;
	}

	bool isFull(){
		return (size == locald);
	}

	void add(Record new_record){
		if(!isFull()){
			records[size++] = new_record;
		}
	}

	void clear(){
		for (int i = 0; i < size; ++i) {
			records[i] = Record();
		}
		size = 0;
	}

	bool isInBucket(int key){
		for (int i = 0; i < size; i++) {
			if (this->records[i].key == key){
				return (true);
			};
		}
		return false;
	}

	Record search(int key){
		if is
	}
};




struct addressRecord {
	char binaryIndex[globalDepth]{};
	int bucketIndex;
	int locald = 1;


};

class ExtendibleHash {
private:
	
	string hashFile;
	string addressTable;
	int nBuckets;
public:
	ExtendibleHash(string addressTable, string hashFile){
		this->hashFile = hashFile;
		this->addressTable = addressTable;


	}








//	ExtendibleHashing(string hashFile, string addressTable) {
//		this->hashFile = hashFile;
//		this->addressTable = addressTable;
//
//		fstream addresstable(addressTable, ios::binary | ios::in | ios::app);
//
//		if (!addresstable.is_open())
//			cout << "Error opening address table" << endl;
//
//		if (addresstable.peek() != ifstream::traits_type::eof()){
//			addresstable.seekg(0, ios::beg);
//			addresstable.read((char*) &nBuckets, sizeof(int));
//		} else {
//			cout << "Creating address table" << endl;
//			addressRecord addressRecord0;
//			addressRecord0.hashIndex[0] = '0';
//			addressRecord0.bucketIndex = 0;
//			addressRecord addressRecord1;
//			addressRecord1.hashIndex[0] = '1';
//			addressRecord1.bucketIndex = 1;
//
//			this->nBuckets = 2;
//			addresstable.seekg(0, ios::beg);
//			addresstable.write((char*) &nBuckets, sizeof(int));
//			addresstable.write((char*) &addressRecord0, sizeof(addressRecord));
//			addresstable.write((char*) &addressRecord1, sizeof(addressRecord));
//		}
//		addresstable.close();
//	}
//
//	void printAll() {
//		ifstream addresstable(addressTable, ios::in | ios::binary);
//
//		cout << "Address Table" << endl;
//		addresstable.seekg(0, ios::beg);
//		addresstable.read((char*) &nBuckets, sizeof(int));
//		cout << "Number of buckets: " << nBuckets << endl;
//
//		addressRecord addressR;
//		while(addresstable.read((char*) &addressR, sizeof(addressRecord))) {
//			cout << addressR.hashIndex << " " << addressR.bucketIndex << endl;
//		}
//
//		addresstable.close();
//	}
};

int main(){
//	ExtendibleHashing hash("addressTable.dat", "addressTable.dat");
//	hash.printAll();

	return 0;
}