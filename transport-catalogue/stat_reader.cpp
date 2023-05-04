#include "stat_reader.h"

#include <iostream>

namespace transport_catalogue::stat_reader {
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

        DataBusInfo bus_inf = transport_catalogue.BusInfo(query);

        if (bus_inf.route_size == 0 && bus_inf.unique_stops == 0 && bus_inf.length == 0 && bus_inf.curvature == 0){
            out << "not found" << std::endl;
            return;
        } else {
            out << bus_inf.route_size << " stops on route, " << bus_inf.unique_stops << " unique stops, " << bus_inf.length
                << " route length, "
                << std::setprecision(6) << bus_inf.curvature << " curvature" << std::endl;
        }
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