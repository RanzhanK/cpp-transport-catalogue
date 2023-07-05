#pragma once

#include <deque>
#include <string>
#include <vector>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <execution>
#include <unordered_set>
#include <unordered_map>

#include "domain.h"

namespace transport_catalogue {

    class TransportCatalogue {
    public:
        struct Hasher {
            size_t operator()(const std::pair<const Stop *, const Stop *> &info_) const {
                auto h1 = hasher_(info_.first->name);
                auto h2 = hasher_(info_.second->name);
                return 37 * h1 + h2;
            }

            std::hash<std::string> hasher_;
        };

        void AddStop(Stop &&stop);

        void AddBus(Bus &&bus);

        const Bus *FindBus(std::string_view bus_name);

        const Stop *FindStop(std::string_view stop_name);

        void SetDistanceBetweenStops(const std::vector<Distance> &distances);

        size_t GetDistanceBetweenStops(const Stop *stop1, const Stop *stop2) const;

        std::unordered_set<const Bus *> GetBusesForStop(const Stop *stop);

        std::unordered_map<std::string_view, Bus *> const& GetAllBuses() const;

        std::unordered_map<std::string_view, Stop *> const& GetAllStops() const;

        std::unordered_set<const Stop *> GetUniqStops(const Bus *bus);

        double GetLength(const Bus *bus);

        size_t GetDistanceToBus(const Bus *bus);

        using distances_ = std::unordered_map<std::pair<const Stop *, const Stop *>, int, Hasher>;

        std::deque<Stop> GetStops() const;

        std::deque<Bus> GetBuses() const;

        distances_ GetDistance() const;

    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, Bus *> name_to_bus_;
        std::unordered_map<std::string_view, Stop *> name_to_stop_;

        distances_ distance_to_stop;
    };
}