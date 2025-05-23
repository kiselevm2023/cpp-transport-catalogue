#pragma once

#include <cassert>
#include <deque>
#include <set>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <vector>

#include "geo.h"

namespace transport_catalogue {

    struct Stop {
        std::string name;
        Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<Stop*> stops;
    };

    struct BusInfo {
        std::size_t stops_on_route;
        std::size_t unique_stops;
        double route_length;
    };

class TransportCatalogue {
public:
    void AddStop(const Stop& stop);

    Stop* FindStop(const std::string_view& stop_name) const ;

    void AddBus(const Bus& bus);

    Bus* FindBus(const std::string_view& bus_name) const;

    BusInfo GetBusInfo(const std::string_view& request) const;

    std::set<std::string_view> GetStopInfo(const std::string_view& request) const;

private:
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;

    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;

    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_busname_;

    std::size_t GetStopsOnRoute(const std::string_view& request) const;

    std::size_t GetUniqueStops(const std::string_view& request) const;

    double GetRouteLength(const std::string_view& request) const;
};

}