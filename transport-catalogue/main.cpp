#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

int main() {
    std::vector<StatRequest> stat_request;
    RenderSettings render_settings;
    TransportCatalogue catalogue;
    transport_catalogue::router::RoutingSettings routing_settings;

    JSONReader json_reader;
    request_handler::RequestHandler request_handler;

    json_reader = JSONReader(std::cin);
    json_reader.Parse(catalogue, stat_request, render_settings, routing_settings);

    json_reader.ExecuteQueries(catalogue, stat_request, render_settings, routing_settings);
    json::Print(json_reader.GetDocument(), std::cout);
}