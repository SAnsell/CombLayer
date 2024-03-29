/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/Decoupled.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Decoupled.h"
#include "VanePoison.h"

namespace moderatorSystem
{

Decoupled::Decoupled(const std::string& Key)  :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  VP(new VanePoison("decPoison"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(VP);
}
  
Decoupled::Decoupled(const Decoupled& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  VP(new VanePoison("decPoison")),width(A.width),
  height(A.height),westCentre(A.westCentre),eastCentre(A.eastCentre),
  westRadius(A.westRadius),eastRadius(A.eastRadius),westDepth(A.westDepth),
  eastDepth(A.eastDepth),alCurve(A.alCurve),alSides(A.alSides),
  alUpDown(A.alUpDown),modTemp(A.modTemp),modMat(A.modMat),
  alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: Decoupled to copy
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(VP);
}

Decoupled&
Decoupled::operator=(const Decoupled& A)
/*!
  Assignment operator
  \param A :: Decoupled to copy
  \return *this
*/
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);

      *VP = *A.VP;
      width=A.width;
      height=A.height;
      westCentre=A.westCentre;
      eastCentre=A.eastCentre;
      westRadius=A.westRadius;
      eastRadius=A.eastRadius;
      westDepth=A.westDepth;
      eastDepth=A.eastDepth;
      alCurve=A.alCurve;
      alSides=A.alSides;
      alUpDown=A.alUpDown;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

Decoupled::~Decoupled() 
/*!
  Destructor
*/
{}

void
Decoupled::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBasex
  */
{
  ELog::RegMethod RegA("Decoupled","populate");

  FixedRotate::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  eastDepth=Control.EvalVar<double>(keyName+"EastDepth");
  westDepth=Control.EvalVar<double>(keyName+"WestDepth");

  eastRadius=Control.EvalVar<double>(keyName+"EastRadius");
  westRadius=Control.EvalVar<double>(keyName+"WestRadius");

  alCurve=Control.EvalVar<double>(keyName+"AlCurve");
  alSides=Control.EvalVar<double>(keyName+"AlSides");
  alUpDown=Control.EvalVar<double>(keyName+"AlUpDown");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
  

void
Decoupled::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points down the Decoupled direction
    - X Across the Decoupled
    - Z up (towards the target)
    \param FC :: FixedComp for origin/axis
  */
{
  ELog::RegMethod RegA("Decoupled","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
      
  // Decoupled Centre:
  // -- Step in by cut, and  

  westCentre=Origin-Y*(westRadius-westDepth);
  eastCentre=Origin+Y*(eastRadius-eastDepth);
  return;
}

void
Decoupled::createLinks()
  /*!
    Construct FixedObject boundary [outgoing]
    Note : 0-5  : External links
    Note : 6-11 : Internal links 
  */
{
  ELog::RegMethod RegA("Decoupled","createLinks");

  FixedComp::setConnect(0,westCentre+Y*(westRadius+alCurve),Y);
  FixedComp::setConnect(1,eastCentre-Y*(eastRadius+alCurve),-Y);
  FixedComp::setConnect(2,Origin-X*(alSides+width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(alSides+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+alUpDown),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+alUpDown),Z);

  // Set Connect surfaces:
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  // For Cylindrical surface must also have a divider:
  // -- Wish
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+17));
  FixedComp::addLinkSurf(0,SMap.realSurf(buildIndex+1));

  // -- Narrow
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+18));
  FixedComp::addLinkSurf(1,-SMap.realSurf(buildIndex+1));


  // Internal links:
  FixedComp::setConnect(6,westCentre+Y*westRadius,-Y);
  FixedComp::setConnect(7,eastCentre-Y*eastRadius,Y);
  FixedComp::setConnect(8,Origin-X*width/2.0,X);
  FixedComp::setConnect(9,Origin+X*width/2.0,-X);
  FixedComp::setConnect(10,Origin-Z*height/2.0,Z);
  FixedComp::setConnect(11,Origin+Z*height/2.0,-Z);

  // For Cylindrical surface [NO Divider added]
  // -- Wish
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+7));
  // -- Narrow
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+8));
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(11,-SMap.realSurf(buildIndex+6));


  return;
}
  

