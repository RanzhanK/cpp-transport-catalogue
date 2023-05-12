#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace map_renderer;
using namespace transport_catalogue;
using namespace json;

namespace request_handler {

    struct BusQueryResult {
        std::string_view name;
        bool not_found;
        int stops_on_route;
        int unique_stops;
        int route_length;
        double curvature;
    };

    struct StopQueryResult {
        std::string_view name;
        bool not_found;
        std::vector<std::string> buses_name;
    };

    class RequestHandler {
    public:
        std::vector<geo::Coordinates> GetStopsCoordinates(TransportCatalogue &catalogue_) const;

        std::vector<std::string_view> GetSortBusesNames(TransportCatalogue &catalogue_) const;

        RequestHandler() = default;

        BusQueryResult BusQuery(TransportCatalogue &catalogue, std::string_view str);

        StopQueryResult StopQuery(TransportCatalogue &catalogue, std::string_view stop_name);

        Node ExecuteMakeNodeStop(int id_request, StopQueryResult query_result);

        Node ExecuteMakeNodeBus(int id_request, BusQueryResult query_result);

        Node ExecuteMakeNodeMap(int id_request, TransportCatalogue &catalogue, RenderSettings render_settings);

        void ExecuteQueries(TransportCatalogue &catalogue, std::vector<StatRequest> &stat_requests,
                            RenderSettings &render_settings);

        void ExecuteRenderMap(MapRenderer &map_catalogue, TransportCatalogue &catalogue_) const;

        const Document &GetDocument();

    private:
        Document doc_out;
    };
}