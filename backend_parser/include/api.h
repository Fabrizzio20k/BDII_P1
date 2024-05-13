#ifndef API_H
#define API_H

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "parser.h"
#include <iostream>

using namespace std;

class Api {
    public:
        Api();
        void run();
        ~Api() = default;
    private:
        crow::App<crow::CORSHandler> app;
        SQLParser parser;
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
        return crow::response(200, "Hello, Worldd!");
    });

    CROW_ROUTE(app, "/api/v1/parser").methods("POST"_method)
    ([this](const crow::request& req) {
        auto consulta = crow::json::load(req.body);
        if (!consulta) {
            return crow::response(400, "Bad request");
        }

        crow::json::wvalue response;
        string command = consulta["command"].s();

        bool isParsed = parser.parse(command);

        if (isParsed) {
            response["status"] = "ok";
            response["message"] = "Command parsed successfully";
            response["command"] = parser.getType();
            response["table"] = parser.getTable();
            response["condition"] = parser.getCondition();
            response["route"] = parser.getFileRoute();
            response["indexType"] = parser.getIndexType();
            response["indexColumn"] = parser.getIndexColumn();
            response["r1"] = parser.getR1();
            response["r2"] = parser.getR2();
            // return also the values in case of INSERTAR
            if (parser.getType() == "INSERTAR") {
                response["values"] = parser.getValues();
            }
        } else {
            response["status"] = "error";
            response["message"] = parser.getCommand().errorMessage;
        }
        return crow::response(response);
    });
}

inline void Api::run() {
    app.port(5000).multithreaded().run();
}

#endif //API_H
