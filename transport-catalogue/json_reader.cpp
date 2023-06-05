#include "json_reader.h"

namespace json {

    struct EdgeInfoGetter {

        Node operator()(const StopEdge &edge_info) {
            using namespace std::literals;

            return Builder{}.StartDict()
                    .Key("type").Value("Wait")
                    .Key("stop_name").Value(std::string(edge_info.name))
                    .Key("time").Value(edge_info.time)
                    .EndDict()
                    .Build();
        }

        Node operator()(const BusEdge &edge_info) {
            using namespace std::literals;

            return Builder{}.StartDict()
                    .Key("type").Value("Bus")
                    .Key("bus").Value(std::string(edge_info.bus_name))
                    .Key("span_count").Value(static_cast<int>(edge_info.span_count))
                    .Key("time").Value(edge_info.time)
                    .EndDict()
                    .Build();
        }
    };

    JSONReader::JSONReader(Document doc) : document_(std::move(doc)) {}

    JSONReader::JSONReader(std::istream &input) : document_(json::Load(input)) {}

    Stop JSONReader::ParseNodeStop(Node &node) {
        Stop stop;
        Dict stop_node;

        if (node.IsDict()) {
            stop_node = node.AsDict();
            stop.name = stop_node.at("name").AsString();
            stop.latitude = stop_node.at("latitude").AsDouble();
            stop.longitude = stop_node.at("longitude").AsDouble();
        }
        return stop;
    }

    std::vector<Distance>
    JSONReader::ParseNodeDistances(Node &node, transport_catalogue::TransportCatalogue &catalogue) {
        std::vector<Distance> distances;
        Dict stop_node;
        Dict stop_road_map;
        std::string begin_name;
        std::string last_name;
        int distance;

        if (node.IsDict()) {
            stop_node = node.AsDict();
            begin_name = stop_node.at("name").AsString();

            try {
                stop_road_map = stop_node.at("road_distances").AsDict();

                for (auto [key, value]: stop_road_map) {
                    last_name = key;
                    distance = value.AsInt();
                    distances.push_back({catalogue.FindStop(begin_name),
                                         catalogue.FindStop(last_name), distance});
                }

            } catch (...) {
                std::cout << "invalide road" << std::endl;
            }
        }
        return distances;
    }

    Bus JSONReader::ParseNodeBus(Node &node, transport_catalogue::TransportCatalogue &catalogue) {
        Bus bus;
        Dict bus_node;
        Array bus_stops;

        if (node.IsDict()) {
            bus_node = node.AsDict();
            bus.name = bus_node.at("name").AsString();
            bus.is_roundtrip = bus_node.at("is_roundtrip").AsBool();

            try {
                bus_stops = bus_node.at("stops").AsArray();

                for (Node stop: bus_stops) {
                    auto const x = catalogue.FindStop(stop.AsString());
                    bus.stops.push_back(const_cast<Stop *&&>(x));
                }

                if (!bus.is_roundtrip) {
                    size_t size = bus.stops.size() - 1;

                    for (size_t i = size; i > 0; i--) {
                        bus.stops.push_back(bus.stops[i - 1]);
                    }

                }

            } catch (...) {
                std::cout << "base_requests: bus: stops is empty" << std::endl;
            }
        }
        return bus;
    }

    void JSONReader::ParseNodeBase(const Node &root, transport_catalogue::TransportCatalogue &catalogue) {
        Array base_requests;
        Dict req_map;
        Node req_node;

        std::vector<Node> buses;
        std::vector<Node> stops;

        if (root.IsArray()) {
            base_requests = root.AsArray();

            for (Node &node: base_requests) {
                if (node.IsDict()) {
                    req_map = node.AsDict();

                    try {
                        req_node = req_map.at("type");

                        if (req_node.IsString()) {

                            if (req_node.AsString() == "Bus") {
                                buses.push_back(req_map);
                            } else if (req_node.AsString() == "Stop") {
                                stops.push_back(req_map);
                            } else {
                                std::cout << "base_requests are invalid";
                            }
                        }

                    } catch (...) {
                        std::cout << "base_requests does not have type value";
                    }
                }
            }

            for (auto stop: stops) {
                catalogue.AddStop(ParseNodeStop(stop));
            }

            for (auto stop: stops) {
                catalogue.SetDistanceBetweenStops(ParseNodeDistances(stop, catalogue));
            }

            for (auto bus: buses) {
                catalogue.AddBus(ParseNodeBus(bus, catalogue));
            }

        } else {
            std::cout << "base_requests is not an array";
        }
    }

