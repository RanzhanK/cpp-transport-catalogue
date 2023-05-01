#include "input_reader.h"

#include <queue>
#include <sstream>
#include <algorithm>

using namespace global;

void input_reader::CutSpaces(std::istringstream& stream) {
    while (stream.peek() == ' ') {
        stream.ignore(1);
    }
}

void input_reader::ParseQuery(std::istream& input, TransportCatalogue& transport_catalogue) {

    std::vector<std::string> queries;
    const int big_number = 1000000007;
    int count_of_queries;
    input >> count_of_queries;
    input.ignore(big_number, '\n');

    std::string query;
    std::vector<std::string> stop_queries, bus_queries;
    stop_queries.reserve(count_of_queries);
    bus_queries.reserve(count_of_queries);

    for (int i = 0; i < count_of_queries; ++i) {
        getline(input, query);
        size_t index = query.find_first_not_of(" ");
        if (query[index] == 'S') {
            stop_queries.push_back(query);
        }
        if (query[index] == 'B') {
            bus_queries.push_back(query);
        }
    }

    for (const auto& query : stop_queries) {
        queries.push_back(query);
    }
    for (const auto& query : bus_queries) {
        queries.push_back(query);
    }

    for (const std::string& query : queries) {
        std::istringstream stream(query);
        std::string operation_code;
        stream >> operation_code;

        if (operation_code == "Stop") {
            ReadStopWithoutDistances(stream, transport_catalogue);
        }
        if (operation_code == "Bus") {
            ReadBus(stream, transport_catalogue);
        }
    }

    for (const std::string& query : queries) {
        std::istringstream stream(query);
        std::string operation_code;
        stream >> operation_code;

        if (operation_code == "Stop") {
            ReadStopDistances(stream, transport_catalogue);
        } else {
            break;
        }
    }
}

void input_reader::ReadStopWithoutDistances(std::istringstream& stream, TransportCatalogue& transport_catalogue) {

    std::string stop;
    CutSpaces(stream);
    getline(stream, stop, ':');
    double lat, lng;
    std::string lat_str;
    getline(stream, lat_str, ',');
    lat = stod(lat_str);
    stream >> lng;
    transport_catalogue.AddStop(stop, {lat, lng});
}

void input_reader::ReadStopDistances(std::istringstream& stream, TransportCatalogue& transport_catalogue) {

    const int big_number = 1000000007;
    std::string stop;
    CutSpaces(stream);
    getline(stream, stop, ':');
    stream.ignore(big_number, ',');
    stream.ignore(big_number, ',');

    while (stream) {
        int distance;
        stream >> distance;
        stream.ignore(1, 'm');
        std::string skip_word_to;
        stream >> skip_word_to;
        CutSpaces(stream);
        std::string stop_to;
        getline(stream, stop_to, ',');
        while (stop_to.back() == ' ') {
            stop_to.pop_back();
        }
        if (!stream) {
            break;
        }
        transport_catalogue.SetDistanceBetweenStops(stop, stop_to, distance);
    }
}

void input_reader::ReadBus(std::istringstream& stream, TransportCatalogue& transport_catalogue) {

    std::string bus;
    CutSpaces(stream);
    getline(stream, bus, ':');

    std::string raw;
    getline(stream, raw);

    char separator;
    if (raw.find('>') != -1) {
        separator = '>';
    } else {
        separator = '-';
    }

    stream = std::istringstream(raw);

    bool is_cycle = true;
    if (separator == '-') {
        is_cycle = false;
    }

    std::vector<std::string> stops;

    while (stream) {
        CutSpaces(stream);
        if (!stream) {
            break;
        }
        std::string stop;
        getline(stream, stop, separator);
        while (stop.back() == ' ') {
            stop.pop_back();
        }
        stops.push_back(stop);
    }

    if (!is_cycle) {
        std::vector<std::string> reversed_path(stops);
        reversed_path.pop_back();
        reverse(reversed_path.begin(), reversed_path.end());
        for (const std::string& stop : reversed_path) {
            stops.push_back(stop);
        }
    }
    transport_catalogue.AddBus(bus, stops);
}