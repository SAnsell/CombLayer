/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/activeUnit.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <boost/filesystem.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "WorkData.h"
#include "activeUnit.h"

extern MTRand RNG;

namespace SDef
{

activeUnit::activeUnit(const WorkData& WF) :
  volume(0.0),integralFlux(0.0),
  cellFlux(WF)
  /*!
    Constructor 
  */
{}

  
activeUnit::activeUnit(const activeUnit& A) :
  volume(A.volume),integralFlux(A.integralFlux),
  cellFlux(A.cellFlux)
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
activeUnit::normalize(const double V)
  /*!
    Normalize and integrate the volumes
    \param V :: Volume of object
   */
{
  ELog::RegMethod RegA("activeUnit","normalize");

  integralFlux=0.0;

  const std::vector<DError::doubleErr> YData= cellFlux.getYdata();
  std::vector<double> YOut;
  for(const DError::doubleErr& DV : YData)
    integralFlux+=DV.getVal();

  if (integralFlux>Geometry::zeroTol)
    {
      double prevSum(0.0);
      for(const DError::doubleErr& DV : YData)
	{
	  prevSum+=DV.getVal();
	  YOut.push_back(prevSum/integralFlux);
	}
      volume=V;
      integralFlux/=V;  // need to normalize 
    }
  return;
}
  
void
activeUnit::writePhoton(std::ostream& OX,const Geometry::Vec3D& Pt) const
  /*!
    Calculate the energy based on RNG nubmer and 
    write a photon in a random direction
    \parma OX :: Output stream
    \param R :: Random number between 0.0 and 1.0. 
    \return  energy value
  */
{
  return;
}
  

  
} // NAMESPACE SDef
