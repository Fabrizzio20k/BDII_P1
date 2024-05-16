#ifndef API_H
#define API_H
#define ll long long

#include "crow.h"
#include "crow/middlewares/cors.h"
#include <iostream>
#include "avlfile.cpp"
#include "appleRecord.h"

using namespace std;

class Api {
public:
    Api();

    void run();

    ~Api() = default;

private:
    crow::App<crow::CORSHandler> app;
    AVLFile<ll> avlfile = AVLFile<ll>("data.dat");
    bool isCreated = false;
    string tableName;
    string columnIndex;
    string indexType;
    string fileName;
    ll seachOne{};
    ll searchTwo{};
};

inline Api::Api() {
    auto cors = app.get_middleware<crow::CORSHandler>();
    avlfile.clear();
    isCreated = false;

    cors
            .global()
            .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
            .methods("POST"_method, "GET"_method)
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
        int i = 0;
        auto data = crow::json::load(req.body);
        //verify if field file_name, table_name, index and column_index are present
        if (!data || !data.has("file_name") || !data.has("table_name") || !data.has("index") || !data.has(
                "column_index")) {
            return crow::response(400, "No file_name, table_name, index or column_index provided");
        }

        if (data["file_name"] != "appleStore.csv") {
            return crow::response(400, "File does not exist");
        }

        if (data["index"] != "ISAM" && data["index"] != "HASH" && data["index"] != "AVL") {
            return crow::response(400, "Index type not supported");
        }

        if (data["column_index"] != "id") {
            return crow::response(400, "Column index not supported");
        }

        if (data["index"] == "ISAM") {
            indexType = "ISAM";
            cout << "Creating ISAM index" << endl;
        }
        else if (data["index"] == "HASH") {
            indexType = "HASH";
            cout << "Creating HASH index" << endl;
        }
        else
        {
            indexType = "AVL";
            string path = __FILE__;
            path = path.substr(0, path.find_last_of("\\/"));
            path = path.substr(0, path.find_last_of("\\/"));
            path = path + "/dataStructures/dbs/AppleStore.csv";

            ifstream file(path);

            avlfile.clear();

            if (!file.is_open()) {
                cerr << "No se pudo abrir el archivo" << endl;
            }

            Record<ll> record;

            string line;
            getline(file, line);

            i = 0;
            while (file >> record && file.peek() != EOF) {
                avlfile.insert(record);
                i++;
            }
            file.close();
        }

        isCreated = true;
        tableName = string{data["table_name"]};
        columnIndex = string{data["column_index"]};
        fileName = string{data["file_name"]};

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Table created successfully";
        response["total_records"] = i;
        response["index_type"] = indexType;
        response["table_name"] = tableName;
        response["column_index"] = columnIndex;
        response["file_name"] = fileName;

        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/select").methods("GET"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }
        if (!isCreated) {
            return crow::response(400, "Table not created");
        }

        if(!data.has("search")) {
            return crow::response(400, "No search key provided");
        }

        seachOne = data["search"].u();


        Record<ll> record;

        if (indexType == "ISAM") {
            //search ISAM
        }
        else if (indexType == "HASH") {
            //search HASH
        }
        else {
            const vector<Record<ll>> rec = avlfile.search(seachOne);
            if (rec.empty()) {
                return crow::response(404, "Record not found");
            }
            record = rec[0];
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record selected successfully";
        response["record"]["id"] = record.id;
        response["record"]["track_name"] = record.track_name;
        response["record"]["size_bytes"] = record.size_bytes;
        response["record"]["currency"] = record.currency;
        response["record"]["price"] = record.price;
        response["record"]["rating_count_tot"] = record.rating_count_tot;
        response["record"]["rating_count_ver"] = record.rating_count_ver;
        response["record"]["user_rating"] = record.user_rating;
        response["record"]["user_rating_ver"] = record.user_rating_ver;
        response["record"]["ver"] = record.ver;
        response["record"]["cont_rating"] = record.cont_rating;
        response["record"]["prime_genre"] = record.prime_genre;
        response["record"]["sup_devices_num"] = record.sup_devices_num;
        response["record"]["ipadSc_urls_num"] = record.ipadSc_urls_num;
        response["record"]["lang_num"] = record.lang_num;
        response["record"]["vpp_lic"] = record.vpp_lic;

        return crow::response(response);
    });

    CROW_ROUTE(app, "/api/v1/range").methods("GET"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }

        if (!isCreated) {
            return crow::response(400, "Table not created");
        }

        if(!data.has("r1") || !data.has("r2")) {
            return crow::response(400, "No complete aitems for search range provided");
        }

        seachOne = data["r1"].u();
        searchTwo = data["r2"].u();

        vector<Record<ll>> records;

        if (indexType == "ISAM") {
            //search ISAM
        }
        else if (indexType == "HASH") {
            return crow::response(400, "Search range not supported for HASH index");
        }
        else {
            cout << "Searching range " << seachOne << " to " << searchTwo << endl;
            records = avlfile.rangeSearch(seachOne, searchTwo);
            if (records.empty()) {
                return crow::response(404, "Records between range not found");
            }
            cout << "Records found: " << records.size() << endl;
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
        if (!data) {
            return crow::response(400, "Bad request");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record inserted successfully";
        return crow::response(response);
    });
    CROW_ROUTE(app, "/api/v1/delete").methods("POST"_method)
    ([this](const crow::request &req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record deleted successfully";
        return crow::response(response);
    });
}

inline void Api::run() {
    app.port(5001).multithreaded().run();
}

#endif //API_H
