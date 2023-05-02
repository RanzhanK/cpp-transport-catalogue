#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <vector>
#include <iostream>

namespace transport_catalogue_global_namespace::input_reader {
    void CutSpaces(std::istringstream& stream);

    void ParseQuery(std::istream& input, transport_catalogue::TransportCatalogue& transport_catalogue);
}