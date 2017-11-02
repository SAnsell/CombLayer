/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/DXTControl.cxx
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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MapRange.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "localRotate.h"
#include "masterRotate.h"

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
  RadiiInner(A.RadiiInner),RadiiOuter(A.RadiiOuter)
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
      RadiiInner=A.RadiiInner;
      RadiiOuter=A.RadiiOuter;
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
  RadiiInner.clear();
  RadiiOuter.clear();
  return;
}
  
void
DXTControl::setUnit(const Geometry::Vec3D& Pt,
		    const double RI,const double RO,
		    const bool rFlag)
  /*!
    Add a ext component
    \param Pt :: Point for dxtran
    \param R :: inner radius of dxtran sphere
    \param RO :: second outer radius of dxtran sphere
    \param rFlag :: rotation required
  */
{
  ELog::RegMethod RegA("DXTControl","setUnit");

  Centres.push_back(Pt);
  RadiiInner.push_back(RI);
  RadiiOuter.push_back(RO);
  rotateFlag.push_back(rFlag);

  return;
}

void
DXTControl::setDD(const double DK,const double DM)
  /*!
    Add a ext component
    \param DK :: K number
    \param DM :: Modification number
  */
{
  ELog::RegMethod RegA("DXTControl","setUnit");

  DDk.push_back(DK);
  DDm.push_back(DM);

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
  // No work:
  if (Centres.empty()) return;

  const masterRotate& MR=masterRotate::Instance(); 
  std::ostringstream cx;
  cx<<"dxt:"<<particle<<" ";
  for(size_t i=0;i<Centres.size();i++)
    {
      if (rotateFlag[i])
        cx<<MR.forceCalcRotate(Centres[i]);
      else
        cx<<Centres[i];
      cx<<" "<<RadiiOuter[i]<<" "<<RadiiInner[i]<<" ";
    }
  StrFunc::writeMCNPX(cx.str(),OX);

  cx.str("");
  cx<<"dd1 ";
  for(size_t i=0;i<DDk.size();i++)
    cx<<DDk[i]<<" "<<DDm[i]<<" ";
  StrFunc::writeMCNPX(cx.str(),OX);

   
  return;
}


} // NAMESPACE physicsCards
      
   
