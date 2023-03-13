#ifndef ARCANE_COORDINATE_H
#define ARCANE_COORDINATE_H

#include <iostream>
#include <iomanip>
#include <limits>

namespace arcane {

struct Coordinate {
    double lon;
    double lat;

    Coordinate()
        : lon(std::numeric_limits<double>::min()),
          lat(std::numeric_limits<double>::min()) {
    }

    Coordinate(double longitude, double latitude)
        : lon(longitude),
          lat(latitude) {
    }

    Coordinate(const Coordinate& other)
        : lon(other.lon),
          lat(other.lat) {
    }

    Coordinate& operator=(const Coordinate& other) {
        lon = other.lon;
        lat = other.lat;
        return *this;
    }

    bool IsValid() const {
        return !(lon > 180.0 || lon < -180.0 || lat > 90.0 || lat < -90.0);
    }

    friend bool operator==(const Coordinate& lhs, const Coordinate& rhs);
    friend bool operator!=(const Coordinate& lhs, const Coordinate& rhs);
    friend std::ostream& operator<<(std::ostream& out, const Coordinate& coordinate);
};

inline bool operator==(const Coordinate& lhs, const Coordinate& rhs) {
    return lhs.lat == rhs.lat && lhs.lon == rhs.lon;
}

inline bool operator!=(const Coordinate& lhs, const Coordinate& rhs) { 
    return !(lhs == rhs); 
}

inline std::ostream& operator<<(std::ostream& out, const Coordinate& coordinate) {
    out << std::setprecision(12) 
        << "(lon:" << coordinate.lon << ", lat:" << coordinate.lat << ")";
    return out;
}

} // namespace arcane

#endif

