#include "stat_reader.h"
#include "input_reader.h"
#include "transport_catalogue.h"

#include <iostream>

namespace global {
    namespace geo {}
    namespace input_reader {}
    namespace transport_catalogue {}
    namespace stat_reader {}
}

int main() {
    global::transport_catalogue::TransportCatalogue transport_catalogue;
    global::input_reader::ParseQuery(std::cin, transport_catalogue);
    global::stat_reader::GetDataBaseInfo(std::cin, transport_catalogue, std::cout);
    return 0;
}