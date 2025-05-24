#include "transport_catalogue.h"

using namespace std::literals;

namespace transport_catalogue {

void TransportCatalogue::AddStop(const Stop& stop) {
    stops_.push_back(stop);
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

Stop* TransportCatalogue::FindStop(const std::string_view stop_name) const {
    auto stop = stopname_to_stop_.find(stop_name);
    if (stop == stopname_to_stop_.end()) {
        return nullptr;
    }
    return stop->second;
}

void TransportCatalogue::AddBus(const Bus& bus) {
    buses_.push_back(bus);
    busname_to_bus_[buses_.back().name] = &buses_.back();
    for (const auto& stop : buses_.back().stops) {
        stopname_to_busname_[stop->name].insert(buses_.back().name);
    }
}

Bus* TransportCatalogue::FindBus(const std::string_view bus_name) const {
    auto bus = busname_to_bus_.find(bus_name);
    if (bus == busname_to_bus_.end()) {
        return nullptr;
    }
    return bus->second;
}

std::size_t TransportCatalogue::GetStopsOnRoute(const std::string_view& request) const {
    return busname_to_bus_.at(request)->stops.size();
}

std::size_t TransportCatalogue::GetUniqueStops(const std::string_view& request) const {
    std::unordered_set<Stop*> unique_stops;
    for (const auto& stop : busname_to_bus_.at(request)->stops) {
        unique_stops.insert(stop);
    }
    return unique_stops.size();
}

double TransportCatalogue::GetRouteLength(const std::string_view& request) const {
    double res = 0.0;

    auto it = busname_to_bus_.find(request);
    if (it == busname_to_bus_.end()) {
        return 0.0;
    }

    const Bus* bus= it->second; // Получаем указатель на Bus

    if (bus->stops.empty()) {
        return 0.0;
    }

    for (std::size_t i = 1; i < bus->stops.size(); ++i) {
        res += ComputeDistance(bus->stops[i - 1]->coordinates, bus->stops[i]->coordinates);
    }

    return res;
}

BusInfo TransportCatalogue::GetBusInfo(const std::string_view request) const {
    return BusInfo({GetStopsOnRoute(request), GetUniqueStops(request), GetRouteLength(request)});
}

const std::set<std::string_view>& TransportCatalogue::GetStopInfo(const std::string_view request) const {
    static const std::set<std::string_view> empty_set; // Статическая переменная для пустого множества

    auto it = stopname_to_busname_.find(request);
    if (it == stopname_to_busname_.end()) {
        return empty_set; // Возвращаем ссылку на статическое пустое множество
    }
    return it->second; // Возвращаем ссылку на существующее множество
}

}