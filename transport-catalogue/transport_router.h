#pragma once

#include "router.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <deque>
#include <iostream>
#include <unordered_map>

namespace transport_catalogue::router {

    struct StopEdge {
        std::string_view name;
        double time = 0;
    };

    struct BusEdge {
        std::string_view bus_name;
        size_t span_count = 0;
        double time = 0;
    };

    struct RoutingSettings {
        double bus_wait_time = 0;
        double bus_velocity = 0;
    };

    struct RouterByStop {
        graph::VertexId bus_wait_start;
        graph::VertexId bus_wait_end;
    };

    struct RouteInfo {
        double total_time = 0.;
        std::vector<std::variant<StopEdge, BusEdge>> edges;
    };

    static const uint16_t KILOMETER = 1000;
    static const uint16_t HOUR = 60;

    class TransportRouter {
    public:
        void SetRoutingSettings(RoutingSettings routing_settings);

        void BuildRoute(TransportCatalogue &transport_catalogue);

        std::optional<RouterByStop> GetRouterByStop(Stop *stop) const;

        std::optional<RouteInfo> GetRouteInfo(std::size_t start, std::size_t end) const;

    private:
        const RoutingSettings &GetRoutingSettings() const;

        const graph::DirectedWeightedGraph<double> &GetGraph() const;

        const graph::Router<double> &GetRouter() const;

        const std::variant<StopEdge, BusEdge> &GetEdge(graph::EdgeId id) const;

        const std::unordered_map<Stop *, RouterByStop> &GetStopToVertex() const;

        const std::unordered_map<graph::EdgeId, std::variant<StopEdge, BusEdge>> &GetEdgeIdToEdge() const;

        std::deque<Stop *> GetStopsPrt(TransportCatalogue &transport_catalogue);

        std::deque<Bus *> GetBusPrt(TransportCatalogue &transport_catalogue);

        void AddEdgeToStop();

        void AddEdgeToBus(TransportCatalogue &transport_catalogue);

        void SetStops(const std::deque<Stop *> &stops);

        void SetGraph(TransportCatalogue &transport_catalogue);

        graph::Edge<double> MakeEdgeToBus(Stop *start, Stop *end, const double distance) const;

        template<typename Iterator>
        void ParseBusToEdges(Iterator first,
                             Iterator last,
                             const TransportCatalogue &transport_catalogue,
                             const Bus *bus);

        std::unordered_map<Stop *, RouterByStop> stop_to_router_;
        std::unordered_map<graph::EdgeId, std::variant<StopEdge, BusEdge>> edge_id_to_edge_;

        std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
        std::unique_ptr<graph::Router<double>> router_;

        RoutingSettings routing_settings_;
    };

    template<typename Iterator>
    void TransportRouter::ParseBusToEdges(Iterator first,
                                          Iterator last,
                                          const TransportCatalogue &transport_catalogue,
                                          const Bus *bus) {

        for (auto it = first; it != last; ++it) {
            size_t distance = 0;
            size_t span = 0;

            for (auto it2 = std::next(it); it2 != last; ++it2) {
                distance += transport_catalogue.GetDistanceBetweenStops(*prev(it2), *it2);
                ++span;

                graph::EdgeId id = graph_->AddEdge(MakeEdgeToBus(*it, *it2, distance));

                edge_id_to_edge_[id] = BusEdge{bus->name, span, graph_->GetEdge(id).weight};
            }
        }
    }

}