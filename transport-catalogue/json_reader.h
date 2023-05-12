#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace json {

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

        const Document &GetDocument() const;

    private:
        Document document_;
    };
}