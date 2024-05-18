#ifndef API_H
#define API_H
#define ll long long

#include "crow.h"
#include "crow/middlewares/cors.h"
#include <iostream>
#include "avlfile.cpp"
#include "appleRecord.h"
#include "extendibleHashOficial.h"
#include <cstring>
#include <string>

using namespace std;

class Api {
public:
    Api();

    void run();

    ~Api() = default;

private:
    crow::App<crow::CORSHandler> app;
    AVLFile<ll> avlfile = AVLFile<ll>("data.dat");
    ExtendibleHash<ll> eh = ExtendibleHash<ll>();
    bool isCreated = false;
    string tableName;
    string columnIndex;
    string indexType;
    string fileName;
    ll searchOne{};
    ll searchTwo{};
};

inline Api::Api() {
    auto cors = app.get_middleware<crow::CORSHandler>();
    avlfile.clear();
    isCreated = false;

    cors
            .global()
            .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
            .methods("POST"_method, "GET"_method, "DELETE"_method)
            .prefix("/cors")
            .origin("example.com")
            .prefix("/nocors")
            .ignore();

    CROW_ROUTE(app, "/").methods("GET"_method)
    ([]() {
        return crow::response(200, "Hello, World!");
    });

    CROW_ROUTE(app, "/api/v1/create").methods("POST"_method)
    ([this](const crow::request &req) {
        vector<Record<ll>> records;
        auto data = crow::json::load(req.body);
        //verify if field file_name, table_name, index and column_index are present
        if (!data || !data.has("file_name") || !data.has("table_name") || !data.has("index") || !data.has(
                "column_index")) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "No file_name, table_name, index or column_index provided";
            return crow::response(response);
        }

        if (data["file_name"] != "appleStore.csv") {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "File name does not exist";
            return crow::response(response);
        }

        if (data["index"] != "ISAM" && data["index"] != "HASH" && data["index"] != "AVL") {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Index type not supported";
            return crow::response(response);
        }

        if (data["column_index"] != "id") {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Column index not supported";
            return crow::response(response);
        }

        string path = __FILE__;
        path = path.substr(0, path.find_last_of("\\/"));
        path = path.substr(0, path.find_last_of("\\/"));
        path = path + "/dataStructures/dbs/AppleStore.csv";

        ifstream file(path);
        if (!file.is_open()) {
            cerr << "No se pudo abrir el archivo" << endl;
        }

        Record<ll> record;
        vector<Record<ll>> recs;

        string line;
        getline(file, line);

        while (file >> record && file.peek() != EOF) {
            recs.push_back(record);
        }
        file.close();

        if (data["index"] == "ISAM") {
            indexType = "ISAM";
            cout << "Creating ISAM index" << endl;
        }
        else if (data["index"] == "HASH") {
            indexType = "HASH";
            eh.clear();
            for (const auto & rec : recs) {
                eh.insert(rec);
            }
            records = eh.get_all();
        }
        else
        {
            indexType = "AVL";
            avlfile.clear();
            for (const auto & rec : recs) {
                avlfile.insert(rec);
            }
            records = avlfile.inorder();
        }

        cout << "Total records: " << records.size() << endl;

        isCreated = true;
        tableName = string{data["table_name"]};
        columnIndex = string{data["column_index"]};
        fileName = string{data["file_name"]};

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Table created successfully";
        response["total_records"] = records.size();
        response["index_type"] = indexType;
        response["table_name"] = tableName;
        response["column_index"] = columnIndex;
        response["file_name"] = fileName;

        for (int i = 0; i < records.size(); i++) {
            response["records"][i]["id"] = records[i].id;
            response["records"][i]["track_name"] = records[i].track_name;
            response["records"][i]["size_bytes"] = records[i].size_bytes;
            response["records"][i]["currency"] = records[i].currency;
            response["records"][i]["price"] = records[i].price;
            response["records"][i]["rating_count_tot"] = records[i].rating_count_tot;
            response["records"][i]["rating_count_ver"] = records[i].rating_count_ver;
            response["records"][i]["user_rating"] = records[i].user_rating;
            response["records"][i]["user_rating_ver"] = records[i].user_rating_ver;
            response["records"][i]["ver"] = records[i].ver;
            response["records"][i]["cont_rating"] = records[i].cont_rating;
            response["records"][i]["prime_genre"] = records[i].prime_genre;
            response["records"][i]["sup_devices_num"] = records[i].sup_devices_num;
            response["records"][i]["ipadSc_urls_num"] = records[i].ipadSc_urls_num;
            response["records"][i]["lang_num"] = records[i].lang_num;
            response["records"][i]["vpp_lic"] = records[i].vpp_lic;
        }

        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/select").methods("POST"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }
        if (!isCreated) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Table not created";
            return crow::response(response);
        }

        if(!data.has("search")) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "No search item provided";
            return crow::response(response);
        }

        searchOne = data["search"].u();

        vector<Record<ll>> records;

        if (indexType == "ISAM") {
            //search ISAM
        }
        else if (indexType == "HASH") {
            Record<ll> record = eh.search(searchOne);
            if(record.id == 0) {
                crow::json::wvalue response;
                response["status"] = "error";
                response["message"] = "Record not found";
                return crow::response(response);
            }
            records.push_back(record);
        }
        else {
            records = avlfile.rangeSearch(searchOne, searchOne);
            if (records.empty()) {
                crow::json::wvalue response;
                response["status"] = "error";
                response["message"] = "Record not found";
                return crow::response(response);
            }
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record selected successfully";
        response["record"]["id"] = records[0].id;
        response["record"]["track_name"] = records[0].track_name;
        response["record"]["size_bytes"] = records[0].size_bytes;
        response["record"]["currency"] = records[0].currency;
        response["record"]["price"] = records[0].price;
        response["record"]["rating_count_tot"] = records[0].rating_count_tot;
        response["record"]["rating_count_ver"] = records[0].rating_count_ver;
        response["record"]["user_rating"] = records[0].user_rating;
        response["record"]["user_rating_ver"] = records[0].user_rating_ver;
        response["record"]["ver"] = records[0].ver;
        response["record"]["cont_rating"] = records[0].cont_rating;
        response["record"]["prime_genre"] = records[0].prime_genre;
        response["record"]["sup_devices_num"] = records[0].sup_devices_num;
        response["record"]["ipadSc_urls_num"] = records[0].ipadSc_urls_num;
        response["record"]["lang_num"] = records[0].lang_num;
        response["record"]["vpp_lic"] = records[0].vpp_lic;

        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/range").methods("POST"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }

        if (!isCreated) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Table not created";
            return crow::response(response);
        }

        if(!data.has("r1") || !data.has("r2")) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "No items for search range provided";
            return crow::response(response);
        }

        searchOne = data["r1"].u();
        searchTwo = data["r2"].u();

        vector<Record<ll>> records;

        if (indexType == "ISAM") {
            //search ISAM
        }
        else if (indexType == "HASH") {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Hash index does not support range search";
            return crow::response(response);
        }
        else {
            records = avlfile.rangeSearch(searchOne, searchTwo);
            if (records.empty()) {
                crow::json::wvalue response;
                response["status"] = "error";
                response["message"] = "Records not found in search range";
                return crow::response(response);
            }
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Search range selected successfully";
        for (int i = 0; i < records.size(); i++) {
            response["records"][i]["id"] = records[i].id;
            response["records"][i]["track_name"] = records[i].track_name;
            response["records"][i]["size_bytes"] = records[i].size_bytes;
            response["records"][i]["currency"] = records[i].currency;
            response["records"][i]["price"] = records[i].price;
            response["records"][i]["rating_count_tot"] = records[i].rating_count_tot;
            response["records"][i]["rating_count_ver"] = records[i].rating_count_ver;
            response["records"][i]["user_rating"] = records[i].user_rating;
            response["records"][i]["user_rating_ver"] = records[i].user_rating_ver;
            response["records"][i]["ver"] = records[i].ver;
            response["records"][i]["cont_rating"] = records[i].cont_rating;
            response["records"][i]["prime_genre"] = records[i].prime_genre;
            response["records"][i]["sup_devices_num"] = records[i].sup_devices_num;
            response["records"][i]["ipadSc_urls_num"] = records[i].ipadSc_urls_num;
            response["records"][i]["lang_num"] = records[i].lang_num;
            response["records"][i]["vpp_lic"] = records[i].vpp_lic;
        }

        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/insert").methods("POST"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        vector<Record<ll>> records;
        if (!data) {
            return crow::response(400, "Bad request");
        }
        if(!data.has("record")) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "No record provided";
            return crow::response(response);
        }

        if (!isCreated) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Table not created";
            return crow::response(response);
        }

        Record<ll> record;

        record.id = data["record"]["id"].u();

        string track_name = data["record"]["track_name"].s();
        strncpy(record.track_name, track_name.c_str(), track_name.size());

        record.size_bytes = data["record"]["size_bytes"].u();

        string currency = data["record"]["currency"].s();
        strncpy(record.currency, currency.c_str(), currency.size());

        record.price = data["record"]["price"].d();
        record.rating_count_tot = data["record"]["rating_count_tot"].u();
        record.rating_count_ver = data["record"]["rating_count_ver"].u();
        record.user_rating = data["record"]["user_rating"].d();
        record.user_rating_ver = data["record"]["user_rating_ver"].d();

        string ver = data["record"]["ver"].s();
        strncpy(record.ver, ver.c_str(), ver.size());

        string cont_rating = data["record"]["cont_rating"].s();
        strncpy(record.cont_rating, cont_rating.c_str(), cont_rating.size());

        string prime_genre = data["record"]["prime_genre"].s();
        strncpy(record.prime_genre, prime_genre.c_str(), prime_genre.size());

        record.sup_devices_num = data["record"]["sup_devices_num"].i();
        record.ipadSc_urls_num = data["record"]["ipadSc_urls_num"].i();
        record.lang_num = data["record"]["lang_num"].i();
        record.vpp_lic = data["record"]["vpp_lic"].i();

        if (indexType == "ISAM") {
            //insert ISAM
        }
        else if (indexType == "HASH") {
            eh.insert(record);
            records = eh.get_all();
        }
        else {
            avlfile.insert(record);
            records = avlfile.inorder();
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record inserted successfully";
        response["id"] = record.id;
        for (int i = 0; i < records.size(); i++) {
            response["records"][i]["id"] = records[i].id;
            response["records"][i]["track_name"] = records[i].track_name;
            response["records"][i]["size_bytes"] = records[i].size_bytes;
            response["records"][i]["currency"] = records[i].currency;
            response["records"][i]["price"] = records[i].price;
            response["records"][i]["rating_count_tot"] = records[i].rating_count_tot;
            response["records"][i]["rating_count_ver"] = records[i].rating_count_ver;
            response["records"][i]["user_rating"] = records[i].user_rating;
            response["records"][i]["user_rating_ver"] = records[i].user_rating_ver;
            response["records"][i]["ver"] = records[i].ver;
            response["records"][i]["cont_rating"] = records[i].cont_rating;
            response["records"][i]["prime_genre"] = records[i].prime_genre;
            response["records"][i]["sup_devices_num"] = records[i].sup_devices_num;
            response["records"][i]["ipadSc_urls_num"] = records[i].ipadSc_urls_num;
            response["records"][i]["lang_num"] = records[i].lang_num;
            response["records"][i]["vpp_lic"] = records[i].vpp_lic;
        }
        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/delete").methods("DELETE"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        vector<Record<ll>> records;
        if (!data) {
            return crow::response(400, "Bad request");
        }
        if(!data.has("id")) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "No id provided";
            return crow::response(response);
        }

        if (!isCreated) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Table not created";
            return crow::response(response);
        }

        if (indexType == "ISAM") {
            //delete ISAM
        }
        else if (indexType == "HASH") {
            //delete HASH
        }
        else {
            bool res = avlfile.remove(data["id"].u());
            if (!res) {
                crow::json::wvalue response;
                response["status"] = "error";
                response["message"] = "Record not found. Cannot delete";
                return crow::response(response);
            }
            records = avlfile.inorder();
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record deleted successfully";
        for (int i = 0; i < records.size(); i++) {
            response["records"][i]["id"] = records[i].id;
            response["records"][i]["track_name"] = records[i].track_name;
            response["records"][i]["size_bytes"] = records[i].size_bytes;
            response["records"][i]["currency"] = records[i].currency;
            response["records"][i]["price"] = records[i].price;
            response["records"][i]["rating_count_tot"] = records[i].rating_count_tot;
            response["records"][i]["rating_count_ver"] = records[i].rating_count_ver;
            response["records"][i]["user_rating"] = records[i].user_rating;
            response["records"][i]["user_rating_ver"] = records[i].user_rating_ver;
            response["records"][i]["ver"] = records[i].ver;
            response["records"][i]["cont_rating"] = records[i].cont_rating;
            response["records"][i]["prime_genre"] = records[i].prime_genre;
            response["records"][i]["sup_devices_num"] = records[i].sup_devices_num;
            response["records"][i]["ipadSc_urls_num"] = records[i].ipadSc_urls_num;
            response["records"][i]["lang_num"] = records[i].lang_num;
            response["records"][i]["vpp_lic"] = records[i].vpp_lic;
        }
        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/get_all").methods("GET"_method)
    ([this](const crow::request &req) {
        vector<Record<ll>> records;

        if (!isCreated) {
            crow::json::wvalue response;
            response["status"] = "error";
            response["message"] = "Table not created";
            return crow::response(response);
        }

        if (indexType == "ISAM") {
            // get all ISAM
        }
        else if (indexType == "HASH") {
            records = eh.get_all();
        }
        else
        {
            records = avlfile.inorder();
        }


        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Records updated successfully";

        for (int i = 0; i < records.size(); i++) {
            response["records"][i]["id"] = records[i].id;
            response["records"][i]["track_name"] = records[i].track_name;
            response["records"][i]["size_bytes"] = records[i].size_bytes;
            response["records"][i]["currency"] = records[i].currency;
            response["records"][i]["price"] = records[i].price;
            response["records"][i]["rating_count_tot"] = records[i].rating_count_tot;
            response["records"][i]["rating_count_ver"] = records[i].rating_count_ver;
            response["records"][i]["user_rating"] = records[i].user_rating;
            response["records"][i]["user_rating_ver"] = records[i].user_rating_ver;
            response["records"][i]["ver"] = records[i].ver;
            response["records"][i]["cont_rating"] = records[i].cont_rating;
            response["records"][i]["prime_genre"] = records[i].prime_genre;
            response["records"][i]["sup_devices_num"] = records[i].sup_devices_num;
            response["records"][i]["ipadSc_urls_num"] = records[i].ipadSc_urls_num;
            response["records"][i]["lang_num"] = records[i].lang_num;
            response["records"][i]["vpp_lic"] = records[i].vpp_lic;
        }

        return crow::response(response);
    });
}

inline void Api::run() {
    app.port(5001).multithreaded().run();
}

#endif //API_H
