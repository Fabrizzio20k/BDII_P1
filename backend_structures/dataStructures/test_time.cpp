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

vector<Record<ll>> readData(string filename)
{
    ifstream file(filename);

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

void measureTime()
{
    // Read data from file
    vector<Record<ll>> records = readData("dbs/test_1000.csv");
    AVLFile<ll> avlFile("data.dat");

    /*
     * Insert 1000 records in AVL and Extendible Hash
     * Measure time
     */

    cout << "----------------------------------------" << endl;

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

    // Search id 769989821
    start = chrono::high_resolution_clock::now();
    avlFile.search(769989821);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to search id 769989821 in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    start = chrono::high_resolution_clock::now();
    eh.search(769989821);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to search id 769989821 in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Range search only works for AVL
    start = chrono::high_resolution_clock::now();
    avlFile.rangeSearch(500000000, 700000000);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to range search in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Delete all ids and chrono
    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        avlFile.remove(record.id);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to delete all records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    /*
     * Insert 10000 records in AVL and Extendible Hash
     * Measure time
     */

    cout << "----------------------------------------" << endl;

    records = readData("dbs/test_10000.csv");
    avlFile.clear();
    eh.clear();
    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        avlFile.insert(record);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to insert 10000 records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        eh.insert(record);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to insert 10000 records in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Search id 879301187
    start = chrono::high_resolution_clock::now();
    avlFile.search(879301187);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to search id 879301187 in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    start = chrono::high_resolution_clock::now();
    eh.search(879301187);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to search id 879301187 in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Range search only works for AVL
    start = chrono::high_resolution_clock::now();
    avlFile.rangeSearch(500000000, 700000000);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to range search in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Delete all ids and chrono
    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        avlFile.remove(record.id);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to delete all records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    cout << "----------------------------------------" << endl
         << endl;

    /*
     * Insert 100000 records in AVL and Extendible Hash
     * Measure time
     */

    cout << "----------------------------------------" << endl;

    records = readData("dbs/test_100000.csv");
    avlFile.clear();
    eh.clear();
    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        avlFile.insert(record);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to insert 100000 records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        eh.insert(record);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to insert 100000 records in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Search id 820014960
    start = chrono::high_resolution_clock::now();
    avlFile.search(820014960);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to search id 820014960 in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    start = chrono::high_resolution_clock::now();
    eh.search(820014960);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to search id 820014960 in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Range search only works for AVL
    start = chrono::high_resolution_clock::now();
    avlFile.rangeSearch(500000000, 700000000);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to range search in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // Delete all ids and chrono
    start = chrono::high_resolution_clock::now();
    for (auto &record : records)
    {
        avlFile.remove(record.id);
    }
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "Time to delete all records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    cout << "----------------------------------------" << endl
         << endl;

    /*
     * Insert 1000000 records in AVL and Extendible Hash
     * Measure time
     */

    // cout << "----------------------------------------" << endl;

    // records = readData("dbs/test_1000000.csv");
    // avlFile.clear();
    // eh.clear();
    // start = chrono::high_resolution_clock::now();
    // for (auto &record : records)
    // {
    //     avlFile.insert(record);
    // }
    // end = chrono::high_resolution_clock::now();
    // diff = end - start;
    // cout << "Time to insert 1000000 records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // start = chrono::high_resolution_clock::now();
    // for (auto &record : records)
    // {
    //     eh.insert(record);
    // }
    // end = chrono::high_resolution_clock::now();
    // diff = end - start;
    // cout << "Time to insert 1000000 records in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // // Search id 190843224
    // start = chrono::high_resolution_clock::now();
    // avlFile.search(190843224);
    // end = chrono::high_resolution_clock::now();
    // diff = end - start;
    // cout << "Time to search id 190843224 in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // start = chrono::high_resolution_clock::now();
    // eh.search(190843224);
    // end = chrono::high_resolution_clock::now();
    // diff = end - start;
    // cout << "Time to search id 190843224 in Extendible Hash: " << fixed << setprecision(6) << diff.count() << " s" << endl << endl;

    // // Range search only works for AVL
    // start = chrono::high_resolution_clock::now();
    // avlFile.rangeSearch(500000000, 700000000);
    // end = chrono::high_resolution_clock::now();
    // diff = end - start;
    // cout << "Time to range search in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // // Delete all ids and chrono
    // start = chrono::high_resolution_clock::now();
    // for (auto &record : records)
    // {
    //     avlFile.remove(record.id);
    // }
    // end = chrono::high_resolution_clock::now();
    // diff = end - start;
    // cout << "Time to delete all records in AVL: " << fixed << setprecision(6) << diff.count() << " s" << endl;

    // cout << "----------------------------------------" << endl;
}

int main()
{
    measureTime();

    return 0;
}