#include "SQLParser.h"
#include <iostream>

using namespace std;

int main()
{
  // Basic Usage Example

  //Query for range search
  string query = "SELECT * FROM test WHERE id BETWEEN 10 AND 20;";

  hsql::SQLParserResult result;
  hsql::SQLParser::parse(query, &result);

  //PRINT ALL THE PARSED STATEMENTS
  if (result.isValid())
  {
        cout << "Parsed successfully!" << endl;
        cout << "Number of statements: " << result.size() << endl;
  // Verify if it is a SELECT statement
        if (result.getStatement(0)->type() == hsql::StatementType::kStmtSelect)
        {
            cout << "It is a SELECT statement" << endl;
        }
        else
        {
            cout << "It is not a SELECT statement" << endl;
        }
  }
  else
  {
        cout << "Given string is not a valid SQL query." << endl;
        cout << result.errorMsg() << " (L" << result.errorLine() << ": " << result.errorColumn() << ")" << endl;
  }
}