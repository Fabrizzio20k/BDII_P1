#include "extendibleHashOficial.h"

void test_insert()
{
    ExtendibleHash<ll> eh;

    ifstream file("AppleStore.csv");

    if (!file.is_open())
    {
        std::cout << "No se pudo abrir el archivo" << std::endl;
        return;
    }

    vector<Record<ll>> records;
    Record<ll> record;

    // skip the first line
    string line;
    getline(file, line);

    while (file >> record && file.peek() != EOF)
    {
        records.push_back(record);
        eh.insert(record);
    }

    cout << "Total records read after insert: " << records.size() << endl;

    file.close();
}

void test_search()
{
    ExtendibleHash<ll> eh1;

    vector<Record<ll>> records = eh1.get_all();
    cout << "Total records read: " << records.size() << endl;

    // search for a record
    Record<ll> r = eh1.search(2);
    if (r.id == 0)
    {
        cout << "Record not found" << endl;
    }
    else
    {
        cout << "Record found: " << endl;
        r.print();
    }
}

int main()
{
    ExtendibleHash<ll>::delete_files();
    test_insert();
    test_search();
    ExtendibleHash<ll>::delete_files();

    return 0;
}
