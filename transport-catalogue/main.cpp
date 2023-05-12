#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

int main() {
    std::vector<StatRequest> stat_request;
    RenderSettings render_settings;
    TransportCatalogue catalogue;

    JSONReader json_reader;
    request_handler::RequestHandler request_handler;

    json_reader = JSONReader(std::cin);
    json_reader.Parse(catalogue, stat_request, render_settings);

    request_handler = request_handler::RequestHandler();
    request_handler.ExecuteQueries(catalogue, stat_request, render_settings);
    json::Print(request_handler.GetDocument(), std::cout);
}