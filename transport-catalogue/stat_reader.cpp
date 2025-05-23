#include <algorithm>
#include <iostream>
#include <string>

#include "stat_reader.h"

using namespace std::literals;

namespace transport_catalogue {

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto command_begin = request.find_first_not_of(' ');
    auto space_pos = request.find(' ');
    auto not_space = request.find_first_not_of(' ', space_pos);

    std::string_view command = request.substr(command_begin, space_pos - command_begin);
    if (command == "Bus"s) {
        auto req = request.substr(not_space);
        if (transport_catalogue.FindBus(req) == nullptr) {
            output << "Bus "s << req << ": not found"s << std::endl;
        } else {
            BusInfo bus_info = transport_catalogue.GetBusInfo(req);
            output << "Bus "s << req << ": "s << bus_info.stops_on_route << " stops on route, "s << bus_info.unique_stops << " unique stops, "s
                   << bus_info.route_length << " route length"s << std::endl;
        }
    } else {
        auto req = request.substr(not_space);
        if (transport_catalogue.FindStop(req) == nullptr) {
            output << "Stop "s << req << ": not found"s << std::endl;
        } else {
            auto buses = transport_catalogue.GetStopInfo(req);
            if (buses.empty()) {
                output << "Stop "s << req << ": no buses"s << std::endl;
            } else {
                output << "Stop "s << req << ": buses"s;
                for (const auto& bus_name : buses) {
                    output << " "s << bus_name;
                }
                output << std::endl;
            }
        }
    }
}

}