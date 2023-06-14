#include "request_handler.h"

namespace transport_catalogue::request_handler {

    std::optional<transport_catalogue::router::RouteInfo> RequestHandler::GetRouteInfo(std::string_view start,
                                                          std::string_view end,
                                                          TransportCatalogue &catalogue,
                                                          transport_catalogue::router::TransportRouter &routing) {


        return routing.GetRouteInfo(
                routing.GetRouterByStop(const_cast<Stop *>(catalogue.FindStop(start)))->bus_wait_start,
                routing.GetRouterByStop(const_cast<Stop *>(catalogue.FindStop(end)))->bus_wait_start);
    }

    std::vector<geo::Coordinates> RequestHandler::GetStopsCoordinates(TransportCatalogue &catalogue_) {

        std::vector<geo::Coordinates> stops_coordinates;
        auto buses = catalogue_.GetAllBuses();

        for (auto &[busname, bus]: buses) {
            for (auto &stop: bus->stops) {
                geo::Coordinates coordinates;
                coordinates.latitude = stop->latitude;
                coordinates.longitude = stop->longitude;

                stops_coordinates.push_back(coordinates);
            }
        }
        return stops_coordinates;
    }

    std::vector<std::string_view> RequestHandler::GetSortBusesNames(TransportCatalogue &catalogue_) {
        std::vector<std::string_view> buses_names;

        auto buses = catalogue_.GetAllBuses();
        if (buses.size() > 0) {

            for (auto &[busname, bus]: buses) {
                buses_names.push_back(busname);
            }

            std::sort(buses_names.begin(), buses_names.end());

            return buses_names;

        } else {
            return {};
        }
    }
}