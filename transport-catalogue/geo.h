#pragma once

#include <string>

namespace geo {

    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    struct Distance {
        int distance;
        std::string stop_name;
    };

    double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo