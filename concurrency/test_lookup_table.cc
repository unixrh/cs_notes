#include "threadsafe_lookup_table.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {
    std::unique_ptr<threadsafe_lookup_table<string, string>> l;
    l.reset(new threadsafe_lookup_table<string, string>());
    l->add_or_update_mapping("apple", "a");
    cout << "value: " << l->value_for("apple") << endl;
    return 0;
}
