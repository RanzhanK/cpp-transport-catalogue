#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace transport_catalogue::map_renderer;
using namespace transport_catalogue::json;
using namespace transport_catalogue;

namespace transport_catalogue::request_handler {

    class RequestHandler {
    public:
        static std::vector<geo::Coordinates> GetStopsCoordinates(TransportCatalogue &catalogue_);

        static std::vector<std::string_view> GetSortBusesNames(TransportCatalogue &catalogue_);

        RequestHandler() = default;

    private:
        Document doc_out;
    };
}