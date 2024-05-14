#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct appleRecord
{
    long long id;
    string track_name;
    long long size_bytes;
    string currency;
    double price;
    long long rating_count_tot;
    long long rating_count_ver;
    double user_rating;
    double user_rating_ver;
    string ver;
    string cont_rating;
    string prime_genre;
    int sup_devices_num;
    int ipadSc_urls_num;
    int lang_num;
    int vpp_lic;

    friend istream &operator>>(istream &str, appleRecord &data)
    {
        string line;
        getline(str, line);

        stringstream lineStream(line);
        string cell;

        getline(lineStream, cell, ',');
        data.id = stoll(cell);

        getline(lineStream, data.track_name, ',');

        getline(lineStream, cell, ',');
        data.size_bytes = stoll(cell);

        getline(lineStream, data.currency, ',');

        getline(lineStream, cell, ',');
        data.price = stod(cell);

        getline(lineStream, cell, ',');
        data.rating_count_tot = stoll(cell);

        getline(lineStream, cell, ',');
        data.rating_count_ver = stoll(cell);

        getline(lineStream, cell, ',');
        data.user_rating = stod(cell);

        getline(lineStream, cell, ',');
        data.user_rating_ver = stod(cell);

        getline(lineStream, data.ver, ',');

        getline(lineStream, data.cont_rating, ',');

        getline(lineStream, data.prime_genre, ',');

        getline(lineStream, cell, ',');
        data.sup_devices_num = stoi(cell);

        getline(lineStream, cell, ',');
        data.ipadSc_urls_num = stoi(cell);

        getline(lineStream, cell, ',');
        data.lang_num = stoi(cell);

        getline(lineStream, cell, ',');
        data.vpp_lic = stoi(cell);

        return str;
    }
    void print()
    {
        cout << "ID: " << id << endl;
        cout << "Track Name: " << track_name << endl;
        cout << "Size Bytes: " << size_bytes << endl;
        cout << "Currency: " << currency << endl;
        cout << "Price: " << price << endl;
        cout << "Rating Count Total: " << rating_count_tot << endl;
        cout << "Rating Count Version: " << rating_count_ver << endl;
        cout << "User Rating: " << user_rating << endl;
        cout << "User Rating Version: " << user_rating_ver << endl;
        cout << "Version: " << ver << endl;
        cout << "Content Rating: " << cont_rating << endl;
        cout << "Prime Genre: " << prime_genre << endl;
        cout << "Sup Devices Num: " << sup_devices_num << endl;
        cout << "Ipad Sc Urls Num: " << ipadSc_urls_num << endl;
        cout << "Lang Num: " << lang_num << endl;
        cout << "Vpp Lic: " << vpp_lic << endl;
    }
};
