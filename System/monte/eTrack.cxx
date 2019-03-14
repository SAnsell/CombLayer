/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/eTrack.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <string>

#include "Exception.h"
#include "RefCon.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "particle.h"
#include "eTrack.h"

namespace MonteCarlo
{


eTrack::eTrack(const Geometry::Vec3D& Pt,
	       const Geometry::Vec3D& D) :
  particle(Pt,D)
  /*!
    Constructor 
    \param W :: Wavelength [Angstrom]
    \param Pt :: Position
    \param D :: Direction vector
  */
{}

eTrack::eTrack(const eTrack& A) :
  particle(A)
  /*!
    Copy constructor
    \param A :: eTrack to copy
  */
{}

eTrack&
eTrack::operator=(const eTrack& A)
  /*!
    Assignment operator
    \param A :: eTrack to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particle::operator=(A);
    }
  return *this;
}

void
eTrack::moveForward(const double Dist)
  /*!
    Move forward by the required distance
    -- This does not change the intercept points
    \param Dist :: Distance to move
  */
{
  Pos+=uVec*Dist;
  travel+=Dist;
  time+=Dist;
  return;
}


void
eTrack::write(std::ostream& OX) const
  /*!
    Write a given eTrack to a stream
    \param OX :: Output stream
   */
{
  OX<<Pos<<" u: "<<uVec 
    <<" W="<<weight<<" T="<<travel<<"("<<time<<")"<<" nCol="
    <<nCollision<<" (ID="<<ID<<")";
  return;
}

}  // NAMESPACE MonteCarlo