    void JSONReader::ParseNodeStat(const Node &node, std::vector<StatRequest> &stat_request) {
        Array stat_requests;
        Dict req_map;
        StatRequest req;

        if (node.IsArray()) {
            stat_requests = node.AsArray();

            for (Node req_node: stat_requests) {

                if (req_node.IsDict()) {
                    req_map = req_node.AsDict();
                    req.id = req_map.at("id").AsInt();
                    req.type = req_map.at("type").AsString();

                    if ((req.type == "Bus") || (req.type == "Stop")) {
                        req.name = req_map.at("name").AsString();
                        req.from = "";
                        req.to = "";
                    } else {
                        req.name = "";
                        if (req.type == "Route") {
                            req.from = req_map.at("from").AsString();
                            req.to = req_map.at("to").AsString();
                        } else {
                            req.from = "";
                            req.to = "";
                        }
                    }

                    stat_request.push_back(req);

                }
            }

        } else {
            std::cout << "base_requests is not array";
        }
    }

    void JSONReader::ParseNodeRender(const Node &node, map_renderer::RenderSettings &rend_set) {
        Dict rend_map;
        Array bus_lab_offset;
        Array stop_lab_offset;
        Array arr_color;
        Array arr_palette;
        uint8_t red_;
        uint8_t green_;
        uint8_t blue_;
        double opacity_;

        if (node.IsDict()) {
            rend_map = node.AsDict();

            try {
                rend_set.width_ = rend_map.at("width").AsDouble();
                rend_set.height_ = rend_map.at("height").AsDouble();
                rend_set.padding_ = rend_map.at("padding").AsDouble();
                rend_set.line_width_ = rend_map.at("line_width").AsDouble();
                rend_set.stop_radius_ = rend_map.at("stop_radius").AsDouble();

                rend_set.bus_label_font_size_ = rend_map.at("bus_label_font_size").AsInt();

                if (rend_map.at("bus_label_offset").IsArray()) {
                    bus_lab_offset = rend_map.at("bus_label_offset").AsArray();
                    rend_set.bus_label_offset_ = std::make_pair(bus_lab_offset[0].AsDouble(),
                                                                bus_lab_offset[1].AsDouble());
                }

                rend_set.stop_label_font_size_ = rend_map.at("stop_label_font_size").AsInt();

                if (rend_map.at("stop_label_offset").IsArray()) {
                    stop_lab_offset = rend_map.at("stop_label_offset").AsArray();
                    rend_set.stop_label_offset_ = std::make_pair(stop_lab_offset[0].AsDouble(),
                                                                 stop_lab_offset[1].AsDouble());
                }

                if (rend_map.at("underlayer_color").IsString()) {
                    rend_set.underlayer_color_ = svg::Color(rend_map.at("underlayer_color").AsString());
                } else if (rend_map.at("underlayer_color").IsArray()) {
                    arr_color = rend_map.at("underlayer_color").AsArray();
                    red_ = arr_color[0].AsInt();
                    green_ = arr_color[1].AsInt();
                    blue_ = arr_color[2].AsInt();

                    if (arr_color.size() == 4) {
                        opacity_ = arr_color[3].AsDouble();
                        rend_set.underlayer_color_ = svg::Color(svg::Rgba(red_,
                                                                          green_,
                                                                          blue_,
                                                                          opacity_));
                    } else if (arr_color.size() == 3) {
                        rend_set.underlayer_color_ = svg::Color(svg::Rgb(red_,
                                                                         green_,
                                                                         blue_));
                    }

                }

                rend_set.underlayer_width_ = rend_map.at("underlayer_width").AsDouble();

                if (rend_map.at("color_palette").IsArray()) {
                    arr_palette = rend_map.at("color_palette").AsArray();

                    for (Node color_palette: arr_palette) {

                        if (color_palette.IsString()) {
                            rend_set.color_palette_.push_back(svg::Color(color_palette.AsString()));
                        } else if (color_palette.IsArray()) {
                            arr_color = color_palette.AsArray();
                            red_ = arr_color[0].AsInt();
                            green_ = arr_color[1].AsInt();
                            blue_ = arr_color[2].AsInt();

                            if (arr_color.size() == 4) {
                                opacity_ = arr_color[3].AsDouble();
                                rend_set.color_palette_.push_back(svg::Color(svg::Rgba(red_,
                                                                                       green_,
                                                                                       blue_,
                                                                                       opacity_)));
                            } else if (arr_color.size() == 3) {
                                rend_set.color_palette_.push_back(svg::Color(svg::Rgb(red_,
                                                                                      green_,
                                                                                      blue_)));
                            }
                        }
                    }
                }
            } catch (...) {
                std::cout << "unable to parsse init settings";
            }

        } else {
            std::cout << "render_settings is not map";
        }
    }

