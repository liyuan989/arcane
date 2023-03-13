#ifndef ARCANE_COORDINATE_CALCULATION_H
#define ARCANE_COORDINATE_CALCULATION_H

#include <stdint.h>
#include <limits>
#include <algorithm>

#include <arcane/coordinate.h>

namespace arcane {

//Takes the squared euclidean distance of the input coordinates. Does not return meters
double SquaredEuclideanDistance(const Coordinate& a, const Coordinate& b);

double ManhattanDistance(const Coordinate& a, const Coordinate& b);

double HaversineDistance(const Coordinate& a, const Coordinate& b);

double GreatCircleDistance(const Coordinate& a, const Coordinate& b);

// get the length of a full coordinate vector, 
// using one of our basic functions to compute distances
template <typename BinaryOperation, typename Iterator>
double GetLength(Iterator begin, const Iterator end, BinaryOperation op) {
    double result = 0;
    const auto functor = [&result, op](const Coordinate& lhs, const Coordinate& rhs) {
        result += op(lhs, rhs);
        return false;
    };
    // side-effect find adding up distances
    std::adjacent_find(begin, end, functor);
    return result;
}

// Find the closest distance and location between coordinate and the line connecting source and
// target:
//             coordinate
//                 |
//                 |
// source -------- x -------- target.
// returns x as well as the distance between source and x as ratio ([0,1])
std::pair<double, Coordinate> ProjectPointOnSegment(
        const Coordinate& source, 
        const Coordinate& target, 
        const Coordinate& coordinate);

// find the closest distance between a coordinate and a segment
// O(1)
double FindClosestDistance(
        const Coordinate& coordinate,
        const Coordinate& segment_begin,
        const Coordinate& segment_end);

// find the closest distance between a coordinate and a set of coordinates
// O(|coordinates|)
template <typename Iterator>
double FindClosestDistance(
        const Coordinate& coordinate,
        const Iterator begin,
        const Iterator end) {
    double current_min = std::numeric_limits<double>::max();

    // comparator updating current_min without ever finding an element
    const auto compute_minimum_distance = [&current_min, coordinate](const Coordinate& lhs,
                                                                     const Coordinate& rhs) {
        current_min = std::min(current_min, FindClosestDistance(coordinate, lhs, rhs));
        return false;
    };

    std::adjacent_find(begin, end, compute_minimum_distance);
    return current_min;
}

// find the closes distance between two sets of coordinates
// O(|lhs| * |rhs|)
template <typename Iterator>
double FindClosestDistance(
        const Iterator lhs_begin,
        const Iterator lhs_end,
        const Iterator rhs_begin,
        const Iterator rhs_end) {
    double current_min = std::numeric_limits<double>::max();

    const auto compute_minimum_distance_in_rhs = [&current_min, rhs_begin, rhs_end](
        const Coordinate& coordinate) {
        current_min = std::min(current_min, findClosestDistance(coordinate, rhs_begin, rhs_end));
        return false;
    };

    std::find_if(lhs_begin, lhs_end, compute_minimum_distance_in_rhs);
    return current_min;
}

// compute the signed area of a triangle
double SignedArea(
        const Coordinate& first_coordinate,
        const Coordinate& second_coordinate,
        const Coordinate& third_coordinate);

// check if a set of three coordinates is given in counterclockwise order
bool IsCounterclockwise(
           const Coordinate& first_coordinate,
           const Coordinate& second_coordinate,
           const Coordinate& third_coordinate);

// counterclockwise rotates a coordinate around the point (0,0). 
// This function can be used to normalise a few
// computations around regression vectors
Coordinate RotateCounterclockwiseAroundZero(
        const Coordinate& coordinate, 
        double angle_in_radians);

Coordinate Centroid(const Coordinate& a, const Coordinate& b);

double Bearing(const Coordinate& a, const Coordinate& b);

// find the center of a circle through three coordinates
std::pair<Coordinate, bool> CircleCenter(
        const Coordinate& first_coordinate,
        const Coordinate& second_coordinate,
        const Coordinate& third_coordinate);

// find the radius of a circle through three coordinates
std::pair<double, bool> CircleRadius(
        const Coordinate& first_coordinate,
        const Coordinate& second_coordinate,
        const Coordinate& third_coordinate);

// factor in [0,1]. Returns point along the straight line between from and to. 
// 0 returns from, 1 returns to
Coordinate InterpolateLinear(
        double factor, 
        const Coordinate& from, 
        const Coordinate& to);

// compute the difference vector of two coordinates a - b
Coordinate Difference(const Coordinate& a, const Coordinate& b);

} // namespace arcane

#endif

