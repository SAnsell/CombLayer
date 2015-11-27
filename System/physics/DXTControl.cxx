/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/DXTControl.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MapRange.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "DXTControl.h"

namespace physicsSystem
{
  
DXTControl::DXTControl() :
  particle("n")
  /*!
    Constructor
  */
{}

DXTControl::DXTControl(const DXTControl& A) :
  particle(A.particle),
  rotateFlag(A.rotateFlag),Centres(A.Centres),
  Radii(A.Radii)
  /*!
    Copy constructor
    \param A :: DXTControl to copy
  */
{}

DXTControl&
DXTControl::operator=(const DXTControl& A)
  /*!
    Assignment operator
    \param A :: DXTControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particle=A.particle;
      rotateFlag=A.rotateFlag;
      Centres=A.Centres;
      Radii=A.Radii;
    }
  return *this;
}

DXTControl::~DXTControl()
  /*!
    Destructor
  */
{}

void
DXTControl::clear()
  /*!
    Clear control
  */
{
  rotateFlag.clear();
  Centres.clear();
  Radii.clear();
  return;
}


void
DXTControl::setUnit(const Geometry::Vec3D& Pt,
		    const double R,
		    const bool rFlag)
  /*!
    Add a ext component
    \param Pt :: Point for dxtran
    \param R :: radius of dxtran sphere
    \param rFlag :: rotation required
  */
{
  ELog::RegMethod RegA("DXTControl","setUnit");

  Centres.push_back(Pt);
  Radii.push_back(R);
  rotateFlag.push_back(rFlag);

  return;
}
    
void
DXTControl::write(std::ostream& OX) const
  /*!
    Write out the card
    \param OX :: Output stream
    \param cellOutOrder :: Cell List
    \param voidCells :: List of void cells
  */
{
  ELog::RegMethod RegA("DXTControl","write");

  std::ostringstream cx;
  cx<<"dxt:"<<particle<<" ";
  for(size_t i=0;i<Centres.size();i++)
    cx<<Centres[i]<<" "<<Radii[i]<<" ";
  
  StrFunc::writeMCNPX(cx.str(),OX);
   
  return;
}


} // NAMESPACE physicsCards
      
   
