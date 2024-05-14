#ifndef API_H
#define API_H

#include "crow.h"
#include "crow/middlewares/cors.h"
#include <iostream>

using namespace std;

class Api {
    public:
        Api();
        void run();
        ~Api() = default;
    private:
        crow::App<crow::CORSHandler> app;

};

inline Api::Api() {
    auto cors = app.get_middleware<crow::CORSHandler>();

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
    ([this](const crow::request& req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Table created successfully";

        return crow::response(response);
    });
    CROW_ROUTE(app, "/api/v1/select").methods("GET"_method)
    ([this](const crow::request& req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Record selected successfully";

        return crow::response(response);
    });
    CROW_ROUTE(app, "/api/v1/range").methods("GET"_method)
    ([this](const crow::request& req) {
        auto data = crow::json::load(req.body);
        if (!data) {
            return crow::response(400, "Bad request");
        }

        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Search range selected successfully";

        return crow::response(response);
    });
    CROW_ROUTE(app, "/api/v1/insert").methods("POST"_method)
    ([this](const crow::request& req) {
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
    ([this](const crow::request& req) {
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
