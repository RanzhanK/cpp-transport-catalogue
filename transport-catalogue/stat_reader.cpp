#include "stat_reader.h"

#include <iostream>

namespace transport_catalogue_global_namespace::stat_reader {
    void GetDataBaseInfo(std::istream &input,const transport_catalogue::TransportCatalogue &transport_catalogue,std::ostream &out) {

        int str;
        input >> str;
        while (input.peek() != '\n') {
            input.ignore(1);
        }
        for (int i = 0; i < str; ++i) {
            std::string operation_code;
            input >> operation_code;

            if (operation_code == "Bus") {
                GetBusesInfo(input, transport_catalogue, out);
            }
            if (operation_code == "Stop") {
                GetStopsInfo(input, transport_catalogue, out);
            }
        }
    }
    void GetBusesInfo(std::istream& input, const transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out) {
        std::string query;
        while (input.peek() == ' ') {
            input.ignore(1);
        }
        getline(input, query, '\n');
        while (query.back() == ' ') {
            query.pop_back();
        }

        out << "Bus " << query << ": ";

        transport_catalogue::Bus bus = transport_catalogue.BusInfo(query);

        if (bus.stops.empty()) {
            out << "not found" << std::endl;
            return;
        }

        size_t route_size = bus.stops.size();

        std::unordered_set<const transport_catalogue::Stop *> set_unique_stops(bus.stops.begin(),bus.stops.end());
        size_t unique_stops = set_unique_stops.size();

        double geo_length = 0;
        int length = 0;

        for (int i = 1; i < route_size; ++i) {
            const transport_catalogue::Stop *prev_stop = bus.stops[i - 1];
            const transport_catalogue::Stop *cur_stop = bus.stops[i];
            geo_length += ComputeDistance(prev_stop->coordinates, cur_stop->coordinates);
            length += transport_catalogue.GetDistanceBetweenStops(prev_stop->name, cur_stop->name);
        }

        double curvature = length / geo_length;
        out << route_size << " stops on route, " << unique_stops << " unique stops, " << length
            << " route length, "
            << std::setprecision(6) << curvature << " curvature" << std::endl;
    }

    void GetStopsInfo(std::istream& input, const transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out) {
        std::string query;
        while (input.peek() == ' ') {
            input.ignore(1);
        }

        getline(input, query, '\n');
        while (query.back() == ' ') {
            query.pop_back();
        }

        out << "Stop " << query << ": ";

        if (!transport_catalogue.StopCheck(query)) {
            out << "not found" << std::endl;
            return;
        }

        std::set<std::string> buses;

        for (const auto &ptr_bus: transport_catalogue.GetBusesForStop(query)) {
            buses.insert(ptr_bus->name);
        }

        if (buses.empty()) {
            out << "no buses" << std::endl;
            return;
        }

        out << "buses ";
        for (const auto &bus: buses) {
            out << bus << " ";
        }
        out << std::endl;
    }
}