#include "BlockHandler.h"
//#include "boost/json/src.hpp"
//#include "boost/json/object.hpp"
//#include "boost/json/array.hpp"
//#include "iostream"
//using namespace boost;
int main(){
    BlockHandler vm = BlockHandler();
    vm.run();

//    json::object object = json::parse(R"({"test":{"test123":1}})").as_object();
//    json::object object2 = object.at("test").as_object();
//    object2.emplace("test321", 2);
//    object2.emplace("test231", 4444);
//    object["test"] = object2;
//    std::cout << object;
}