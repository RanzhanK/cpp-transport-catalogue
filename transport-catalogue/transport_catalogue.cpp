#include "transport_catalogue.h"

namespace transport_catalogue {

    void TransportCatalogue::AddStop(Stop &&stop) {
        stops_.push_back(std::move(stop));
        name_to_stop_[stops_.back().name] = &stops_.back();
    }

    void TransportCatalogue::AddBus(Bus &&bus) {
        buses_.push_back(std::move(bus));
        name_to_bus_[buses_.back().name] = &buses_.back();

        Bus *bus_buf = &buses_.back();

        for (Stop *stop: bus_buf->stops) {
            stop->buses.push_back(bus_buf);
        }

        bus_buf->route_length = GetDistanceToBus(bus_buf);
    }

    Bus *TransportCatalogue::FindBus(std::string_view bus_name) {
        if (name_to_bus_.empty()) {
            return nullptr;
        }

        try {
            return name_to_bus_.at(bus_name);
        }
        catch (const std::out_of_range &e) {
            return nullptr;
        }
    }

    Stop *TransportCatalogue::FindStop(std::string_view stop_name) {
        if (name_to_stop_.empty()) {
            return nullptr;
        }

        try {
            return name_to_stop_.at(stop_name);
        } catch (const std::out_of_range &e) {
            return nullptr;
        }
    }

    void TransportCatalogue::SetDistanceBetweenStops(const std::vector<Distance> &distances) {
        for (auto distance: distances) {
            auto dist_pair = std::make_pair(distance.start, distance.end);
            distance_to_stop.insert(distances_::value_type(dist_pair, distance.distance));
        }
    }

    std::unordered_set<const Bus *> TransportCatalogue::GetBusesForStop(Stop *stop) {
        std::unordered_set<const Bus *> unique_stops;

        unique_stops.insert(stop->buses.begin(), stop->buses.end());

        return unique_stops;
    }

    size_t TransportCatalogue::GetDistanceBetweenStops(const Stop *stop1, const Stop *stop2) {
        if (distance_to_stop.empty()) {
            return 0;
        }

        try {

            auto dist_pair = std::make_pair(stop1, stop2);
            return distance_to_stop.at(dist_pair);

        } catch (const std::out_of_range &e) {

            try {
                auto dist_pair = std::make_pair(stop2, stop1);
                return distance_to_stop.at(dist_pair);
            } catch (const std::out_of_range &e) {
                return 0;
            }

        }
    }

    std::unordered_map<std::string_view, Bus *> TransportCatalogue::GetAllBuses() const {
        return name_to_bus_;
    }

    std::unordered_map<std::string_view, Stop *> TransportCatalogue::GetAllStops() const {
        return name_to_stop_;
    }

    std::unordered_set<const Stop *> TransportCatalogue::GetUniqStops(Bus *bus) {
        std::unordered_set<const Stop *> unique_stops;

        unique_stops.insert(bus->stops.begin(), bus->stops.end());

        return unique_stops;
    }

    double TransportCatalogue::GetLength(Bus *bus) {
        return transform_reduce(
                next(bus->stops.begin()),
                bus->stops.end(),
                bus->stops.begin(),
                0.0,
                std::plus<>{},
                [](const Stop *lhs, const Stop *rhs) {
                    return geo::compute_distance({(*lhs).latitude, (*lhs).longitude},
                                                 {(*rhs).latitude, (*rhs).longitude});
                });
    }

    size_t TransportCatalogue::GetDistanceToBus(Bus *bus) {
        size_t distance = 0;
        auto stops_size = bus->stops.size() - 1;
        for (int i = 0; i < static_cast<int>(stops_size); i++) {
            distance += GetDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
        }
        return distance;
    }
}