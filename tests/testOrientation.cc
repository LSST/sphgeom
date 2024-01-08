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
/// \brief This file contains tests for the orientation function.

#include "lsst/sphgeom/orientation.h"

#include "test.h"

using namespace lsst::sphgeom;

void checkOrientation(UnitVector3d const & v0,
                      UnitVector3d const & v1,
                      UnitVector3d const & v2,
                      int expectedOrientation)
{
    CHECK(orientation(v0, v1, v2) == expectedOrientation);
    CHECK(orientationExact(v0, v1, v2) == expectedOrientation);
    if (v0.y() == 0.0 && v0.z() == 0.0) {
        if (v0.x() > 0.0) {
            CHECK(orientationX(v1, v2) == expectedOrientation);
        } else {
            CHECK(-orientationX(v1, v2) == expectedOrientation);
        }
    }
    if (v0.x() == 0.0 && v0.z() == 0.0) {
        if (v0.y() > 0.0) {
            CHECK(orientationY(v1, v2) == expectedOrientation);
        } else {
            CHECK(-orientationY(v1, v2) == expectedOrientation);
        }
    }
    if (v0.x() == 0.0 && v0.y() == 0.0) {
        if (v0.z() > 0.0) {
            CHECK(orientationZ(v1, v2) == expectedOrientation);
        } else {
            CHECK(-orientationZ(v1, v2) == expectedOrientation);
        }
    }
}

void testOrientation(UnitVector3d const & v0,
                     UnitVector3d const & v1,
                     UnitVector3d const & v2,
                     int expectedOrientation)
{
    checkOrientation(v0, v1, v2, expectedOrientation);
    checkOrientation(v0, v1, v2, expectedOrientation);
    checkOrientation(v1, v2, v0, expectedOrientation);
    checkOrientation(v2, v0, v1, expectedOrientation);
    checkOrientation(v1, v0, v2, -expectedOrientation);
    checkOrientation(v0, v2, v1, -expectedOrientation);
    checkOrientation(v2, v1, v0, -expectedOrientation);
    checkOrientation(v0, v0, v1, 0);
    checkOrientation(v0, -v0, v1, 0);
    checkOrientation(v0, v1, v1, 0);
    checkOrientation(v0, v1, -v1, 0);
    checkOrientation(v0, v1, v0, 0);
    checkOrientation(v0, v1, -v0, 0);
}

TEST_CASE(Orientation) {
    testOrientation(
        UnitVector3d::X(), UnitVector3d::Y(), UnitVector3d::Z(), 1);
    testOrientation(
        -UnitVector3d::X(), -UnitVector3d::Y(), -UnitVector3d::Z(), -1);
}

TEST_CASE(OrientationUnderflow) {
    UnitVector3d v0 = UnitVector3d::X();
    UnitVector3d v1 = UnitVector3d::fromNormalized(1.0, 1.0e-300, 0.0);
    UnitVector3d v2 = UnitVector3d::fromNormalized(1.0, 0.0, 1.0e-300);
    testOrientation(v0, v1, v2, 1);
}

TEST_CASE(OrientationOverflow) {
    Vector3d v0(1.0e300, 0, 0);
    Vector3d v1(1.0e300, 1.0e300, 0.0);
    Vector3d v2(1.0e300, 0.0, 1.0e300);
    CHECK(orientationExact(v0, v1, v2) == 1);
}
