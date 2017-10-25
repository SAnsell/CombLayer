/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/VolumeBeam.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <complex>
#include <map>
#include <vector>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "neutron.h"
#include "Detector.h"
#include "Beam.h"
#include "VolumeBeam.h"

extern MTRand RNG;

namespace Transport
{

VolumeBeam::VolumeBeam() : 
  Beam(),wavelength(0.7),Corner(0,0,0),
  X(1,0,0),Y(0,1,0),Z(0,0,1),
  yBias(-1.0)
  /*!
    Constructor
  */
{}

VolumeBeam::VolumeBeam(const VolumeBeam& A) : 
  Beam(A),wavelength(A.wavelength),Corner(A.Corner),X(A.X),Y(A.Y),
  Z(A.Z),yBias(A.yBias)
  /*!
    Copy constructor
    \param A :: VolumeBeam to copy
  */
{}

VolumeBeam&
VolumeBeam::operator=(const VolumeBeam& A)
  /*!
    Assignment operator
    \param A :: VolumeBeam to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Beam::operator=(A);
      wavelength=A.wavelength;
      Corner=A.Corner;
      X=A.X; 
      Y=A.Y;
      Z=A.Z;
      yBias=A.yBias;
    }
  return *this;
}

VolumeBeam::~VolumeBeam()
  /*!
    Destructor
  */
{}

const Geometry::Vec3D& 
VolumeBeam::getAxis(const int Index) const
  /*!
    Access a given axis
    \param Index :: Index value (0-2)
    \return Axis
  */
{
  if (Index<0 || Index>2)
    throw ColErr::IndexError<int>(Index,2,"VolumeBeam::getAxis");
  if (!Index) return X;
  return (Index==1) ? Y : Z;
}

void
VolumeBeam::setCorners(const Geometry::Vec3D& LC,
		       const Geometry::Vec3D& TC)
  /*!
    Set the corners : LC and TC. Assumes orthoganal alignment
    \param LC :: Lower corner
    \param TC :: Upper corner
   */
{
  ELog::RegMethod RegA("VolumeBeam","setCorners");
  Corner=LC;
  Geometry::Vec3D Diff=TC-LC;
  for(int i=0;i<3;i++)
    {
      if (Diff.abs()<Geometry::zeroTol)
        {
	  ELog::EM<<"Volume insufficiently big:"<<LC<<" to "<<TC<<ELog::endErr;
	  X=Geometry::Vec3D(1,0,0);
	  Y=Geometry::Vec3D(0,1,0);
	  Z=Geometry::Vec3D(0,0,1);
	  return;
	}
    }

  X=Y=Z=Geometry::Vec3D(0,0,0);
  X[0]=Diff[0];
  Y[1]=Diff[1];
  Z[2]=Diff[2];
  return;
}

MonteCarlo::neutron
VolumeBeam::generateNeutron() const
  /*!
    Return the neutron : 
    Note that beam travels in the x direction
    \return Randomize point
  */
{
  ELog::RegMethod RegA("VolumeBeam","generateNeutron");

  const double theta=2.0*M_PI*RNG.rand();
  const double phi=M_PI*RNG.rand();
  Geometry::Vec3D uV(cos(theta)*sin(phi),sin(theta)*sin(phi),
		     cos(phi));
  MonteCarlo::neutron Out(wavelength,Corner,uV);
  // Weighting based on the cos() factors of the centroid probability:
  Geometry::Vec3D NLocal(Corner);   // local position of the neutron
  
  double xfrac=RNG.rand();
  Out.weight*=cos( (xfrac-0.5)*M_PI );
  NLocal+=X*xfrac;
  xfrac=RNG.rand();
  Out.weight*=cos( (xfrac-0.5)*M_PI );
  NLocal+=Z*xfrac;
  // Y is special
  xfrac=RNG.rand();
  Out.weight*=cos( (xfrac-0.5)*M_PI );
  NLocal+=Y*xfrac;
  if (yBias>0.0)
    {
      const double cpower=(1.0-yBias)+yBias*cos( 0.5*xfrac*M_PI );
      Out.weight*=pow(cpower,yBias);
    }
  Out.Pos=NLocal;
  
  

  return Out;
}

void 
VolumeBeam::write(std::ostream& OX) const
  /*!
    Write some information to an output stream
    \param OX :: Output Stream
   */
{
  OX<<"# Wavelength = "<<wavelength<<" "
    <<Corner<<" :: "<<Corner+X+Y+Z<<std::endl;

  return;
}
 
} // NAMESPACE Transport
