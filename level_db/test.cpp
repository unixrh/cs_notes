#include <iostream>
#include <cassert>
#include "leveldb/db.h"

int32_t main(int32_t argc, char* argv[]) {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    // options.error_if_exists = true;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());
    std::string key1 = "a";
    std::string key2 = "b";
    std::string value = "3";
    std::string val;
    leveldb::Status s1 = db->Put(leveldb::WriteOptions(), key1, value);
    leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &val);
    if (s.ok()) std::cout << key1 << ": " << val;
    if (s.ok()) s = db->Put(leveldb::WriteOptions(), key2, value);
    else std::cout << "error get " << key1 << std::endl;
    if (s.ok()) s = db->Delete(leveldb::WriteOptions(), key1);
    else std::cout << "error get " << key2 << std::endl;
    return 0;
}
