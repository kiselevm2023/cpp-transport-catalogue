#pragma once
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <string>
#include <stddef.h>
#include "geo.h"

using stops = std::vector<std::string>;
using buses = std::set<std::string>;

namespace transport{

class TransportCatalogue {

private:

    std::unordered_map<std::string, stops> bus_stops_; //key - автобус, value - vector<string> остановок
    std::unordered_map<std::string, geo::Coordinates> stops_coordinates; //key - остановка, value - координаты остановки
    std::unordered_map<std::string, buses> stop_buses_;

public:

    void InputBusStops(std::string& name_bus, const std::vector<std::string_view>& input);

    void InputStopsCoordinates(std::string& name_stop , geo::Coordinates& coord);

    size_t InsertStopsInSet(const std::vector<std::string> &input) const;

    std::string PrintBusInfo(std::string_view& name_bus) const;

    std::string PrintStopBuses(std::string_view& name_stop) const;


};
}