#pragma once

#include "transport_catalogue.h"

#include <vector>
#include <iostream>

namespace transport_catalogue_global_namespace::stat_reader {
    void GetDataBaseInfo(std::istream& input, const transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out = std::cout);
    void GetBusesInfo(std::istream& input, const transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out);
    void GetStopsInfo(std::istream& input, const transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out);
}