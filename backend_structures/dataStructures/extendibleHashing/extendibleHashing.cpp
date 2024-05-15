#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstring>

const int globalD = 3;
const int maxSize = 3;

using namespace std;

struct Record {
	int key;
	char nombre[12];
	int ciclo;
};

struct Bucket {
	int bucketId{};
	Record records[maxSize]{};
	int locald;
	int size;
	int nextBucket;

	Bucket(){
		locald = 1;
		size = 0;
		nextBucket = -1;
	}
};

struct addressRecord {
	char hashIndex[globalD]{};
	int bucketIndex;
	int locald = 1;

//	addressRecord(string hashIndex, int bucketIndex){
//		this->locald = 1;
//
//		strncpy(this->hashIndex, hashIndex.c_str(), 3);
//
//		this->bucketIndex = bucketIndex;
//	}
//
//	addressRecord(){
//		this->locald = 1;
//		this->bucketIndex = -1;
//	}
};

class ExtendibleHashing {
private:
	string hashFile;
	string addressTable;
	int nBuckets;
public:
	ExtendibleHashing(string hashFile, string addressTable) {
		this->hashFile = hashFile;
		this->addressTable = addressTable;

		fstream addresstable(addressTable, ios::binary | ios::in | ios::app);

		if (!addresstable.is_open())
			cout << "Error opening address table" << endl;

		if (addresstable.peek() != ifstream::traits_type::eof()){
			addresstable.seekg(0, ios::beg);
			addresstable.read((char*) &nBuckets, sizeof(int));
		} else {
			cout << "Creating address table" << endl;
			addressRecord addressRecord0;
			addressRecord0.hashIndex[0] = '0';
			addressRecord0.bucketIndex = 0;
			addressRecord addressRecord1;
			addressRecord1.hashIndex[0] = '1';
			addressRecord1.bucketIndex = 1;

			this->nBuckets = 2;
			addresstable.seekg(0, ios::beg);
			addresstable.write((char*) &nBuckets, sizeof(int));
			addresstable.write((char*) &addressRecord0, sizeof(addressRecord));
			addresstable.write((char*) &addressRecord1, sizeof(addressRecord));
		}
		addresstable.close();
	}

	void printAll() {
		ifstream addresstable(addressTable, ios::in | ios::binary);

		cout << "Address Table" << endl;
		addresstable.seekg(0, ios::beg);
		addresstable.read((char*) &nBuckets, sizeof(int));
		cout << "Number of buckets: " << nBuckets << endl;

		addressRecord addressR;
		while(addresstable.read((char*) &addressR, sizeof(addressRecord))) {
			cout << addressR.hashIndex << " " << addressR.bucketIndex << endl;
		}

		addresstable.close();
	}
};

int main(){
	ExtendibleHashing hash("addressTable.dat", "addressTable.dat");

	hash.printAll();

	return 0;
}