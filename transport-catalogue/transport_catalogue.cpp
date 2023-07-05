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

    const Bus *TransportCatalogue::FindBus(std::string_view bus_name) {
        if (name_to_bus_.empty()) {
            return nullptr;
        }

        if (name_to_bus_.count(bus_name)) {
            return name_to_bus_.at(bus_name);
        } else {
            return nullptr;
        }
    }

    const Stop *TransportCatalogue::FindStop(std::string_view stop_name) {
        if (name_to_stop_.empty()) {
            return nullptr;
        }

        if (name_to_stop_.count(stop_name)) {
            return name_to_stop_.at(stop_name);
        } else {
            return nullptr;
        }
    }

    void TransportCatalogue::SetDistanceBetweenStops(const std::vector<Distance> &distances) {
        for (auto distance: distances) {
            auto dist_pair = std::make_pair(distance.start, distance.end);
            distance_to_stop.insert(distances_::value_type(dist_pair, distance.distance));
        }
    }

    std::unordered_set<const Bus *> TransportCatalogue::GetBusesForStop(const Stop *stop) {
        std::unordered_set<const Bus *> unique_stops;

        unique_stops.insert(stop->buses.begin(), stop->buses.end());

        return unique_stops;
    }

    size_t TransportCatalogue::GetDistanceBetweenStops(const Stop *stop1, const Stop *stop2) const {

        if (distance_to_stop.empty()) {
            return 0;

        } else {

            if (const auto &dist_pair = std::make_pair(stop1, stop2);
                    distance_to_stop.count(dist_pair)) {

                return distance_to_stop.at(dist_pair);

            } else if (const auto &dist_pair = std::make_pair(stop2, stop1);
                    distance_to_stop.count(dist_pair)) {

                return distance_to_stop.at(dist_pair);

            } else {

                return 0;
            }
        }
    }

    std::unordered_map<std::string_view, Bus *> const& TransportCatalogue::GetAllBuses() const {
        return name_to_bus_;
    }

    std::unordered_map<std::string_view, Stop *> const& TransportCatalogue::GetAllStops() const {
        return name_to_stop_;
    }

    std::unordered_set<const Stop *> TransportCatalogue::GetUniqStops(const Bus *bus) {
        std::unordered_set<const Stop *> unique_stops;

        unique_stops.insert(bus->stops.begin(), bus->stops.end());

        return unique_stops;
    }

    double TransportCatalogue::GetLength(const Bus *bus) {
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

    size_t TransportCatalogue::GetDistanceToBus(const Bus *bus) {
        size_t distance = 0;
        auto stops_size = bus->stops.size() - 1;
        for (int i = 0; i < static_cast<int>(stops_size); i++) {
            distance += GetDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
        }
        return distance;
    }

    std::deque<Stop> TransportCatalogue::GetStops() const {
        return stops_;
    }

    std::deque<Bus> TransportCatalogue::GetBuses() const {
        return buses_;
    }

    TransportCatalogue::distances_ TransportCatalogue::GetDistance() const {
        return distance_to_stop;
    }
}