/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/activeUnit.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream> 
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <fmt/core.h>

#include "Exception.h"
#include "Random.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "mathSupport.h"
#include "activeUnit.h"

namespace SDef
{

activeUnit::activeUnit(const double IF,
                       std::vector<double>  E,
                       std::vector<double>  G) :
  volume(0.0),integralFlux(IF),
  scaleCnt(0),scaleIntegral(0.0),
  energy(std::move(E)),cellFlux(std::move(G))
  /*!
    Constructor 
    \param IF :: integral flux
    \param E :: Energy
    \param G :: gamma value
  */
{}

  
activeUnit::activeUnit(const activeUnit& A) :
  volume(A.volume),integralFlux(A.integralFlux),
  scaleCnt(A.scaleCnt),scaleIntegral(A.scaleIntegral),
  energy(A.energy),cellFlux(A.cellFlux)
  /*!
    Copy Constructor 
    \param A :: activeUnit to copy
  */
{}

activeUnit&
activeUnit::operator=(const activeUnit& A)
  /*!
    Assignement operator
    \param A :: activeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      volume=A.volume;
      integralFlux=A.integralFlux;
      scaleCnt=A.scaleCnt;
      scaleIntegral=A.scaleIntegral;
      energy=A.energy;
      cellFlux=A.cellFlux;

    }
  return *this;
}  

activeUnit::~activeUnit() 
  /*!
    Destructor
  */
{}


void
activeUnit::normalize(const double fracValue,
		      const double V)
  /*!
    Normalize and integrate the volumes
    \param fracValue :: fraction ratio of volume in sampled volume
    \param V :: Volume of object
   */
{
  ELog::RegMethod RegA("activeUnit","normalize");

  double normFlux=0.0;

  std::vector<double> YOut;
  for(const double& FV : cellFlux)
    normFlux+=FV;

  if (normFlux>Geometry::zeroTol)
    {
      double prevSum(0.0);
      for(double& FV : cellFlux)
	{
	  prevSum+=FV;
	  FV=prevSum/normFlux;
	}
    }
  integralFlux*=fracValue;
  volume=V;
  return;
}

double
activeUnit::XInverse(const double R) const
  /*!
    Inverse the y data [as based]
    \param R :: Value to search for             
    \return value at E(R) 
  */
{
  ELog::RegMethod RegA("activeUnit","XInverse");
  
  long int index=indexPos(cellFlux,R);
  const size_t IX=static_cast<size_t>(index);
  if (index<0 || IX>=cellFlux.size())
    throw ColErr::RangeError<double>(R,cellFlux.front(),
                                     cellFlux.back(),"value out of range");
  
  const double yA=cellFlux[IX];
  const double yB=cellFlux[IX+1];

  const double frac=(R-yA)/(yB-yA);

  return energy[IX]+frac*(energy[IX+1]-energy[IX]);
}

void
activeUnit::zeroScale()
  /*!
    Aero the scaleIntegral
  */
{
  scaleIntegral=0.0;
  scaleCnt=0;
  return;
}
  
void
activeUnit::addScaleSum(const double S)
  /*!
    Add a integral contribution to sum
    \param S :: value to add
   */
{
  scaleCnt++;
  scaleIntegral+=S;
  return;
}

double
activeUnit::getScaleFlux() const
  /*!
    return normalized scale flux value
    \return flux / npoints
  */
{
  return (scaleCnt) ? scaleIntegral/static_cast<double>(scaleCnt) : 1.0;
}

  
  
void
activeUnit::writePhoton(std::ostream& OX,const Geometry::Vec3D& Pt,
			const double weight) const
  /*!
    Calculate the energy based on RNG nubmer and 
    write a photon in a random direction
    \param OX :: Output stream
    \param Pt :: Point for interaction
    \param weight :: External Scaling factor 
  */
{
  const double thetaAngle=2*M_PI*Random::rand();
  const double z=2.0*(Random::rand()-0.5);
  const double sinZ=sqrt(1-z*z);
  const Geometry::Vec3D uvw(sinZ*cos(thetaAngle),sinZ*sin(thetaAngle),z);

  const double R=Random::rand();
  const double E=XInverse(R);

  //  OX<<(FMT % Pt.X() % Pt.Y() % Pt.Z())<<std::endl;
  if (E>1e-3)  // above threshold
    {
      
      OX<<"2 "<<fmt::format(" {:<12.6e}  {:<12.6e}  {:<12.6e}",
			    Pt.X(),Pt.Y(),Pt.Z());
      OX<<" "<<fmt::format(" {:<12.6e}  {:<12.6e}  {:<12.6e}",
			   uvw.X(),uvw.Y(),uvw.Z());
      
      OX<<" "<<fmt::format(" {:<12.6e}  {:<12.6e}",
			   E,(weight*getScaleFlux()*integralFlux))
	<<std::endl;
    }
  return;
}
    
} // NAMESPACE SDef
