#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "appleRecord.h"

using namespace std;

int main()
{
    ifstream file("AppleStore.csv");

    if (!file.is_open())
    {
        std::cout << "No se pudo abrir el archivo" << std::endl;
        return 1;
    }

    vector<Record> records;
    Record record;

    // skip the first line
    string line;
    getline(file, line);

    // read the first 10 records
    while (file >> record && file.peek() != EOF)
    {
        records.push_back(record);
    }

    cout << "Total records read: " << records.size() << endl;

    for (const auto &r : records)
    {
        cout << r.id << " " << r.prime_genre << endl;
    }

    file.close();
    return 0;
}
