#pragma once

#include <vector>
#include <string>
#include <variant>
#include <algorithm>

#include "geo.h"
#include "graph.h"

namespace transport_catalogue {

    struct Bus;

    struct Stop {
        std::string name;
        double latitude;
        double longitude;

        std::vector<Bus*> buses;
    };

    struct Bus {
        std::string name;
        std::vector<Stop*> stops;
        bool is_roundtrip;
        size_t route_length;
    };

    struct Distance {
        const Stop* start;
        const Stop* end;
        int distance;
    };

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
}