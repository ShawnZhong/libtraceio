#include <iostream>

#include "leveldb/db.h"

int main() {
  system("rm -rf testdb");

  leveldb::DB *db;
  leveldb::Options options;
  options.create_if_missing = true;

  auto status = leveldb::DB::Open(options, "testdb", &db);
  if (!status.ok()) {
    std::cerr << status.ToString() << std::endl;
    return EXIT_FAILURE;
  }

  std::string key = "key";
  std::string value = "value";
  db->Put(leveldb::WriteOptions(), key, value);
  db->Get(leveldb::ReadOptions(), key, &value);
  db->Delete(leveldb::WriteOptions(), key);

  delete db;
}
