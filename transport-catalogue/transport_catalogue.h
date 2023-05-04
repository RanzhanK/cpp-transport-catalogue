#pragma once

#include "geo.h"

#include <set>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <cassert>
#include <iomanip>
#include <optional>
#include <iostream>
#include <string_view>
#include <unordered_set>
#include <unordered_map>

namespace transport_catalogue {

    struct Stop {
        std::string name;
        geo::Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
    };

    struct DataBusInfo {
        size_t route_size;
        size_t unique_stops;
        int length;
        double curvature;
    };

    class TransportCatalogue {
    public:
        void AddStop(std::string_view stop, geo::Coordinates coordinates);
        void AddBus(std::string_view bus, const std::vector<std::string>& stops);
        void SetDistanceBetweenStops(std::string_view from, std::string_view to, int distance);
        int GetDistanceBetweenStops(std::string_view from, std::string_view to) const;
        std::unordered_set<const Bus*> GetBusesForStop(std::string_view stop) const;
        bool StopCheck(std::string_view stop) const;
        DataBusInfo BusInfo(const std::string& query) const;

    private:
        struct Hasher {
            size_t operator() (const std::pair<const Stop*, const Stop*>& info_) const {
                auto h1 = hasher_(info_.first->name);
                auto h2 = hasher_(info_.second->name);
                return 37 * h1 + h2;
            }
            std::hash<std::string> hasher_;
        };
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Stop*> name_to_stop_;
        std::unordered_map<std::string_view, const Bus*> name_to_bus_;
        std::unordered_map<const Stop*, std::unordered_set<const Bus*>> stop_to_buses_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, Hasher> distances_;
    };
}