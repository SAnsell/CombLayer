/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/photon.cxx
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
#include "particleConv.h"
#include "particle.h"
#include "photon.h"

namespace MonteCarlo
{

photon::photon(const double W,const Geometry::Vec3D& Pt,
	       const Geometry::Vec3D& D) :
  particle("photon",Pt,D,particleConv::Instance().mcplWavelengthKE(22,W))
  /*!
    Constructor 
    \param W :: Wavelength [Angstrom]
-    \param Pt :: Position
    \param D :: Direction vector
  */
{}

photon::photon(const Geometry::Vec3D& Pt,
	       const Geometry::Vec3D& D,
	       const double E) :
  particle("photon",Pt,D,E)
  /*!
    Constructor 
    \param Pt :: Position
    \param D :: Direction vector
    \param E :: energy [keV]
  */
{}
  
photon::photon(const photon& A) :
  particle(A)
  /*!
    Copy constructor
    \param A :: photon to copy
  */
{}

photon&
photon::operator=(const photon& A)
  /*!
    Assignment operator
    \param A :: photon to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particle::operator=(A);
    }
  return *this;
}





}  // NAMESPACE MonteCarlo
