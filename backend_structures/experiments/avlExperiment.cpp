#include <iostream>
#include <chrono>
#include <string>
#include <cstring>
#include "avlfile.cpp"
//#include "appleRecord.h"

using namespace std;
using namespace std::chrono;

#define ll long long

void loadAppleData(AVLFile<ll> &avlfile){
	ifstream file(R"(C:\Users\jeffr\GitHub\BDII_P1\backend_structures\dataStructures\dbs\AppleStore.csv)");

	avlfile.clear();

	if (!file.is_open()){
		cerr << "No se pudo abrir el archivo" << endl;
	}

	vector<Record<ll>> records;
	Record<ll> record;

	string line;
	getline(file, line);

	int i = 0;
	while (file >> record && i < 6){
		avlfile.insert(record);
		i++;
	}

//	while (file >> record && file.peek() != EOF){
////		records.push_back(record);
//		avlfile.insert(record);
//	}

//	cout << "Total records read: " << records.size() << endl;

//	for (const auto &r : records){
//		cout << r.id << " " << r.prime_genre << endl;
//	}

	file.close();
}

//void fulfill(AVLFile<int> &file, ll dataSize) {
//	file.clear();
//
////	for (int i = 0; i < dataSize; i++) {
////		Record<int> r;
////
////		string name = "Name" + to_string(i);
////
////		r.cod = i;
////		strncpy(r.nombre, name.c_str(), 12);
////		r.ciclo = i % 10;
////
////		file.insert(r);
////	}
//}

//void testSingleSearch(AVLFile<int> &avlfile, int key) {
//	cout << "Busqueda de un registro" << endl;
//
//	auto start = high_resolution_clock::now();
//	auto records = avlfile.search(key);
//	auto end = high_resolution_clock::now();
//	auto time_span = duration_cast<microseconds>(end - start);
//
//	for (auto r : records)
//		cout << r.cod << " " << r.nombre << " " << endl;
//
//	cout << "Tiempo de busqueda: " << time_span.count() << " ms" << endl;
//	cout << "################################" << endl << endl;
//}
//
//void testRangeSearch(AVLFile<int> &avlfile, int beginKey, int endKey) {
//	cout << "Busqueda de un rango de registros" << endl;
//
//	auto start = high_resolution_clock::now();
//	auto records = avlfile.rangeSearch(beginKey, endKey);
//	auto end = high_resolution_clock::now();
//	auto time_span = duration_cast<microseconds>(end - start);
//
//	for (auto r : records)
//		cout << r.cod << " " << r.nombre << " " << endl;
//
//	cout << "Tiempo de busqueda por rango: " << time_span.count() << " ms" << endl;
//	cout << "################################" << endl << endl;
//}
//
//void testInsert(AVLFile<int> &avlfile, int key) {
//	cout << "Insercion de un registro" << endl;
//
//	Record<int> r = {key, "Name", 1};
//
//	auto start = high_resolution_clock::now();
//	avlfile.insert(r);
//	auto end = high_resolution_clock::now();
//	auto time_span = duration_cast<microseconds>(end - start);
//
//	cout << "Tiempo de insercion: " << time_span.count() << " ms" << endl;
//	cout << "################################" << endl << endl;
//}
//
//void testDelete(AVLFile<int> &avlfile, int key) {
//	cout << "Eliminacion de un registro" << endl;
//
//	auto start = high_resolution_clock::now();
//	cout << avlfile.remove(key);
//	auto end = high_resolution_clock::now();
//	auto time_span = duration_cast<microseconds>(end - start);
//
//	cout << "Tiempo de eliminacion: " << time_span.count() << " ms" << endl;
//	cout << "################################" << endl << endl;
//}

int main(){
	AVLFile<ll> avlfile("data.dat");

	loadAppleData(avlfile);

	avlfile.printAll();
	return 0;
}