#include "transport_router.h"

namespace transport_catalogue::router {

    void TransportRouter::SetRoutingSettings(RoutingSettings routing_settings) {
        routing_settings_ = std::move(routing_settings);
    }

    const RoutingSettings &TransportRouter::GetRoutingSettings() const { return routing_settings_; }

    void TransportRouter::BuildRoute(TransportCatalogue &transport_catalogue) {
        SetGraph(transport_catalogue);
        router_ = std::make_unique<graph::Router<double>>
                (*graph_);
        router_->build();
    }

    const graph::DirectedWeightedGraph<double> &TransportRouter::GetGraph() const { return *graph_; }

    const graph::Router<double> &TransportRouter::GetRouter() const { return *router_; }

    const std::variant<StopEdge, BusEdge> &
    TransportRouter::GetEdge(graph::EdgeId id) const { return edge_id_to_edge_.at(id); }

    std::optional<RouterByStop> TransportRouter::GetRouterByStop(Stop *stop) const {
        if (stop_to_router_.count(stop)) {
            return stop_to_router_.at(stop);

        } else {
            return std::nullopt;
        }
    }

    std::optional<RouteInfo> TransportRouter::GetRouteInfo(std::size_t start, std::size_t end) const {
        const auto &route_info = router_->BuildRoute(start, end);
        if (route_info) {
            RouteInfo result;
            result.total_time = route_info->weight;

            for (const auto edge: route_info->edges) {
                result.edges.emplace_back(GetEdge(edge));
            }

            return result;

        } else {
            return std::nullopt;
        }
    }

    const std::unordered_map<Stop *, RouterByStop> &
    TransportRouter::GetStopToVertex() const { return stop_to_router_; }

    const std::unordered_map<graph::EdgeId, std::variant<StopEdge, BusEdge>> &
    TransportRouter::GetEdgeIdToEdge() const { return edge_id_to_edge_; }

    std::deque<Stop *> TransportRouter::GetStopsPrt(TransportCatalogue &transport_catalogue) {
        std::deque<Stop *> stops_ptr;

        for (auto [_, stop_ptr]: transport_catalogue.GetAllStops()) {
            stops_ptr.push_back(stop_ptr);
        }

        return stops_ptr;
    }

    std::deque<Bus *> TransportRouter::GetBusPrt(TransportCatalogue &transport_catalogue) {
        std::deque<Bus *> buses_ptr;

        for (auto [_, bus_ptr]: transport_catalogue.GetAllBuses()) {
            buses_ptr.push_back(bus_ptr);
        }

        return buses_ptr;
    }

    void TransportRouter::SetStops(const std::deque<Stop *> &stops) {
        size_t i = 0;

        for (const auto stop: stops) {
            graph::VertexId first = i++;
            graph::VertexId second = i++;

            stop_to_router_[stop] = RouterByStop{first, second};
        }
    }

    void TransportRouter::AddEdgeToStop() {

        for (const auto [stop, num]: stop_to_router_) {
            graph::EdgeId id = graph_->AddEdge(graph::Edge<double>{num.bus_wait_start,
                                                                   num.bus_wait_end,
                                                                   routing_settings_.bus_wait_time});

            edge_id_to_edge_[id] = StopEdge{stop->name, routing_settings_.bus_wait_time};
        }
    }

    void TransportRouter::AddEdgeToBus(TransportCatalogue &transport_catalogue) {

        for (auto bus: GetBusPrt(transport_catalogue)) {
            ParseBusToEdges(bus->stops.begin(),
                            bus->stops.end(),
                            transport_catalogue,
                            bus);

            if (!bus->is_roundtrip) {
                ParseBusToEdges(bus->stops.rbegin(),
                                bus->stops.rend(),
                                transport_catalogue,
                                bus);
            }
        }
    }

    void TransportRouter::SetGraph(TransportCatalogue &transport_catalogue) {
        const auto stops_ptr_size = GetStopsPrt(transport_catalogue).size();

        graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>
                (2 * stops_ptr_size);

        SetStops(GetStopsPrt(transport_catalogue));
        AddEdgeToStop();
        AddEdgeToBus(transport_catalogue);
    }

    graph::Edge<double> TransportRouter::MakeEdgeToBus(Stop *start, Stop *end, const double distance) const {
        graph::Edge<double> result{};

        result.from = stop_to_router_.at(start).bus_wait_end;
        result.to = stop_to_router_.at(end).bus_wait_start;
        result.weight = distance * 1.0 / (routing_settings_.bus_velocity * KILOMETER / HOUR);

        return result;
    }

}