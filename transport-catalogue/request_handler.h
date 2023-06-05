#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "transport_catalogue.h"

using namespace transport_catalogue::map_renderer;
using namespace transport_catalogue;
using namespace json;

namespace transport_catalogue::request_handler {

    class RequestHandler {
    public:
        static std::optional<RouteInfo> GetRouteInfo(std::string_view start,
                                                     std::string_view end,
                                                     TransportCatalogue &catalogue,
                                                     transport_catalogue::detail::router::TransportRouter &routing);

        static std::vector<geo::Coordinates> GetStopsCoordinates(TransportCatalogue &catalogue_);

        static std::vector<std::string_view> GetSortBusesNames(TransportCatalogue &catalogue_);

        RequestHandler() = default;

    private:
        Document doc_out;
    };
}