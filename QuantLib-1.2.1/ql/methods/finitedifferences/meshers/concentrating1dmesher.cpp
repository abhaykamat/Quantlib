/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2009 Ralph Schreyer

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file concentrating1dmesher.cpp
    \brief One-dimensional grid mesher concentrating around critical points
*/

#include <ql/errors.hpp>
#include <ql/utilities/null.hpp>
#include <ql/methods/finitedifferences/meshers/concentrating1dmesher.hpp>
#include <cmath>

// asinh is missing in WIN32 (and possibly on other compilers)
#if !defined(QL_HAVE_ASINH)
#define asinh(x) std::log(x + std::sqrt(x * x + 1))
#endif

namespace QuantLib {

    Concentrating1dMesher::Concentrating1dMesher(
        Real start, Real end, Size size, const std::pair<Real, Real>& cPoints)
    : Fdm1dMesher(size) {

        QL_REQUIRE(end > start, "end must be larger than start");

        const Real cPoint  = cPoints.first;
        const Real density = cPoints.second == Null<Real>() ?
                             Null<Real>() : cPoints.second*(end-start);

        QL_REQUIRE(    cPoint == Null<Real>()
                   || (cPoint >= start && cPoint <= end),
                   "cPoint must be between start and end");
        QL_REQUIRE(density == Null<Real>() || density > 0.0,
                   "density > 0 required" );

        const Real dx = 1.0/(size-1);
        for (Size i=1; i < size-1; ++i) {
            if(cPoint != Null<Real>()) {
                const Real c1 = asinh((start-cPoint)/density);
                const Real c2 = asinh((end-cPoint)/density);
                locations_[i] = cPoint
                    + density*std::sinh(c1*(1.0-i*dx)+c2*i*dx);
            }
            else {
                locations_[i] = start + i*dx*(end-start);
            }
        }
        locations_.front() = start;
        locations_.back() = end;

        for (Size i=0; i < size-1; ++i) {
            dplus_[i] = dminus_[i+1] = locations_[i+1] - locations_[i];
        }
        dplus_.back() = dminus_.front() = Null<Real>();
    }
}
