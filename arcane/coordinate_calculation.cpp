#include <arcane/coordinate_calculation.h>

#include <math.h>
#include <assert.h>

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

double SquaredEuclideanDistance(const Coordinate& a, const Coordinate& b) {
    double diff_lon = a.lon - b.lon;
    double diff_lat = a.lat - b.lat;

    double sq_lon = diff_lon * diff_lon;
    double sq_lat = diff_lat * diff_lat;

    return sq_lon + sq_lat;
}



double HaversineDistance(const Coordinate& a, const Coordinate& b) {
    const double dlat1 = a.lat * detail::DEGREE_TO_RAD;
    const double dlong1 = a.lon * detail::DEGREE_TO_RAD;
    const double dlat2 = b.lat * detail::DEGREE_TO_RAD;
    const double dlong2 = b.lon * detail::DEGREE_TO_RAD;

    const double dlong = dlong1 - dlong2;
    const double dlat = dlat1 - dlat2;

    const double aharv = pow(sin(dlat / 2.0), 2.0) +
                         cos(dlat1) * cos(dlat2) * pow(sin(dlong / 2.), 2);
    const double charv = 2. * atan2(sqrt(aharv), sqrt(1.0 - aharv));
    return charv * detail::EARTH_RADIUS; 
}

double GreatCircleDistance(const Coordinate& a, const Coordinate& b) {
    const double float_lat1 = a.lat * detail::DEGREE_TO_RAD;
    const double float_lon1 = a.lon * detail::DEGREE_TO_RAD;
    const double float_lat2 = b.lat * detail::DEGREE_TO_RAD;
    const double float_lon2 = b.lon * detail::DEGREE_TO_RAD;

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

Coordinate centroid(const Coordinate& a, const Coordinate& b) {
    Coordinate centroid;
    centroid.lon = (a.lon + b.lon) / 2.0;
    centroid.lat = (a.lat + b.lat) / 2.0;
    return centroid;
}

double Bearing(const Coordinate& a, const Coordinate& b) {
    const double lon_diff = b.lon - a.lon;
    const double lon_delta = detail::degToRad(lon_diff);
    const double lat1 = detail::degToRad(a.lat);
    const double lat2 = detail::degToRad(b.lat);
    const double y = sin(lon_delta) * cos(lat2);
    const double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon_delta);
    double result = detail::radToDeg(atan2(y, x));
    while (result < 0.0) {
        result += 360.0;
    }
    while (result >= 360.0) {
        result -= 360.0;
    }
    // If someone gives us two identical coordinates, then the concept of a bearing
    // makes no sense.  However, because it sometimes happens, we'll at least
    // return a consistent value of 0 so that the behaviour isn't random.
    assert(a != b || result == 0.);

    return result;
}

std::pair<Coordinate, bool> CircleCenter(
        const Coordinate& C1,
        const Coordinate& C2,
        const Coordinate& C3) {
    // free after http://paulbourke.net/geometry/circlesphere/
    // require three distinct points
    if (C1 == C2 || C2 == C3 || C1 == C3) {
        return std::make_pair(Coordinate(), false);
    }

    // define line through c1, c2 and c2,c3
    const double C2C1_lat = C2.lat - C1.lat; // yDelta_a
    const double C2C1_lon = C2.lon - C1.lon; // xDelta_a
    const double C3C2_lat = C3.lat - C2.lat; // yDelta_b
    const double C3C2_lon = C3.lon - C2.lon; // xDelta_b

    // check for collinear points in X-Direction / Y-Direction
    if ((abs(C2C1_lon) < std::numeric_limits<double>::epsilon() &&
         abs(C3C2_lon) < std::numeric_limits<double>::epsilon()) ||
        (abs(C2C1_lat) < std::numeric_limits<double>::epsilon() &&
         abs(C3C2_lat) < std::numeric_limits<double>::epsilon())) {
        return std::make_pair(Coordinate(), false);
    } else if (abs(C2C1_lon) < std::numeric_limits<double>::epsilon()) {
        // vertical line C2C1
        // due to c1.lon == c2.lon && c1.lon != c3.lon we can rearrange this way
        assert(abs(C3.lon - C1.lon) >= std::numeric_limits<double>::epsilon() &&
               abs(C2.lon - C3.lon) >= std::numeric_limits<double>::epsilon());
        return CircleCenter(C1, C3, C2);
    } else if (abs(C3C2_lon) < std::numeric_limits<double>::epsilon()) {
        // vertical line C3C2
        // due to c2.lon == c3.lon && c1.lon != c3.lon we can rearrange this way
        // after rearrangement both deltas will be zero
        assert(abs(C1.lon - C2.lon) >= std::numeric_limits<double>::epsilon() &&
               abs(C3.lon - C1.lon) >= std::numeric_limits<double>::epsilon());
        return CircleCenter(C2, C1, C3);
    } else {
        const double C2C1_slope = C2C1_lat / C2C1_lon;
        const double C3C2_slope = C3C2_lat / C3C2_lon;

        if (abs(C2C1_slope) < std::numeric_limits<double>::epsilon()) {
            // Three non-collinear points with C2,C1 on same latitude.
            // Due to the x-values correct, 
            // we can swap C3 and C1 to obtain the correct slope value
            return CircleCenter(C3, C2, C1);
        }
        // valid slope values for both lines, calculate the center as intersection of the lines
        // can this ever happen?
        if (abs(C2C1_slope - C3C2_slope) < std::numeric_limits<double>::epsilon()) {
            return std::make_pair(Coordinate(), false);
        }

        const double C1_y = C1.lat;
        const double C1_x = C1.lon;
        const double C2_y = C2.lat;
        const double C2_x = C2.lon;
        const double C3_y = C3.lat;
        const double C3_x = C3.lon;

        const double lon = (C2C1_slope * C3C2_slope * (C1_y - C3_y) + C3C2_slope * (C1_x + C2_x) -
                            C2C1_slope * (C2_x + C3_x)) /
                           (2 * (C3C2_slope - C2C1_slope));
        const double lat = (0.5 * (C1_x + C2_x) - lon) / C2C1_slope + 0.5 * (C1_y + C2_y);
        if (lon < -180.0 || lon > 180.0 || lat < -90.0 || lat > 90.0) {
            return std::make_pair(Coordinate(), false);
        } else {
            return std::make_pair(Coordinate(lon, lat), true);
        }
    }
}

std::pair<double, bool> CircleRadius(
        const Coordinate& C1,
        const Coordinate& C2,
        const Coordinate& C3) {
    auto res = std::make_pair(std::numeric_limits<double>::infinity(), false);
    auto center = CircleCenter(C1, C2, C3);
    if (center.second) {
        res.first = HaversineDistance(C1, center.first);
        res.second = true;
    }
    return res;
}

Coordinate InterpolateLinear(
        double factor, 
        const Coordinate& from, 
        const Coordinate& to) {
    assert(0 <= factor && factor <= 1.0);
    const double lon = from.lon + factor * (to.lon - from.lon);
    const double lat = from.lat + factor * (to.lat - from.lat);
    return Coordinate(lon, lat);
}

Coordinate Difference(const Coordinate& a, const Coordinate& b) {
    const double lon = a.lon - b.lon;
    const double lat = a.lat - b.lat;
    return Coordinate(lon, lat);
}

} // namespace arcane

