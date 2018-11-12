
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
  PlateUnit(ON,LS),nSegments(0),gapSep(10)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}

SteppedPlateUnit::SteppedPlateUnit(const SteppedPlateUnit& A) :
  PlateUnit(A),nSegments(A.nSegments),gapSep(A.gapSep)
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
  ELog::RegMethod RegA("SteppedPlateUnit","createSurfaces");

  PlateUnit::createSurfaces(SMap,Thick);

  nSegments = stepLength.size();

  // Build side planes for the steps in each layer
  for(size_t layer=0;layer<Thick.size();layer++)
    {
      // Start from 1
      int SN = shapeIndex+layerSep*static_cast<int>(layer)+1;

      for(size_t i=0;i<nCorner;i++)
	{
	  if (layer)
	    ModelSupport::buildShiftedPlane(SMap,SN+gapSep,
					    SMap.realPtr<Geometry::Plane>(SN),
					    -stepThick);
	  SN++;
	}
    }

  // Build front/back planes to separate steps in each layer
  const int SI(shapeIndex+1000);
  const Geometry::Vec3D front(frontPt(0,Thick[0]));
  for (size_t layer=1;layer<Thick.size(); layer++)
    {
      double L(0.0);
      for (size_t segment=0; segment<nSegments;segment++)
	{
	  const double dt(Thick[layer]-Thick[1]);
	  const double step = segment%2 ? dt : -dt;
	  L += stepLength[segment];

	  ModelSupport::buildPlane(SMap,SI+static_cast<int>(layer)*layerSep+
				   static_cast<int>(segment),
				   front+YVec*(L+step),YVec);
	}
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
  // both for segments without (cx) and with (cxStep) step
  int SN = layerSep*static_cast<int>(layerN)+1;
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
      cxStep<<SN+gapSep;
      SN++;
    }
  if (bFlag) cx<<")";

  std::string Out=cx.str();
  if ((layerN) && (nSegments))
    {
      std::string OutEven=cx.str() + " (";
      std::string OutOdd=cxStep.str() + " (";
      const int SG(1000);
      for (size_t segment=0; segment<=nSegments; segment++)
	{
	  // indices of back/front planes
	  const int s1(SG+static_cast<int>(layerN)*layerSep+
		       static_cast<int>(segment));
	  const int s2(s1+1);

	  if (segment==0)
	    Out = std::to_string(-s1) + " : ";
	  else if (segment==nSegments)
	    Out = std::to_string(s2-2) + " ";
	  else
	    {
	      Out = std::to_string(s1-1) + " " + std::to_string(-s1);
	      if (segment!=nSegments-1)
		Out += " : ";
	      else
		Out += " ";
	    }

	  if (segment%2)
	    OutOdd += Out;
	  else
	    OutEven += Out;
	}
      OutOdd += " )";
      OutEven += " )";
      Out = OutOdd + " : " + OutEven;
    }

  return ModelSupport::getComposite(SMap,shapeIndex," ( " + Out + " ) ");
}
}  // NAMESPACE beamlineSystem
