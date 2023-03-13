#ifndef ARCANE_COORDINATE_TRANSFORM
#define ARCANE_COORDINATE_TRANSFORM

#include <string>

#include <arcane/coordinate.h>

namespace arcane {

// transform coordinate system among wgs84, gcj02, bd09ll
Coordinate CoordinateTransform(
        const Coordinate& coordinate,
        const std::string& from,
        const std::string& to);

} // namespace arcane

#endif

