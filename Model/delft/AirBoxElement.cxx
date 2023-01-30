/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/AirBoxElement.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <climits>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <boost/multi_array.hpp>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "RElement.h"
#include "AirBoxElement.h"

namespace delftSystem
{


AirBoxElement::AirBoxElement(const size_t XI,const size_t YI,
			 const std::string& Key) :
  RElement(XI,YI,Key),waterMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: X coordinate
    \param YI :: Y coordinate
    \param Key :: BaseName 
  */
{}

AirBoxElement::AirBoxElement(const size_t XI,const size_t YI,
			     const std::string& Key,
			     const int waterM) :
  RElement(XI,YI,Key),waterMat(waterM)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: X coordinate
    \param YI :: Y coordinate
    \param Key :: BaseName 
  */
{}

AirBoxElement::AirBoxElement(const AirBoxElement& A) : 
  RElement(A),
  Width(A.Width),Depth(A.Depth),Height(A.Height),
  wallThick(A.wallThick),edgeGap(A.edgeGap),innerMat(A.innerMat),
  wallMat(A.wallMat),waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: AirBoxElement to copy
  */
{}

AirBoxElement&
AirBoxElement::operator=(const AirBoxElement& A)
  /*!
    Assignment operator
    \param A :: AirBoxElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RElement::operator=(A);
      Width=A.Width;
      Depth=A.Depth;
      Height=A.Height;
      wallThick=A.wallThick;
      edgeGap=A.edgeGap;
      innerMat=A.innerMat;
      wallMat=A.wallMat;
      waterMat=A.waterMat;
    }
  return *this;
}
    
void
AirBoxElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: Database for variables
  */
{
  ELog::RegMethod RegA("AirBoxElement","populate");

  Width=ReactorGrid::getElement<double>
    (Control,keyName+"Width",XIndex,YIndex);
  Depth=ReactorGrid::getElement<double>
    (Control,keyName+"Depth",XIndex,YIndex);
  Height=ReactorGrid::getElement<double>
    (Control,keyName+"Height",XIndex,YIndex);

  edgeGap=ReactorGrid::getElement<double>
    (Control,keyName+"EdgeGap",XIndex,YIndex);
  wallThick=ReactorGrid::getElement<double>
    (Control,keyName+"WallThick",XIndex,YIndex);

  innerMat=ReactorGrid::getMatElement
    (Control,keyName+"InnerMat",XIndex,YIndex);
  wallMat=ReactorGrid::getMatElement
    (Control,keyName+"WallMat",XIndex,YIndex);
  
  return;
}

void
AirBoxElement::createSurfaces()
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
  */
{  
  ELog::RegMethod RegA("AirBoxElement","createSurfaces");


  // Planes [OUTER]:
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*Depth/2.0,Y); 
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*Height,Z);


  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(Depth/2.0-edgeGap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(Depth/2.0-edgeGap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(Width/2.0-edgeGap),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(Width/2.0-edgeGap),X);

  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*(Depth/2.0-edgeGap-wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(Depth/2.0-edgeGap-wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(Width/2.0-edgeGap-wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(Width/2.0-edgeGap-wallThick),X);

  makeShiftedSurf(SMap,"BasePlate",buildIndex+25,Z,wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(Height-wallThick),Z);


  ModelSupport::buildPlane(SMap,buildIndex+6,Z*Height,Z);


  return;
}

void
AirBoxElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("AirBoxElement","createObjects");

  const HeadRule& baseHR=getRule("BasePlate");
  HeadRule HR;

  // Outer Layers
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 3 -4 -6 ");
  addOuterSurf(HR*baseHR);      
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " 1 -2 3 -4 -6 (-11:12:-13:14)");
  System.addCell(cellIndex++,waterMat,0.0,HR*baseHR);

  // walls
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "11 -12 13 -14 -6 (-21:22:-23:24:-25:26)");
  System.addCell(cellIndex++,wallMat,0.0,HR*baseHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 23 -24 25 -26");
  System.addCell(cellIndex++,innerMat,0.0,HR);

  return;
}


void
AirBoxElement::createLinks()
  /*!
    Creates lines [currently does nothing]
  */
{

  return;
}

void
AirBoxElement::createAll(Simulation& System,
			 const attachSystem::FixedComp& RG,
			 const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param RG :: Fixed Unit
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("AirBoxElement","createAll");
  populate(System.getDataBase());

  createUnitVector(RG,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
