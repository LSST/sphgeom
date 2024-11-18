/*
 * This file is part of sphgeom.
 *
 * Developed for the LSST Data Management System.
 * This product includes software developed by the LSST Project
 * (http://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This software is dual licensed under the GNU General Public License and also
 * under a 3-clause BSD license. Recipients may choose which of these licenses
 * to use; please see the files gpl-3.0.txt and/or bsd_license.txt,
 * respectively.  If you choose the GPL option then the following text applies
 * (but note that there is still no warranty even if you opt for BSD instead):
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// \file
/// \brief This file contains tests for the ConvexPolygon class.

#include <vector>

#include "lsst/sphgeom/Box.h"
#include "lsst/sphgeom/Box3d.h"
#include "lsst/sphgeom/Circle.h"
#include "lsst/sphgeom/ConvexPolygon.h"

#include "test.h"


using namespace lsst::sphgeom;

typedef std::vector<UnitVector3d>::const_iterator VertexIterator;

void checkRelations(Region const& r1, Region const& r2, Relationship rel, TriState overlaps) {
    CHECK(r1.relate(r2) == rel);
    CHECK(r1.overlaps(r2) == overlaps);
}

void checkProperties(ConvexPolygon const & p) {
    CHECK(p.getVertices().size() >= 3);
    CHECK(p == p);
    CHECK(!(p != p));
    // A polygon should contain its vertices.
    for (VertexIterator v = p.getVertices().begin(), end = p.getVertices().end();
         v != end; ++v) {
        CHECK(p.contains(*v));
    }
    // A polygon should contain its centroid.
    CHECK(p.contains(p.getCentroid()));
    // The bounding circle and box for a polygon should
    // CONTAIN and INTERSECT the polygon.
    checkRelations(p.getBoundingCircle(), p, CONTAINS, TriState(true));
    checkRelations(p.getBoundingBox(), p, CONTAINS, TriState(true));
}

ConvexPolygon makeSimpleTriangle() {
    std::vector<UnitVector3d> points;
    points.push_back(UnitVector3d::X());
    points.push_back(UnitVector3d::Y());
    points.push_back(UnitVector3d::Z());
    return ConvexPolygon(points);
}

ConvexPolygon makeNgon(UnitVector3d const & center,
                       UnitVector3d const & v0,
                       size_t n)
{
    REQUIRE(center.dot(v0) > 1.5 * EPSILON);
    REQUIRE(n >= 3);
    std::vector<UnitVector3d> points;
    points.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        double f = static_cast<double>(i) / static_cast<double>(n);
        points.push_back(v0.rotatedAround(center, Angle(2.0 * PI) * f));
    }
    return ConvexPolygon(points);
}

TEST_CASE(Stream) {
    ConvexPolygon p = makeSimpleTriangle();
    std::stringstream ss;
    ss << p;
    CHECK(ss.str() == "{\"ConvexPolygon\": [[1, 0, 0], [0, 1, 0], [0, 0, 1]]}");
}

TEST_CASE(Clone) {
    ConvexPolygon p = makeSimpleTriangle();
    std::unique_ptr<Region> r(p.clone());
    REQUIRE(dynamic_cast<ConvexPolygon *>(r.get()) != 0);
    CHECK(*dynamic_cast<ConvexPolygon *>(r.get()) == p);
    CHECK(dynamic_cast<ConvexPolygon *>(r.get()) != &p);
}

TEST_CASE(CylicPermutation) {
    std::vector<UnitVector3d> points;
    points.push_back(UnitVector3d::X());
    points.push_back(UnitVector3d::Y());
    points.push_back(UnitVector3d::Z());
    ConvexPolygon p0 = ConvexPolygon(points);
    std::rotate(points.begin(), points.begin() + 1, points.end());
    ConvexPolygon p1 = ConvexPolygon(points);
    std::rotate(points.begin(), points.begin() + 1, points.end());
    ConvexPolygon p2 = ConvexPolygon(points);
    std::rotate(points.begin(), points.begin() + 1, points.end());
    ConvexPolygon p3 = ConvexPolygon(points);
    CHECK(p0 == p0);
    CHECK(p0 == p1);
    CHECK(p1 == p2);
    CHECK(p2 == p3);
}

TEST_CASE(Construction) {
    std::vector<UnitVector3d> points;
    points.push_back(UnitVector3d(1, 2, 1));
    points.push_back(UnitVector3d(1, 2, 1));
    points.push_back(UnitVector3d(2, 1, 1));
    points.push_back(UnitVector3d(2, 1, 1));
    points.push_back(UnitVector3d(1, 1, 2));
    points.push_back(UnitVector3d(1, 1, 2));
    points.push_back(UnitVector3d(1, 1, 1));
    points.push_back(UnitVector3d::Y());
    points.push_back(UnitVector3d(1, 1, 0));
    points.push_back(UnitVector3d(0, 1, 1));
    points.push_back(UnitVector3d(1, 0, 1));
    points.push_back(UnitVector3d::X());
    points.push_back(UnitVector3d::Z());
    ConvexPolygon p(points);
    CHECK(p.getVertices().size() == 3);
    if (p.getVertices().size() >= 3) {
        CHECK(p.getVertices()[0] == UnitVector3d::X());
        CHECK(p.getVertices()[1] == UnitVector3d::Y());
        CHECK(p.getVertices()[2] == UnitVector3d::Z());
    }
    checkProperties(p);
    for (VertexIterator v = points.begin(), end = points.end(); v != end; ++v) {
        CHECK(p.contains(*v));
    }
    CHECK(p.contains(UnitVector3d(1, 1, 1)));
    std::rotate(points.begin(), points.begin() + 3, points.end());
    CHECK(p == ConvexPolygon(points));
}

TEST_CASE(ConstructionFailure) {
    std::vector<UnitVector3d> points;
    points.push_back(UnitVector3d::Y());
    CHECK_THROW(ConvexPolygon::convexHull(points), std::invalid_argument);
    points.push_back(UnitVector3d::X());
    CHECK_THROW(ConvexPolygon::convexHull(points), std::invalid_argument);
    points.push_back(UnitVector3d::Z());
    points.push_back(UnitVector3d(-1, -1, -1));
    CHECK_THROW(ConvexPolygon::convexHull(points), std::invalid_argument);
}

TEST_CASE(Centroid) {
    ConvexPolygon p = makeSimpleTriangle();
    UnitVector3d c = p.getCentroid();
    CHECK(c.dot(UnitVector3d(1, 1, 1)) >= 1.0 - EPSILON);
}

TEST_CASE(CircleRelations) {
    ConvexPolygon p = makeSimpleTriangle();
    checkRelations(p, p.getBoundingCircle(), WITHIN, TriState(true));
    checkRelations(p.getBoundingCircle(), p, CONTAINS, TriState(true));
    checkRelations(p, Circle::full(), WITHIN, TriState(true));
    checkRelations(p, Circle::empty(), (CONTAINS | DISJOINT), TriState(false));
    checkRelations(p, Circle(UnitVector3d(1, 1, 1), 0.25), CONTAINS, TriState(true));
    checkRelations(p, Circle(UnitVector3d::X(), 1), INTERSECTS, TriState());
    checkRelations(p, Circle(UnitVector3d::Y(), 1), INTERSECTS, TriState());
    checkRelations(p, Circle(UnitVector3d::Z(), 1), INTERSECTS, TriState());
    checkRelations(p, Circle(-UnitVector3d::X(), 1), DISJOINT, TriState(false));
    checkRelations(p, Circle(-UnitVector3d::Y(), 1), DISJOINT, TriState(false));
    checkRelations(p, Circle(-UnitVector3d::Z(), 1), DISJOINT, TriState(false));
}

TEST_CASE(PolygonRelations1) {
    ConvexPolygon t = makeSimpleTriangle();
    std::vector<UnitVector3d> points;
    points.push_back(UnitVector3d::X());
    points.push_back(UnitVector3d::Y());
    points.emplace_back(1, 1, 1);
    ConvexPolygon p = ConvexPolygon::convexHull(points);
    checkRelations(p, p, (CONTAINS | WITHIN), TriState(true));
    checkRelations(t, p, CONTAINS, TriState(true));
    checkRelations(p, t, WITHIN, TriState(true));
}

TEST_CASE(PolygonRelations2) {
    // These are all degenerate cases where the intersection of
    // two polygons is an edge or an edge segment.
    ConvexPolygon t = makeSimpleTriangle();
    std::vector<UnitVector3d> points;
    points.emplace_back(1, 2, 0);
    points.emplace_back(2, 1, 0);
    points.push_back(-UnitVector3d::Z());
    ConvexPolygon p = ConvexPolygon::convexHull(points);
    checkRelations(p, t, INTERSECTS, TriState());
    checkRelations(t, p, INTERSECTS, TriState());
    points.clear();
    points.emplace_back(2, -1, 0);
    points.emplace_back(-1, 2, 0);
    points.push_back(-UnitVector3d::Z());
    p = ConvexPolygon::convexHull(points);
    checkRelations(p, t, INTERSECTS, TriState());
    checkRelations(t, p, INTERSECTS, TriState());
    points.clear();
    points.emplace_back(1, 1, 0);
    points.emplace_back(-1, 2, 0);
    points.push_back(-UnitVector3d::Z());
    p = ConvexPolygon::convexHull(points);
    checkRelations(p, t, INTERSECTS, TriState());
    checkRelations(t, p, INTERSECTS, TriState());
}

TEST_CASE(PolygonRelations3) {
    ConvexPolygon p1 = makeSimpleTriangle();
    ConvexPolygon p2 = makeNgon(UnitVector3d::X(), UnitVector3d(1, 1, 1), 3);
    std::vector<UnitVector3d> points;
    points.emplace_back(2, -1, 1);
    points.emplace_back(-1, 2, 1);
    points.emplace_back(2, 2, -1);
    ConvexPolygon p3 = ConvexPolygon::convexHull(points);
    checkRelations(p1, p2, INTERSECTS, TriState());
    checkRelations(p1, p3, INTERSECTS, TriState());
    checkRelations(p2, p3, INTERSECTS, TriState());
}

TEST_CASE(BoundingBox) {
    ConvexPolygon p = makeNgon(UnitVector3d::Z(), UnitVector3d(1, 1, 1), 4);
    Box b = p.getBoundingBox();
    Angle a = Angle::fromRadians(0.61547970867038734);
    CHECK(b.getLon().isFull());
    CHECK(b.getLat().getA() >= a - Angle(MAX_ASIN_ERROR));
    CHECK(b.getLat().getA() <= a);
    CHECK(b.getLat().getB() == Angle(0.5 * PI));
    p = makeNgon(-UnitVector3d::Z(), UnitVector3d(-1, -1, -1), 4);
    b = p.getBoundingBox();
    CHECK(b.getLon().isFull());
    CHECK(b.getLat().getA() == -Angle(0.5 * PI));
    CHECK(b.getLat().getB() >= -a);
    CHECK(b.getLat().getB() <= -a + Angle(MAX_ASIN_ERROR));
    p = makeNgon(UnitVector3d::Y(), UnitVector3d(1, 1, 1), 4);
    b = p.getBoundingBox();
    CHECK(b.getLon().getA() >= Angle(0.25 * PI - MAX_ASIN_ERROR));
    CHECK(b.getLon().getA() <= Angle(0.25 * PI));
    CHECK(b.getLon().getB() >= Angle(0.75 * PI));
    CHECK(b.getLon().getB() <= Angle(0.75 * PI + MAX_ASIN_ERROR));
    CHECK(b.getLat().getA() <= Angle(-0.25 * PI));
    CHECK(b.getLat().getA() >= Angle(-0.25 * PI - MAX_ASIN_ERROR));
    CHECK(b.getLat().getB() >= Angle(0.25 * PI));
    CHECK(b.getLat().getB() <= Angle(0.25 * PI + MAX_ASIN_ERROR));
}

TEST_CASE(BoundingBox3d) {
    ConvexPolygon p = makeSimpleTriangle();
    Box3d b = p.getBoundingBox3d();
    CHECK(b.x().getA() >= -1.0e-14 && b.x().getA() <= 0);
    CHECK(b.y().getA() >= -1.0e-14 && b.y().getA() <= 0);
    CHECK(b.z().getA() >= -1.0e-14 && b.z().getA() <= 0);
    CHECK(b.x().getB() == 1);
    CHECK(b.y().getB() == 1);
    CHECK(b.z().getB() == 1);
}

TEST_CASE(BoundingCircle) {
    ConvexPolygon p = makeSimpleTriangle();
    Circle c = p.getBoundingCircle();
    CHECK(c.contains(UnitVector3d::X()));
    CHECK(c.contains(UnitVector3d::Y()));
    CHECK(c.contains(UnitVector3d::Z()));
    CHECK(c.getCenter().dot(UnitVector3d(1, 1, 1)) >= 1.0 - EPSILON);
    double scl = 2.0 * (std::sqrt(3.0) - 1.0) / std::sqrt(3.0);
    CHECK(c.getSquaredChordLength() >= scl);
    CHECK(c.getSquaredChordLength() <=
          scl + 3.0 * MAX_SQUARED_CHORD_LENGTH_ERROR);
}

TEST_CASE(Codec) {
    ConvexPolygon p = makeNgon(UnitVector3d(1, -1, -1), UnitVector3d(2, -2, -1), 5);
    std::vector<uint8_t> buffer = p.encode();
    CHECK(*ConvexPolygon::decode(buffer) == p);
    std::unique_ptr<Region> r = Region::decode(buffer);
    CHECK(dynamic_cast<ConvexPolygon *>(r.get()) != nullptr);
    CHECK(*dynamic_cast<ConvexPolygon *>(r.get()) == p);
}

TEST_CASE(Hull) {
    std::vector<UnitVector3d> points = {
        UnitVector3d(0.9962891943972693, -0.06085984360495963, -0.06085984360495963),
        UnitVector3d(0.9950864205485712, -0.0607863703316013, 0.07815390471205888),
        UnitVector3d(0.9938879923679332, 0.07805978038285497, 0.07805978038285499),
        UnitVector3d(0.9950864205485711, 0.07815390471205892, -0.06078637033160138)
    };
    ConvexPolygon poly(points);
    CHECK(poly.getVertices().size() == 4);
    CHECK(poly.getVertices()[0] != poly.getVertices()[1]);
    CHECK(poly.getVertices()[1] != poly.getVertices()[2]);
    CHECK(poly.getVertices()[2] != poly.getVertices()[3]);
    CHECK(poly.getVertices()[3] != poly.getVertices()[0]);
}

TEST_CASE(Disjoint) {
    std::vector<UnitVector3d> points1 = {
        UnitVector3d(1.0, 0.0, -1.0),
        UnitVector3d(1.0, 0.0, 1.0),
        UnitVector3d(1.0, 1.0, 0.0)
    };
    std::vector<UnitVector3d> points2 = {
        UnitVector3d(-1.0, 1.0, 0.0),
        UnitVector3d(-1.0, -1.0, 0.0),
        UnitVector3d(-1.0, 0.0, 1.0)
    };
    ConvexPolygon poly1(points1);
    ConvexPolygon poly2(points2);
    checkRelations(poly1, poly2, DISJOINT, TriState(false));
}