void
Decoupled::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Decoupled","createSurface");

  // INNER DIVIDE PLANE
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  SMap.setKeep(buildIndex+1,1);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+7,westCentre,Z,westRadius);  // wish
  ModelSupport::buildCylinder(SMap,buildIndex+8,eastCentre,Z,eastRadius);  // narrow

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(alSides+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(alSides+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(alUpDown+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(alUpDown+height/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+17,westCentre+Y*alCurve,Z,westRadius);  // wish
  ModelSupport::buildCylinder(SMap,buildIndex+18,eastCentre-Y*alCurve,Z,eastRadius);  // narrow

  return;
}

void
Decoupled::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Decoupled","createObjects");
  
  HeadRule HR;

  // Methane
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6 -7 -8");
  makeCell("Methane",System,cellIndex++,modMat,modTemp,HR);


  // Inner Al layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 15 -16 -17 -18 "
				 "(-3:4:-5:6:7:8) ");
  System.addCell(cellIndex++,alMat,modTemp,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 15 -16 -17 -18 ");
  addOuterSurf(HR);

  return;
}

int
Decoupled::getDividePlane(const int side) const
  /*!
    Given the side get the divide plane and sense
    \param side :: 0 for Wish / 1 for Narrow
    \return divide surf
  */
{
  return (side) ? SMap.realSurf(buildIndex+1) : 
    -SMap.realSurf(buildIndex+1);
}

int
Decoupled::viewSurf(const int side) const
  /*!
    Given the viewPlane/surface
    \param side :: 0 for Wish / 1 for Narrow
    \return divide surf
  */
{
  return (!side) ? SMap.realSurf(buildIndex+7) : 
    SMap.realSurf(buildIndex+8);
}

Geometry::Vec3D
Decoupled::getDirection(const size_t side) const
  /*!
    Determine the exit direction
    \param side :: Direction number [0-5] (internally checked)
    \return dirction vector
  */
{
  switch (side) 
    {
    case 0:
      return Y;
    case 1:
      return -Y;
    case 2:
      return -X;
    case 3:
      return X;
    case 4:
      return -Z;
    }
  return Z;
}

Geometry::Vec3D
Decoupled::getSurfacePoint(const size_t layerIndex,
                           const long int sideIndex) const
  /*!
    Get the center point for the surfaces in each layer
    \param layerIndex :: Layer number : 0 is inner 4 is outer
    \param sideIndex :: Index to side (front/back/left/right/up/down)
    \return point on surface
  */
{
  ELog::RegMethod RegA("Decoupled","getSurfacePoint");

  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  if (SI>5) 
    throw ColErr::IndexError<long int>(sideIndex,5,"sideIndex");
  if (layerIndex>=2) 
    throw ColErr::IndexError<size_t>(layerIndex,2,"layer");

  
  const double westDist[]={westDepth,alCurve};
  const double eastDist[]={eastDepth,alCurve};
  const double leftDist[]={width/2.0,alSides};
  const double rightDist[]={width/2.0,alSides};
  const double baseDist[]={height/2.0,alUpDown};
  const double topDist[]={height/2.0,alUpDown};

  const double* DPtr[]={westDist,eastDist,leftDist,rightDist,baseDist,topDist};

  // Initialize layer pts:
  Geometry::Vec3D layerPts(Origin);
  const Geometry::Vec3D XYZ=getDirection(SI);

  double sumVec(0.0);
  for(size_t i=0;i<=layerIndex;i++)
    sumVec+=DPtr[SI][i];
  
  return layerPts+XYZ*sumVec;
}
  
void
Decoupled::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp for origin/axis
    \param sideIndex :: Link offset
  */
{
  ELog::RegMethod RegA("Decoupled","createAll");
  Decoupled::populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  VP->addInsertCell(getCell("Methane"));
  VP->setCutSurf("Base",*this,11);
  VP->setCutSurf("Top",*this,12);
  VP->createAll(System,*this,8);

  return;
}
  
}  // NAMESPACE moderatorSystem
