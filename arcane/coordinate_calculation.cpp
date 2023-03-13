#include <arcane/coordinate_calculation.h>

#include <math.h>

namespace arcane {

namespace detail {

constexpr const double PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651;
constexpr const double DEGREE_TO_RAD = 0.017453292519943295769236907684886;
constexpr const double RAD_TO_DEGREE = 1. / DEGREE_TO_RAD;
// earth radius varies between 6,356.750-6,378.135 km (3,949.901-3,963.189mi)
// The IUGG value for the equatorial radius is 6378.137 km (3963.19 miles)
constexpr const double EARTH_RADIUS = 6372797.560856;

double degToRad(const double degree) {
    return degree * (PI / 180.0);
}

double radToDeg(const double radian) {
    return radian * (180.0 * (1. / PI));
}

} // namespace detail

double SquaredEuclideanDistance(const Coordinate& lhs, const Coordinate& rhs) {
    double d_lon = lhs.lon - rhs.lon;
    double d_lat = lhs.lat - rhs.lat;

    double sq_lon = d_lon * d_lon;
    double sq_lat = d_lat * d_lat;

    return sq_lon + sq_lat;
}

double HaversineDistance(const Coordinate& coordinate_1, const Coordinate& coordinate_2) {
    const double dlat1 = coordinate_1.lat * detail::DEGREE_TO_RAD;
    const double dlong1 = coordinate_1.lon * detail::DEGREE_TO_RAD;
    const double dlat2 = coordinate_2.lat * detail::DEGREE_TO_RAD;
    const double dlong2 = coordinate_2.lon * detail::DEGREE_TO_RAD;

    const double dlong = dlong1 - dlong2;
    const double dlat = dlat1 - dlat2;

    const double aharv = pow(sin(dlat / 2.0), 2.0) +
                         cos(dlat1) * cos(dlat2) * pow(sin(dlong / 2.), 2);
    const double charv = 2. * atan2(sqrt(aharv), sqrt(1.0 - aharv));
    return charv * detail::EARTH_RADIUS; 
}

double GreatCircleDistance(const Coordinate& coordinate_1, const Coordinate& coordinate_2) {
    const double float_lat1 = coordinate_1.lat * detail::DEGREE_TO_RAD;
    const double float_lon1 = coordinate_1.lon * detail::DEGREE_TO_RAD;
    const double float_lat2 = coordinate_2.lat * detail::DEGREE_TO_RAD;
    const double float_lon2 = coordinate_2.lon * detail::DEGREE_TO_RAD;

    const double x_value = (float_lon2 - float_lon1) * cos((float_lat1 + float_lat2) / 2.0);
    const double y_value = float_lat2 - float_lat1;
    return hypot(x_value, y_value) * detail::EARTH_RADIUS;
}

std::pair<double, Coordinate> ProjectPointOnSegment(
        const Coordinate& source, 
        const Coordinate& target, 
        const Coordinate& coordinate) {
    const Coordinate slope_vector(target.lon - source.lon, target.lat - source.lat);
    const Coordinate rel_coordinate(coordinate.lon - source.lon, coordinate.lat - source.lat);
    // dot product of two un-normed vectors
    const double unnormed_ratio = (slope_vector.lon * rel_coordinate.lon) + 
                                  (slope_vector.lat * rel_coordinate.lat);
    // squared length of the slope vector
    const double squared_length = (slope_vector.lon * slope_vector.lon) +
                                  (slope_vector.lat * slope_vector.lat);

    if (squared_length < std::numeric_limits<double>::epsilon()) {
        return std::make_pair(0, source);
    }

    const double normed_ratio = unnormed_ratio / squared_length;
    double clamped_ratio = normed_ratio;
    if (clamped_ratio > 1.) {
        clamped_ratio = 1.;
    } else if (clamped_ratio < 0.) {
        clamped_ratio = 0.;
    }

    double lon_x = (1.0 - clamped_ratio) * source.lon + target.lon * clamped_ratio;
    double lat_x = (1.0 - clamped_ratio) * source.lat + target.lat * clamped_ratio;

    return std::make_pair(clamped_ratio, Coordinate(lon_x, lat_x));
}

double FindClosestDistance(
        const Coordinate& coordinate,
        const Coordinate& segment_begin,
        const Coordinate& segment_end) {
    return HaversineDistance(
            coordinate, 
            ProjectPointOnSegment(segment_begin, segment_end, coordinate).second);
}

double SignedArea(
        const Coordinate& first_coordinate,
        const Coordinate& second_coordinate,
        const Coordinate& third_coordinate) {
    const double lat_1 = first_coordinate.lat;
    const double lon_1 = first_coordinate.lon;
    const double lat_2 = second_coordinate.lat;
    const double lon_2 = second_coordinate.lon;
    const double lat_3 = third_coordinate.lat;
    const double lon_3 = third_coordinate.lon;
    return 0.5 * (-lon_2 * lat_1 + lon_3 * lat_1 + lon_1 * lat_2 - 
            lon_3 * lat_2 - lon_1 * lat_3 + lon_2 * lat_3);
}

bool IsCounterclockwise(
        const Coordinate& first_coordinate,
        const Coordinate& second_coordinate,
        const Coordinate& third_coordinate) {
    return SignedArea(first_coordinate, second_coordinate, third_coordinate) > 0;
}

Coordinate RotateCounterclockwiseAroundZero(
        const Coordinate& coordinate, 
        double angle_in_radians) {
    // counterclockwise a rotation  around 0,0 in vector space is defined as
    // 
    // | cos a   -sin a | . | lon |
    // | sin a    cos a |   | lat |
    //
    // resulting in cos a lon - sin a lon for the new longitude and sin a lon + cos a lat for the
    // new latitude

    const double cos_alpha = cos(angle_in_radians);
    const double sin_alpha = sin(angle_in_radians);

    const double lon = cos_alpha * coordinate.lon - sin_alpha * coordinate.lat;
    const double lat = sin_alpha * coordinate.lon + cos_alpha * coordinate.lat;

    return Coordinate(lon, lat);
}

} // namespace arcane

