#ifndef SQLPARSER_H
#define SQLPARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <regex>

using namespace std;

enum class CommandType
{
    CREATE,
    SELECT,
    RANGE,
    INSERTAR,
    BORRAR,
    UNKNOWN
};

struct Command
{
    CommandType type = CommandType::UNKNOWN;
    string table;
    string condition;
    string route;
    string indexType;
    string indexColumn;
    string r1, r2;
    string errorMessage;
    vector<string> values = {};
};

class SQLParser
{
private:
    string command;
    Command cmd;
    bool parseCommand()
    {
        regex createRegex(R"(create table (\w+) from file \"(.+)\" using (HASH|ISAM|AVL) INDEX \(\"(\w+)\"\))");
        regex selectRegex(R"(select \* from (\w+) where (\w+) = (.+))");
        regex rangeRegex(R"(select \* from (\w+) where (\w+) between (.+) and (.+))");
        regex insertRegex(R"(insert into (\w+) values \((.+)\))");
        regex deleteRegex(R"(delete from (\w+) where (\w+) = (.+))");

        smatch matches;

        if (regex_search(command, matches, createRegex))
        {
            cmd.type = CommandType::CREATE;
            cmd.table = matches[1];
            cmd.route = matches[2];
            cmd.indexType = matches[3];
            cmd.indexColumn = matches[4];
            return true;
        }
        if (regex_search(command, matches, selectRegex))
        {
            cmd.type = CommandType::SELECT;
            if (cmd.table != matches[1])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Table name in select command does not match the table name in the command";
                return false;
            }
            if (cmd.indexColumn != matches[2])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Index column in select command does not match the index column in the command";
                return false;
            }
            cmd.condition = matches[3];
            cmd.errorMessage.clear();
            return true;
        }
        if (regex_search(command, matches, rangeRegex))
        {
            cmd.type = CommandType::RANGE;
            if (cmd.table != matches[1])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Table name in select range command does not match the table name in the command";
                return false;
            }
            if (cmd.indexColumn != matches[2])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Index column in select range command does not match the index column in the command";
                return false;
            }
            cmd.r1 = matches[3];
            cmd.r2 = matches[4];
            return true;
        }
        if (regex_search(command, matches, insertRegex))
        {
            cmd.type = CommandType::INSERTAR;
            if (cmd.table != matches[1])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Table name in insert command does not match the table name in the command";
                return false;
            }
            string values = matches[2];
            string value;
            for (char c : values)
            {
                if (c == ',')
                {
                    cmd.values.push_back(value);
                    value.clear();
                }
                else
                {
                    value += c;
                }
            }
            cmd.values.push_back(value);
            return true;
        }
        if (regex_search(command, matches, deleteRegex))
        {
            cmd.type = CommandType::BORRAR;
            if (cmd.table != matches[1])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Table name in delete command does not match the table name in the command";
                return false;
            }
            if (cmd.indexColumn != matches[2])
            {
                cmd.type = CommandType::UNKNOWN;
                cmd.errorMessage = "Index column in delete command does not match the index column in the command";
                return false;
            }

            cmd.condition = matches[3];
            return true;
        }

        cmd.type = CommandType::UNKNOWN;
        cmd.errorMessage = "Invalid command";
        return false;
    }

public:
    SQLParser() {}

    Command getCommand()
    {
        return cmd;
    }

    bool parse(const string &command)
    {
        this->command = command;
        return parseCommand();
    }

    string getType()
    {
        switch (cmd.type)
        {
        case CommandType::CREATE:
            return "CREATE";
        case CommandType::SELECT:
            return "SELECT";
        case CommandType::RANGE:
            return "RANGE";
        case CommandType::INSERTAR:
            return "INSERTAR";
        case CommandType::BORRAR:
            return "BORRAR";
        default:
            return "UNKNOWN";
        }
    }

    void printSentence()
    {
        switch (cmd.type)
        {
        case CommandType::CREATE:
            std::cout << "Creating table " << cmd.table << " from file " << cmd.route << " using " << cmd.indexType << " (" << cmd.indexColumn << ")\n";
            break;
        case CommandType::SELECT:
            std::cout << "Selecting from table " << cmd.table << " where " << cmd.indexColumn << " = " << cmd.condition << "\n";
            break;
        case CommandType::RANGE:
            std::cout << "Selecting from table " << cmd.table << " where " << cmd.indexColumn << " between " << cmd.r1 << " and " << cmd.r2 << "\n";
            break;
        case CommandType::INSERTAR:
            std::cout << "Inserting into table " << cmd.table << " values (";
            for (int i = 0; i < cmd.values.size(); i++)
            {
                std::cout << cmd.values[i];
                if (i < cmd.values.size() - 1)
                {
                    std::cout << ", ";
                }
            }
            std::cout << ")\n";
            break;
        case CommandType::BORRAR:
            std::cout << "Deleting from table " << cmd.table << " where " << cmd.indexColumn << " = " << cmd.condition << "\n";
            break;
        default:
            std::cout << "Unknown command\n";
        }
    }
};

#endif // !SQLPARSER_H