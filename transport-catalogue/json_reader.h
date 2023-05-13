#pragma once

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace json {

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

        void ParseNode(const Node &root, transport_catalogue::TransportCatalogue &catalogue, std::vector<StatRequest> &stat_request,
                       map_renderer::RenderSettings &render_settings);

        void Parse(transport_catalogue::TransportCatalogue &catalogue, std::vector<StatRequest> &stat_request,
                   map_renderer::RenderSettings &render_settings);

        Stop ParseNodeStop(Node &node);

        Bus ParseNodeBus(Node &node, transport_catalogue::TransportCatalogue &catalogue);

        std::vector<Distance> ParseNodeDistances(Node &node, transport_catalogue::TransportCatalogue &catalogue);

        BusQueryResult BusQuery(transport_catalogue::TransportCatalogue &catalogue, std::string_view str);

        StopQueryResult StopQuery(transport_catalogue::TransportCatalogue &catalogue, std::string_view stop_name);

        Node ExecuteMakeNodeStop(int id_request, StopQueryResult query_result);

        Node ExecuteMakeNodeBus(int id_request, BusQueryResult query_result);

        Node ExecuteMakeNodeMap(int id_request, transport_catalogue::TransportCatalogue &catalogue, map_renderer::RenderSettings render_settings);

        void ExecuteQueries(transport_catalogue::TransportCatalogue &catalogue, std::vector<StatRequest> &stat_requests,
                            map_renderer::RenderSettings &render_settings);

        void ExecuteRenderMap(map_renderer::MapRenderer &map_catalogue, transport_catalogue::TransportCatalogue &catalogue_) const;

        const Document &GetDocument() const;

    private:
        Document document_;
        Document doc_out;
    };
}