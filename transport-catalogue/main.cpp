#include "stat_reader.h"
#include "input_reader.h"
#include "transport_catalogue.h"

#include <iostream>

int main() {
    transport_catalogue_global_namespace::transport_catalogue::TransportCatalogue transport_catalogue;
    transport_catalogue_global_namespace::input_reader::ParseQuery(std::cin, transport_catalogue);
    transport_catalogue_global_namespace::stat_reader::GetDataBaseInfo(std::cin, transport_catalogue, std::cout);
    return 0;
}