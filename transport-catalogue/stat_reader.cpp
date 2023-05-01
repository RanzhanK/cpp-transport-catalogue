#include "stat_reader.h"

#include <iostream>

using namespace global;
using transport_catalogue::Bus, transport_catalogue::Stop;

void stat_reader::GetDataBaseInfo(std::istream& input, TransportCatalogue& transport_catalogue, std::ostream& out) {

    int str;
    input >> str;
    while (input.peek() != '\n') {
        input.ignore(1);
    }
    for (int i = 0; i < str; ++i) {
        std::string operation_code;
        input >> operation_code;

        while (input.peek() == ' ') {
            input.ignore(1);
        }

        std::string query;
        getline(input, query, '\n');
        while (query.back() == ' ') {
            query.pop_back();
        }

        if (operation_code == "Bus") {
            out << "Bus " << query << ": ";
            transport_catalogue.BusInfo(query, out);
        }
        if (operation_code == "Stop") {
            out << "Stop " << query << ": ";
            transport_catalogue.BusesForStopInfo(query, out);
        }
    }
}