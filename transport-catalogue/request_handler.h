#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace map_renderer;
using namespace transport_catalogue;
using namespace json;

namespace request_handler {

    class RequestHandler {
    public:
        static std::vector<geo::Coordinates> GetStopsCoordinates(TransportCatalogue &catalogue_);

        static std::vector<std::string_view> GetSortBusesNames(TransportCatalogue &catalogue_);

        RequestHandler() = default;

    private:
        Document doc_out;
    };
}