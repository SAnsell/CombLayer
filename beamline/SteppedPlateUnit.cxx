
/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/SteppedPlateUnit.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell / Konstantin Batkov
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
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "surfRegister.h"
#include "Surface.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ShapeUnit.h"
#include "PlateUnit.h"
#include "SteppedPlateUnit.h"

#include "Rules.h"
#include "Quadratic.h"
#include "Plane.h"
#include "HeadRule.h"

namespace beamlineSystem
{

SteppedPlateUnit::SteppedPlateUnit(const int ON,const int LS)  :
  PlateUnit(ON,LS),nSegments(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}

SteppedPlateUnit::SteppedPlateUnit(const SteppedPlateUnit& A) :
  PlateUnit(A),nSegments(A.nSegments)
  /*!
    Copy constructor
    \param A :: SteppedPlateUnit to copy
  */
{}

SteppedPlateUnit&
SteppedPlateUnit::operator=(const SteppedPlateUnit& A)
  /*!
    Assignment operator
    \param A :: SteppedPlateUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      PlateUnit::operator=(A);
      nSegments=A.nSegments;
    }
  return *this;
}

SteppedPlateUnit*
SteppedPlateUnit::clone() const 
  /*!
    Clone funciton
    \return *this
   */
{
  return new SteppedPlateUnit(*this);
}

void
SteppedPlateUnit::clear()
  /*!
    Clear all the points
   */
{
  PlateUnit::clear();
  nSegments=0.0;
  return;
}


void
SteppedPlateUnit::createSurfaces(ModelSupport::surfRegister& SMap,
			  const std::vector<double>& Thick,
			  const double& stepThick,
			  const std::vector<double>& stepLength)
  /*!
    Build the surfaces for the track
    \param SMap :: SMap to use
    \param Thick :: Thickness for each layer
    \param stepThick :: step thickness
    \param stepLength :: step length
   */
{
  ELog::RegMethod RegA("SteppedPlateUnit","createSurfaces [with stes]");

  PlateUnit::createSurfaces(SMap,Thick);

  nSegments = stepLength.size();

  for(size_t j=0;j<Thick.size();j++)
    {
      // Start from 1
      int SN(shapeIndex+layerSep*static_cast<int>(j)+1);

      for(size_t i=0;i<nCorner;i++)
	{
	  if (j)
	    ModelSupport::buildShiftedPlane(SMap,SN+10,
					    SMap.realPtr<Geometry::Plane>(SN),
					    -stepThick);
	  SN++;
	}
    }

  double L(stepLength[0]);
  const int SI(shapeIndex+1000);
  const Geometry::Vec3D front(frontPt(0,Thick[0]));

  for (size_t i=1; i<=nSegments;i++)
    {
      ModelSupport::buildPlane(SMap,SI+static_cast<int>(i),
			       front+YVec*(L),YVec);
      const double step = i%2 ? -stepThick : stepThick;
      ModelSupport::buildPlane(SMap,SI+static_cast<int>(i+100),
			       front+YVec*(L+step),YVec);
      L += stepLength[i];
    }

  return;
}

std::string
SteppedPlateUnit::getString(const ModelSupport::surfRegister& SMap,
		      const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("SteppedPlateUnit","getString");

  if (!nCorner) return "";

  std::ostringstream cx,cxStep;
  bool bFlag(0);
  // Start from 1
  // define rules for side surfaces (without front/back)
  // both for segmentts without (cx) and with (cxStep) step
  int SN(layerSep*static_cast<int>(layerN)+1);
  for(size_t i=0;i<nCorner;i++)
    {
      if (nonConvex[i] || nonConvex[(i+1) % nCorner])
	{
	  cx<< ((!bFlag) ? " (" : ":");
	  bFlag=1;
	}
      else
	{
	  cx<< ((bFlag) ? ") " : " ");
	  cxStep << " "; // currently cxStep does not work if bFlag is true
	  bFlag=0;
	}
      cx<<SN;
      cxStep<<SN+10;
      SN++;
    }
  if (bFlag) cx<<")";


  std::string Out=cx.str();
  if ((layerN) && (nSegments))
    {
      int SG=1000;
      for (size_t j=0; j<=nSegments; j++)
	{
	  int s1=SG+static_cast<int>(j);
	  int s2=s1+1;
	  if (j==0)
	    Out += " " + std::to_string(-s2) + " : ";
	  else if (j==nSegments)
	      Out += " " + std::to_string(s1) + " " + cx.str();
	  else
	      Out += " " + std::to_string(s1) + " " +
		(j%2 ? cxStep.str() : cx.str()) + " " +
		std::to_string(-s2) + " : ";
	}
    }

  return ModelSupport::getComposite(SMap,shapeIndex," ( " + Out + " ) ");
}
}  // NAMESPACE beamlineSystem
