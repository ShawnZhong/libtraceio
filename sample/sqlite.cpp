#include <sqlite3.h>

#include <iostream>

static void exec_sql(sqlite3 *db, const char *sql) {
  int rc = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    std::cerr << "SQL: " << sql << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main() {
  system("rm -f test.db");

  sqlite3 *db;
  if (auto rc = sqlite3_open("test.db", &db); rc != SQLITE_OK) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    return EXIT_FAILURE;
  }

  exec_sql(db, "CREATE TABLE Cars(Id INT, Name TEXT, Price INT);");
  exec_sql(db, "INSERT INTO Cars VALUES(1, 'Audi', 52642);");

  sqlite3_close(db);
}
