/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmax/danmaxOpticsHutch.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "PortChicane.h"
#include "OpticsHutch.h"

#include "danmaxOpticsHutch.h"

namespace xraySystem
{

danmaxOpticsHutch::danmaxOpticsHutch(const std::string& Key) : 
  OpticsHutch(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

danmaxOpticsHutch::~danmaxOpticsHutch() 
  /*!
    Destructor
  */
{}

void
danmaxOpticsHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("danmaxOpticsHutch","populate");
  
  OpticsHutch::populate(Control);

  ringStepLength=Control.EvalVar<double>(keyName+"RingStepLength");
  ringStepWidth=Control.EvalVar<double>(keyName+"RingStepWidth");

  return;
}

void
danmaxOpticsHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("danmaxOpticsHutch","createSurfaces");

  OpticsHutch::createSurfaces();

  Geometry::Vec3D BPoint(Origin+Y*ringStepLength);
  ModelSupport::buildPlane(SMap,buildIndex+202,BPoint,Y);
  BPoint+=Y*innerThick;
  ModelSupport::buildPlane(SMap,buildIndex+212,BPoint,Y);
  BPoint+=Y*pbBackThick;
  ModelSupport::buildPlane(SMap,buildIndex+222,BPoint,Y);
  BPoint+=Y*outerThick;
  ModelSupport::buildPlane(SMap,buildIndex+232,BPoint,Y);
      
  Geometry::Vec3D SPoint(Origin+X*ringStepWidth);
  ModelSupport::buildPlane(SMap,buildIndex+204,SPoint,X);
  SPoint+=X*innerThick;
  ModelSupport::buildPlane(SMap,buildIndex+214,SPoint,X);
  SPoint+=X*pbWallThick;
  ModelSupport::buildPlane(SMap,buildIndex+224,SPoint,X);
  SPoint+=X*outerThick;
  ModelSupport::buildPlane(SMap,buildIndex+234,SPoint,X);
  
  return;
}

void
danmaxOpticsHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("danmaxOpticsHutch","createObjects");

  // ring wall
  const HeadRule sideWall=ExternalCut::getValidRule("SideWall",Origin);
  const HeadRule sideCut=ExternalCut::getValidRule("SideWallCut",Origin);
  
  const HeadRule floor=ExternalCut::getValidRule("Floor",Origin);
  const HeadRule frontWall=
    ExternalCut::getValidRule("RingWall",Origin+Y*length);

  HeadRule holeCut;
  int BI(buildIndex);
  for(size_t i=0;i<holeRadius.size();i++)
    {
      holeCut*=ModelSupport::getHeadRule(SMap,BI,"107");
      BI+=100;
    }

  HeadRule HR;
  if (innerOutVoid>Geometry::zeroTol)
    {  
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -1003 -6 ");
      makeCell("WallVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 1003 -6 (-204:-202)");
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -6 (-204:-202)");
    }
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*frontWall*sideCut);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 -36 234 232");
  makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*floor*sideWall);

  // walls:
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -3 13 -6");
  makeCell("InnerWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -13 23 -16");
  makeCell("LeadWall",System,cellIndex++,pbMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 -23 33 -26");
  makeCell("OuterWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);

  
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"2 -12 13 -6 -204");
  makeCell("BackIWall",System,cellIndex++,skinMat,0.0,HR*floor*holeCut);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"12 -22 23 -16 -214");
  makeCell("BackPbWall",System,cellIndex++,pbMat,0.0,HR*floor*holeCut);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"22 -32 33 -26 -224");
  makeCell("BackOuterWall",System,cellIndex++,skinMat,0.0,HR*floor*holeCut);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 13 6 -16 (-204:-202)");
  makeCell("RoofIWall",System,cellIndex++,skinMat,0.0,HR*frontWall*sideCut);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 23 16 -26 (-214:-212)");
  makeCell("RoofPbWall",System,cellIndex++,pbMat,0.0,HR*frontWall*sideCut);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33  26 -36 (-224:-222)");
  makeCell("RoofOuterWall",System,cellIndex++,skinMat,0.0,HR*frontWall*sideCut);

  // ring cutout
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 204 -214 -12 -16");
  makeCell("ringIWall",System,cellIndex++,skinMat,0.0,HR*floor);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"212 214 -224 -22 -26");
  makeCell("ringPbWall",System,cellIndex++,pbMat,0.0,HR*floor);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"222 224 -234 -32 -36");
  makeCell("ringOWall",System,cellIndex++,skinMat,0.0,HR*floor);

  // flat cutout
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 -212 214 -16");
  makeCell("flatIWall",System,cellIndex++,skinMat,0.0,HR*floor*sideWall);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"212 -222 224 -26");
  makeCell("flatPbWall",System,cellIndex++,pbMat,0.0,HR*floor*sideWall);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"222 -232 234 234 -36");
  makeCell("flatOWall",System,cellIndex++,skinMat,0.0,HR*floor*sideWall);


  // Outer void for pipe(s)
  BI=buildIndex;
  for(size_t i=0;i<holeRadius.size();i++)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,BI," 2 -32 -107");
      makeCell("ExitHole",System,cellIndex++,voidMat,0.0,HR);
      BI+=100;
    }

  // EXCLUDE:
  if (outerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 1033 -33 -36");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 1033 -36");
    }
  else
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33 -36");


  addOuterSurf(HR*frontWall*sideCut);

  return;
}

void
danmaxOpticsHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("danmaxOpticsHutch","createLinks");

  
  //  const double extraBack(innerThick+outerThick+pbBackThick);
  //  const double extraWall(innerThick+outerThick+pbWallThick);

  OpticsHutch::createLinks();

  return;
}
  
}  // NAMESPACE xraySystem