    void JSONReader::ParseNodeRouting(const Node &node, RoutingSettings &route_set) {
        Dict route;

        if (node.IsDict()) {
            route = node.AsDict();

            try {

                route_set.bus_wait_time = route.at("bus_wait_time").AsDouble();
                route_set.bus_velocity = route.at("bus_velocity").AsDouble();

            } catch (...) {
                std::cout << "unable to parse routing settings";
            }

        } else {
            std::cout << "routing settings is not map";
        }

    }

    void JSONReader::ParseNode(const Node &root, transport_catalogue::TransportCatalogue &catalogue,
                               std::vector<StatRequest> &stat_request,
                               map_renderer::RenderSettings &render_settings, RoutingSettings &routing_settings) {
        Dict root_dictionary;

        if (root.IsDict()) {
            root_dictionary = root.AsDict();

            try {
                ParseNodeBase(root_dictionary.at("base_requests"), catalogue);
            } catch (...) {
                std::cout << "base_requests is empty";
            }

            try {
                ParseNodeStat(root_dictionary.at("stat_requests"), stat_request);
            } catch (...) {
                std::cout << "stat_requests is empty";
            }

            try {
                ParseNodeRender(root_dictionary.at("render_settings"), render_settings);
            } catch (...) {
                std::cout << "render_settings is empty";
            }

            try {
                ParseNodeRouting(root_dictionary.at("routing_settings"), routing_settings);
            } catch (...) {
                std::cout << "routing_settings is empty";
            }

        } else {
            std::cout << "root is not map";
        }
    }

    void JSONReader::Parse(transport_catalogue::TransportCatalogue &catalogue, std::vector<StatRequest> &stat_request,
                           map_renderer::RenderSettings &render_settings, RoutingSettings &routing_settings) {
        ParseNode(document_.GetRoot(),
                  catalogue,
                  stat_request,
                  render_settings,
                  routing_settings);
    }

    Node JSONReader::ExecuteMakeNodeStop(int id_request, const StopQueryResult &stop_info) {
        Node result;
        Array buses;
        json::Builder builder;

        std::string str_not_found = "not found";

        if (stop_info.not_found) {
            builder.StartDict()
                    .Key("request_id").Value(id_request)
                    .Key("error_message").Value(str_not_found)
                    .EndDict();

            result = builder.Build();

        } else {
            builder.StartDict()
                    .Key("request_id").Value(id_request)
                    .Key("buses").StartArray();

            for (std::string bus_name: stop_info.buses_name) {
                builder.Value(bus_name);
            }

            builder.EndArray().EndDict();

            result = builder.Build();
        }

        return result;
    }

    Node JSONReader::ExecuteMakeNodeBus(int id_request, const BusQueryResult &bus_info) {
        Node result;
        std::string str_not_found = "not found";

        if (bus_info.not_found) {
            result = json::Builder{}.StartDict()
                    .Key("request_id").Value(id_request)
                    .Key("error_message").Value(str_not_found)
                    .EndDict()
                    .Build();
        } else {
            result = json::Builder{}.StartDict()
                    .Key("request_id").Value(id_request)
                    .Key("curvature").Value(bus_info.curvature)
                    .Key("route_length").Value(bus_info.route_length)
                    .Key("stop_count").Value(bus_info.stops_on_route)
                    .Key("unique_stop_count").Value(bus_info.unique_stops)
                    .EndDict()
                    .Build();
        }

        return result;
    }

    Node JSONReader::ExecuteMakeNodeMap(int id_request, transport_catalogue::TransportCatalogue &catalogue_,
                                        const map_renderer::RenderSettings &render_settings) {
        Node result;

        std::ostringstream map_stream;
        std::string map_str;

        MapRenderer map_catalogue(render_settings);

        map_catalogue.InitSphereProjector(request_handler::RequestHandler::GetStopsCoordinates(catalogue_));

        ExecuteRenderMap(map_catalogue, catalogue_);
        map_catalogue.GetStreamMap(map_stream);
        map_str = map_stream.str();

        result = json::Builder{}.StartDict()
                .Key("request_id").Value(id_request)
                .Key("map").Value(map_str)
                .EndDict()
                .Build();

        return result;
    }

    Node JSONReader::ExecuteMakeNodeRoute(StatRequest &request,
                                          TransportCatalogue &catalogue,
                                          transport_catalogue::detail::router::TransportRouter &routing) {
        const auto &route_info = request_handler::RequestHandler::GetRouteInfo(request.from, request.to, catalogue,
                                                                               routing);

        if (!route_info) {
            return Builder{}.StartDict()
                    .Key("request_id").Value(request.id)
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build();
        }

        Array items;
        for (const auto &item: route_info->edges) {
            items.emplace_back(std::visit(EdgeInfoGetter{}, item));
        }

        return Builder{}.StartDict()
                .Key("request_id").Value(request.id)
                .Key("total_time").Value(route_info->total_time)
                .Key("items").Value(items)
                .EndDict()
                .Build();
    }

