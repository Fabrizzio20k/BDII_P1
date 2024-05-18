#include "dbs/appleRecord.h"
#include <fstream>
#include <vector>
#include <chrono>
#include <iostream>
#include <ctime>
#include <iomanip>
#include "avlFile/avlfile.cpp"
#include "extendibleHash/extendibleHashOficial.h"

#define ll long long

using namespace std;

vector<Record<ll>> readData()
{
    ifstream file("dbs/test_1000.csv");

    if (!file.is_open())
    {
        std::cout << "No se pudo abrir el archivo" << std::endl;
        return {};
    }

    vector<Record<ll>> records;
    Record<ll> record;

    // skip the first line
    string line;
    getline(file, line);

    while (file >> record && file.peek() != EOF)
    {
        records.push_back(record);
    }

    cout << "Total records read after insert: " << records.size() << endl;

    file.close();
    return records;
}

void measureTimeInsert()
{
    vector<Record<ll>> records = readData();
    AVLFile<ll> avlFile("data.dat");
    auto start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        avlFile.insert(record);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    cout << "Time to insert 1000 records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    ExtendibleHash<ll> eh;
    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        eh.insert(record);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to insert 1000 records in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;
}

int main()
{
    measureTimeInsert();

    return 0;
}