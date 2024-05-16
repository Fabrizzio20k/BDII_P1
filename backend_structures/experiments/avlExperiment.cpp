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
	string path = __FILE__;
	path = path.substr(0, path.find_last_of("\\/"));
	path = path.substr(0, path.find_last_of("\\/"));
	path = path + "/dataStructures/dbs/AppleStore.csv";

	ifstream file(path);

	avlfile.clear();

	if (!file.is_open()){
		cerr << "No se pudo abrir el archivo" << endl;
	}

	Record<ll> record;

	string line;
	getline(file, line);

	int i = 0;
	while (file >> record && file.peek() != EOF){
		avlfile.insert(record);
 		i++;
 	}

	cout << "Total records read: " << i << endl;

//	for (const auto &r : records){
//		cout << r.id << " " << r.prime_genre << endl;
//	}
//
	file.close();
}

//void fulfill(AVLFile<ll> &file, ll beg, ll dataSize) {
//	file.clear();
//
//	for (ll i = beg; i < dataSize; i++) {
//		Record<ll> r;
//
//		string name = "Name" + to_string(i);
//
//		r.id = i;
//		strncpy(r.nombre, name.c_str(), 12);
//		r.ciclo = 1;
//
//		file.insert(r);
//	}
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

	// avlfile.printAll();

	Record<ll> r ={ 99, "YAPE FAKE APK", 1000000, "USD", 0.0, 0, 0, 0.0, 0.0, "1.0", "4+", "Finance", 38, 5, 1, 1, -1, -1, 0};

	avlfile.insert(r);
	//avlfile.remove(124324423000);
	auto v1 = avlfile.rangeSearch(300000000, 429047995);
	auto v2 = avlfile.search(99);

	cout << "Size v1: " << v1.size() << endl;
	cout << "Size v2: " << v2.size() << endl;

	for (auto&i : v2) {
		cout << i.id;
	}

	return 0;
}