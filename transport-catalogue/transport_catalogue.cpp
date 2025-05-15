#include "transport_catalogue.h"

using namespace transport;

void TransportCatalogue::InputBusStops(std::string& name_bus, const std::vector<std::string_view>& input) { //для ввода остановок автобуса X
    size_t count = 0;
    size_t size_input = input.size();
    while(count != size_input){
        bus_stops_[name_bus].push_back(std::string(input[count]));
        stop_buses_[std::string(input[count])].insert(name_bus);
        ++count;
    }
}

size_t TransportCatalogue::InsertStopsInSet(const std::vector<std::string>& input) const{
    std::unordered_set<std::string> out(input.begin(),input.end());
    return out.size();

}

void TransportCatalogue::InputStopsCoordinates(std::string& name_stop , geo::Coordinates& coord) { //для ввода координат остановки
    stops_coordinates[name_stop] = {coord.lat, coord.lng};
}

std::string TransportCatalogue::PrintStopBuses(std::string_view& name_stop) const{

    if(stops_coordinates.count(std::string(name_stop)) == 0) {
        return "Stop " + std::string(name_stop) + ": not found";
    }
    else if(stop_buses_.count(std::string(name_stop)) == 0) {
        return "Stop " + std::string(name_stop) + ": no buses";
    }
    std::string out = "Stop " + std::string(name_stop) + ": buses ";

    for(const auto& val : stop_buses_.at(std::string(name_stop))){
        out += val + " ";
    }

    return out;

}

std::string TransportCatalogue::PrintBusInfo(std::string_view& name_bus) const{
    std::string i(name_bus);
    auto it = bus_stops_.count(i);
    if(it == 0){
        return  "Bus " + i + ": not found";
    }

    size_t count_unique_stops = 0;
    size_t count_stops = 0;
    double distance = 0.;
    std::string out;


    count_stops = bus_stops_.at(i).size();
    count_unique_stops = InsertStopsInSet(bus_stops_.at(i));

    auto vec_stops = bus_stops_.at(i);

    for(auto ii = 0u; ii < vec_stops.size() - 1; ++ii){
        distance += ComputeDistance(stops_coordinates.at(vec_stops[ii]), stops_coordinates.at(vec_stops[ii+1]));
    }

    out += "Bus " + i + ": " + std::to_string(count_stops)
            + " stops on route, " + std::to_string(count_unique_stops) +
            " unique stops, " + std::to_string(distance) + " route length";

    return out;
}