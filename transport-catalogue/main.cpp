#include <fstream>
#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    TransportCatalogue transport_catalogue;

    RenderSettings render_settings;
    transport_catalogue::router::RoutingSettings routing_settings;

    serialization::SerializationSettings serialization_settings;

    JSONReader json_reader;
    vector<StatRequest> stat_request;

    if (mode == "make_base"sv) {

        json_reader = JSONReader(cin);

        json_reader.ParseNodeMakeBase(transport_catalogue,
                                         render_settings,
                                         routing_settings,
                                         serialization_settings);

        ofstream out_file(serialization_settings.file_name, ios::binary);
        serialization::catalogue_serialization(transport_catalogue, render_settings, routing_settings, out_file);

    } else if (mode == "process_requests"sv) {

        json_reader = JSONReader(cin);

        json_reader.ParseNodeProcessRequests(stat_request,
                                                serialization_settings);

        ifstream in_file(serialization_settings.file_name, ios::binary);

        serialization::Catalogue catalogue = serialization::catalogue_deserialization(in_file);

        transport_catalogue::request_handler::RequestHandler request_handler;

        json_reader.ExecuteQueries(catalogue.transport_catalogue_,
                                        stat_request,
                                        catalogue.render_settings_,
                                        catalogue.routing_settings_);

        Print(json_reader.GetDocument(), cout);

    } else {
        PrintUsage();
        return 1;
    }
}