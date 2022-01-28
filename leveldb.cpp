#include <iostream>

#include "leveldb/db.h"
#include "trace.h"

int main(int argc, char **argv) {
  system("rm -rf testdb");

  leveldb::DB *db;
  leveldb::Options options;
  options.create_if_missing = true;

  auto status = leveldb::DB::Open(options, "testdb", &db);
  if (!status.ok()) {
    std::cerr << status.ToString() << std::endl;
    return EXIT_FAILURE;
  }

  leveldb::WriteOptions writeOptions;
  db->Put(writeOptions, "key", "value");

  delete db;
}
