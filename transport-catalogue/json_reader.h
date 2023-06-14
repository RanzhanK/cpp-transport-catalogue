#pragma once

#include "map_renderer.h"
#include "json_builder.h"
#include "request_handler.h"
#include "transport_router.h"
#include "transport_catalogue.h"

namespace json {

    struct StatRequest {
        int id;
        std::string type;
        std::string name;
        std::string from;
        std::string to;
    };

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

    class JSONReader {
    public:
        JSONReader() = default;

        explicit JSONReader(Document doc);

        explicit JSONReader(std::istream &input);

        void ParseNodeBase(const Node &root, transport_catalogue::TransportCatalogue &catalogue);

        void ParseNodeStat(const Node &root, std::vector<StatRequest> &stat_request);

        void ParseNodeRender(const Node &node, map_renderer::RenderSettings &render_settings);

        void ParseNodeRouting(const Node &node, transport_catalogue::router::RoutingSettings &route_set);

        void ParseNode(const Node &root, transport_catalogue::TransportCatalogue &catalogue,
                       std::vector<StatRequest> &stat_request,
                       map_renderer::RenderSettings &render_settings, transport_catalogue::router::RoutingSettings &routing_settings);

        void Parse(transport_catalogue::TransportCatalogue &catalogue, std::vector<StatRequest> &stat_request,
                   map_renderer::RenderSettings &render_settings, transport_catalogue::router::RoutingSettings &routing_settings);

        Stop ParseNodeStop(Node &node);

        Bus ParseNodeBus(Node &node, transport_catalogue::TransportCatalogue &catalogue);

        std::vector<Distance> ParseNodeDistances(Node &node, transport_catalogue::TransportCatalogue &catalogue);

        void ExecuteQueries(transport_catalogue::TransportCatalogue &catalogue, std::vector<StatRequest> &stat_requests,
                            map_renderer::RenderSettings &render_settings, transport_catalogue::router::RoutingSettings &routing_settings);

        const Document &GetDocument() const;

    private:
        BusQueryResult BusQuery(transport_catalogue::TransportCatalogue &catalogue, std::string_view str);

        StopQueryResult StopQuery(transport_catalogue::TransportCatalogue &catalogue, std::string_view stop_name);

        Node ExecuteMakeNodeStop(int id_request, const StopQueryResult &query_result);

        Node ExecuteMakeNodeBus(int id_request, const BusQueryResult &query_result);

        Node ExecuteMakeNodeMap(int id_request, transport_catalogue::TransportCatalogue &catalogue,
                                const map_renderer::RenderSettings& render_settings);

        Node ExecuteMakeNodeRoute(StatRequest &request,
                                  TransportCatalogue &catalogue,
                                  transport_catalogue::router::TransportRouter &routing);

        void ExecuteRenderMap(map_renderer::MapRenderer &map_catalogue,
                              transport_catalogue::TransportCatalogue &catalogue_) const;

        Document document_;

        Document doc_out;
    };
}