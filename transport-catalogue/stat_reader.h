#pragma once

#include "transport_catalogue.h"

#include <vector>
#include <iostream>

using global::transport_catalogue::TransportCatalogue;

namespace global::stat_reader {
    void GetDataBaseInfo(std::istream& input, TransportCatalogue& transport_catalogue, std::ostream& out = std::cout);
}