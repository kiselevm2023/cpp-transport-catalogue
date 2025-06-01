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
        std::vector<const Stop*> stops;
    };

    struct BusInfo {
        std::size_t stops_on_route;
        std::size_t unique_stops;
        int route_length;
        double curvature;
    };

    struct PairHasher {
        template <typename T1, typename T2>
        std::size_t operator() (const std::pair<T1, T2>& pair) const {
            auto hash1 = std::hash<const void*>()(static_cast<const void*>(pair.first));
            auto hash2 = std::hash<const void*>()(static_cast<const void*>(pair.second));
            return hash1 ^ (hash2 << 1);
        }
    };

    struct PairEqual {
        template <typename T1, typename T2>
        bool operator() (const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };

class TransportCatalogue {
public:
    void AddStop(const Stop& stop);

    const Stop* FindStop(const std::string_view stop_name) const;

    void SetDistanceBetweenStops(const Stop* from, const Stop* to, int distance);

    int GetDistanceBetweenStops(const Stop* from, const Stop* to) const;

    void AddBus(const Bus& bus);

    const Bus* FindBus(const std::string_view bus_name) const;

    BusInfo GetBusInfo(const std::string_view request) const;

    const std::set<std::string_view>& GetStopInfo(const std::string_view request) const;

private:
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairHasher, PairEqual> distance_between_stops_;

    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;

    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_busname_;
    
    std::size_t GetStopsOnRoute(const std::string_view request) const;

    std::size_t GetUniqueStops(const std::string_view& request) const;

    int GetRouteLength(const std::string_view& request) const;

    double GetCurvature(const std::string_view request) const;
};
} 