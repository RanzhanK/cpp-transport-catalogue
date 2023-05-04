#include "transport_catalogue.h"

namespace transport_catalogue {

    void TransportCatalogue::AddStop(std::string_view stop, geo::Coordinates coordinates) {
        std::string stop_name(stop);
        stops_.push_back({stop_name, coordinates});
        name_to_stop_[stops_.back().name] = &(stops_.back());
        stop_to_buses_[&(stops_.back())];
    }

    void TransportCatalogue::AddBus(std::string_view bus, const std::vector<std::string> &stops) {
        std::string bus_name(bus);
        buses_.push_back({bus_name, {}});
        buses_.back().stops.reserve(stops.size());
        for (const std::string &stop: stops) {
            buses_.back().stops.push_back(name_to_stop_[stop]);
        }

        name_to_bus_[std::string_view(buses_.back().name)] = &buses_.back();

        for (const std::string &stop: stops) {
            const Stop *ptr_stop = name_to_stop_.at(stop);
            stop_to_buses_[ptr_stop].insert(&buses_.back());
        }
    }

    void TransportCatalogue::SetDistanceBetweenStops(std::string_view from, std::string_view to, int distance) {
        std::string from_name(from), to_name(to);
        const Stop *from_ptr = name_to_stop_.at(from_name);
        const Stop *to_ptr = name_to_stop_.at(to_name);

        distances_[{from_ptr, to_ptr}] = distance;
    }

    std::unordered_set<const Bus *> TransportCatalogue::GetBusesForStop(std::string_view stop) const {
        static const std::unordered_set<const Bus *> empty_set;
        std::string stop_name(stop);
        if (name_to_stop_.count(stop_name) == 0) {
            return empty_set;
        }
        const Stop *ptr_stop = name_to_stop_.at(stop_name);
        return stop_to_buses_.at(ptr_stop);
    }

    int TransportCatalogue::GetDistanceBetweenStops(std::string_view from, std::string_view to) const {
        std::string from_name(from), to_name(to);
        const Stop *from_ptr = name_to_stop_.at(from_name);
        const Stop *to_ptr = name_to_stop_.at(to_name);

        if (distances_.count({from_ptr, to_ptr})) {
            return distances_.at({from_ptr, to_ptr});
        }
        assert(distances_.count({to_ptr, from_ptr}));
        return distances_.at({to_ptr, from_ptr});
    }

    bool TransportCatalogue::StopCheck(std::string_view stop) const {
        return name_to_stop_.count(std::string(stop));
    }

    DataBusInfo TransportCatalogue::BusInfo(const std::string& query) const {
        DataBusInfo matched_bus{};

        if (name_to_bus_.count(query) == 0) {
            return {};
        }
        Bus bus = *(name_to_bus_.at(query));
        size_t route_size = bus.stops.size();
        std::unordered_set<const Stop *> set_unique_stops(bus.stops.begin(), bus.stops.end());
        size_t unique_stops = set_unique_stops.size();

        double geo_length = 0;
        int length = 0;

        for (int i = 1; i < route_size; ++i) {
            const Stop *prev_stop = bus.stops[i - 1];
            const Stop *cur_stop = bus.stops[i];
            geo_length += ComputeDistance(prev_stop->coordinates, cur_stop->coordinates);
            length += GetDistanceBetweenStops(prev_stop->name, cur_stop->name);
        }

        matched_bus.route_size = route_size;
        matched_bus.unique_stops = unique_stops;
        matched_bus.length = length;
        matched_bus.curvature = length / geo_length;;

        return matched_bus;
    }
}