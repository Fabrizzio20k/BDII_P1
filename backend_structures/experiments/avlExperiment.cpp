#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <cstring>
#include "avlfile.cpp"

using namespace std;
using namespace std::chrono;

#define ll long long

void test(AVLFile<int> &file, ll dataSize) {
	file.clear();

	for (int i = 0; i < dataSize; i++) {
		Record<int> r;

		string name = "Name" + to_string(i);

		r.cod = i;
		strncpy(r.nombre, name.c_str(), 12);
		r.ciclo = i % 10;

		file.insert(r);
	}
}


int main(){
	AVLFile<int> avlfile("data.dat");
	ll dataSize = 1000;

	test(avlfile, dataSize);

	time_point<high_resolution_clock> start, end;

	// Fix this
//	duration<double> time_span;

	cout << "Busqueda de un registro" << endl;

	start = high_resolution_clock::now();
	auto records = avlfile.search(63);
	end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(end - start);

	cout << "Tiempo de busqueda " << duration.count() << " ms" << endl;

	// #############################################################3

//	cout << "Búsqueda de un rango de registros" << endl;

//	start = high_resolution_clock::now();
//	auto records2 = avlfile.rangeSearch(63, 70);
//	end = high_resolution_clock::now();




	return 0;
}