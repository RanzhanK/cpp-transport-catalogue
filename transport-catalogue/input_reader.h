#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <vector>
#include <iostream>

using global::transport_catalogue::TransportCatalogue;

namespace global::input_reader {
    void CutSpaces(std::istringstream& stream);

    void ParseQuery(std::istream& input, TransportCatalogue& transport_catalogue);

    void ReadStopWithoutDistances(std::istringstream& stream, TransportCatalogue& transport_catalogue);

    void ReadStopDistances(std::istringstream& stream, TransportCatalogue& transport_catalogue);

    void ReadBus(std::istringstream& stream, TransportCatalogue& transport_catalogue);
}