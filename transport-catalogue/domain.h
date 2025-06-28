#pragma once

#include <string>
#include <vector>

#include "geo.h"

namespace domain {

    using namespace geo;

    struct Stop {
        std::string name;
        Coordinates coordinates;

        bool operator<(const Stop& other) const {
            return name < other.name;
        }
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
        bool is_roundtrip;
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

}  // namespace domain