    void JSONReader::ExecuteQueries(transport_catalogue::TransportCatalogue &catalogue,
                                    std::vector<StatRequest> &stat_requests,
                                    map_renderer::RenderSettings &render_settings,
                                    RoutingSettings &routing_settings) {
        std::vector<Node> result_request;
        transport_catalogue::detail::router::TransportRouter routing;

        routing.SetRoutingSettings(routing_settings);
        routing.BuildRoute(catalogue);

        for (StatRequest req: stat_requests) {

            if (req.type == "Stop") {
                result_request.push_back(ExecuteMakeNodeStop(req.id, StopQuery(catalogue, req.name)));
            } else if (req.type == "Bus") {
                result_request.push_back(ExecuteMakeNodeBus(req.id, BusQuery(catalogue, req.name)));
            } else if (req.type == "Map") {
                result_request.push_back(ExecuteMakeNodeMap(req.id, catalogue, render_settings));
            } else if (req.type == "Route") {
                result_request.push_back(ExecuteMakeNodeRoute(req, catalogue, routing));
            }
        }

        doc_out = Document{Node(result_request)};
    }

    void JSONReader::ExecuteRenderMap(map_renderer::MapRenderer &map_catalogue,
                                      transport_catalogue::TransportCatalogue &catalogue) const {
        std::vector<std::pair<Bus *, int>> buses_palette;
        std::vector<Stop *> stops_sort;
        int palette_size = 0;
        int palette_index = 0;

        palette_size = map_catalogue.GetPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return;
        }

        auto buses = catalogue.GetAllBuses();
        if (buses.size() > 0) {

            for (std::string_view bus_name: request_handler::RequestHandler::GetSortBusesNames(catalogue)) {
                Bus *bus_info = const_cast<Bus *>(catalogue.FindBus(bus_name));

                if (bus_info) {
                    if (bus_info->stops.size() > 0) {
                        buses_palette.push_back(std::make_pair(bus_info, palette_index));
                        palette_index++;

                        if (palette_index == palette_size) {
                            palette_index = 0;
                        }
                    }
                }
            }

            if (buses_palette.size() > 0) {
                map_catalogue.AddLine(buses_palette);
                map_catalogue.AddBusesName(buses_palette);
            }
        }

        auto stops = catalogue.GetAllStops();
        if (stops.size() > 0) {
            std::vector<std::string_view> stops_name;

            for (auto &[stop_name, stop]: stops) {

                if (stop->buses.size() > 0) {
                    stops_name.push_back(stop_name);
                }
            }

            std::sort(stops_name.begin(), stops_name.end());

            for (std::string_view stop_name: stops_name) {
                Stop *stop = const_cast<Stop *>(catalogue.FindStop(stop_name));
                if (stop) {
                    stops_sort.push_back(stop);
                }
            }

            if (stops_sort.size() > 0) {
                map_catalogue.AddStopsCircle(stops_sort);
                map_catalogue.AddStopsName(stops_sort);
            }
        }
    }

    BusQueryResult JSONReader::BusQuery(transport_catalogue::TransportCatalogue &catalogue, std::string_view bus_name) {
        BusQueryResult bus_info;
        Bus *bus = const_cast<Bus *>(catalogue.FindBus(bus_name));

        if (bus != nullptr) {
            bus_info.name = bus->name;
            bus_info.not_found = false;
            bus_info.stops_on_route = static_cast<int>(bus->stops.size());
            bus_info.unique_stops = static_cast<int>(catalogue.GetUniqStops(bus).size());
            bus_info.route_length = static_cast<int>(bus->route_length);
            bus_info.curvature = double(catalogue.GetDistanceToBus(bus)
                                        / catalogue.GetLength(bus));
        } else {
            bus_info.name = bus_name;
            bus_info.not_found = true;
        }

        return bus_info;
    }

    StopQueryResult
    JSONReader::StopQuery(transport_catalogue::TransportCatalogue &catalogue, std::string_view stop_name) {
        std::unordered_set<const Bus *> unique_buses;
        StopQueryResult stop_info;
        Stop *stop = const_cast<Stop *>(catalogue.FindStop(stop_name));

        if (stop != NULL) {

            stop_info.name = stop->name;
            stop_info.not_found = false;
            unique_buses = catalogue.GetBusesForStop(stop);

            if (unique_buses.size() > 0) {
                for (const Bus *bus: unique_buses) {
                    stop_info.buses_name.push_back(bus->name);
                }

                std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
            }

        } else {
            stop_info.name = stop_name;
            stop_info.not_found = true;
        }

        return stop_info;
    }

    const Document &JSONReader::GetDocument() const {
        return doc_out;
    }
}