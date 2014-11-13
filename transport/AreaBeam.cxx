/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transport/AreaBeam.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <complex>
#include <vector>

#include "MersenneTwister.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "neutron.h"
#include "Beam.h"
#include "AreaBeam.h"

extern MTRand RNG;

namespace Transport
{

AreaBeam::AreaBeam() : 
  Beam(),Cent(0,0,0),
  Axis(0,1,0),WVec(1,0,0),HVec(0,0,1),
  wavelength(0.7),Width(0.4),
  Height(5.0),startY(-10)
  /*!
    Constructor
  */
{}

AreaBeam::AreaBeam(const AreaBeam& A) : 
  Beam(A),
  Cent(A.Cent),Axis(A.Axis),WVec(A.WVec),HVec(A.HVec),
  wavelength(A.wavelength),Width(A.Width),Height(A.Height),
  startY(A.startY)
  /*!
    Copy constructor
    \param A :: AreaBeam to copy
  */
{}

AreaBeam&
AreaBeam::operator=(const AreaBeam& A)
  /*!
    Assignment operator
    \param A :: AreaBeam to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Beam::operator=(A);
      Cent=A.Cent;
      Axis=A.Axis;
      WVec=A.WVec;
      HVec=A.HVec;
      wavelength=A.wavelength;
      Width=A.Width;
      Height=A.Height;
      startY=A.startY;
    }
  return *this;
}

AreaBeam::~AreaBeam()
  /*!
    Destructor
  */
{}

MonteCarlo::neutron
AreaBeam::generateNeutron() const
  /*!
    Return the Point of the neutron.
    Note that beam travels in the x direction
    \return Randomize point
  */
{
  const Geometry::Vec3D CP=Cent+
    WVec*(RNG.rand()-0.5)*Width*2.0+
    HVec*(RNG.rand()-0.5)*Height*2.0+
    Axis*startY;
  return MonteCarlo::neutron(wavelength,CP,Axis);
}

void 
AreaBeam::write(std::ostream& OX) const
  /*!
    Write some information to an output stream
    \param OX :: Output Stream
   */
{
  OX<<"# Wavelength = "<<wavelength<<std::endl;
  OX<<"# XZ = "<<Width<<" "<<Height<<" from "<<startY<<std::endl;
  return;
}
  


} // NAMESPACE MonteCarlo
