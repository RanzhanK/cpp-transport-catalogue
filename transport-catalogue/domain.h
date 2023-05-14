#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "geo.h"

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
}