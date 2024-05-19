#include "appleRecord.h"
#include "BPlusTree.h"
#include <chrono>
#define ll long long

using namespace std;
using namespace std::chrono;


auto cmp = [](ll k1, ll k2) -> ll
{
    if (k1 < k2)
        return -1;
    if (k1 > k2)
        return 1;
    return 0;
};

auto getKey = [](Record<ll> &s)
{ return s.id; };
vector<Record<ll>> readAndInsertData(string filename)
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

    remove("bplustree.bin");
    BPlusTree<Record<ll>, ll, sizeof(ll)> btree("bplustree.bin", getKey, cmp);

    auto start = std::chrono::high_resolution_clock::now();
    while (file >> record && file.peek() != EOF)
    {
        btree.insert(record);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto time_span = duration_cast<std::chrono::microseconds>(end - start);

    cout << "################################" << endl << endl;
    cout << "Insert time: " << time_span.count() << " microseconds" << endl;

//    cout << "Total records read after insert: " << records.size() << endl;

    file.close();
    return records;
}

int main()
{
    vector<Record<ll>> records = readAndInsertData("AppleStore.csv");
}
