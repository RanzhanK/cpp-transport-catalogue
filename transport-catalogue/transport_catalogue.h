#pragma once

#include "geo.h"

#include <set>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <unordered_set>
#include <unordered_map>

using namespace global::geo;

namespace global::transport_catalogue {

    struct Stop {
        std::string name;
        Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
    };

    class TransportCatalogue {
    public:
        void AddStop(std::string_view stop, Coordinates coordinates);
        void AddBus(std::string_view bus, const std::vector<std::string>& stops);
        void SetDistanceBetweenStops(std::string_view from, std::string_view to, int distance);
        void BusInfo(std::string query, std::ostream& out = std::cout);
        void BusesForStopInfo(std::string query, std::ostream& out = std::cout);
        int DistanceBetweenStops(std::string_view from, std::string_view to) const;
        std::unordered_set<const Bus*> GetBusesForStop(std::string_view stop) const;

        struct Hasher {
            size_t operator() (const std::pair<const Stop*, const Stop*>& info_) const {
                auto h1 = hasher_(info_.first->name);
                auto h2 = hasher_(info_.second->name);
                return 37 * h1 + h2;
            }
            std::hash<std::string> hasher_;
        };

    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
        std::unordered_map<std::string, const Stop*> name_to_stop_;
        std::unordered_map<std::string, const Bus*> name_to_bus_;
        std::unordered_map<const Stop*, std::unordered_set<const Bus*>> stop_to_buses_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, Hasher> distances_;
    };
}