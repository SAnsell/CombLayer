/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/ExperimentalHutch.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "ExperimentalHutch.h"

namespace xraySystem
{

ExperimentalHutch::ExperimentalHutch(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  hutIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ExperimentalHutch::~ExperimentalHutch() 
  /*!
    Destructor
  */
{}

void
ExperimentalHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  pbThick=Control.EvalVar<double>(keyName+"PbThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  holeXStep=Control.EvalDefVar<double>(keyName+"HoleXStep",0.0);
  holeZStep=Control.EvalDefVar<double>(keyName+"HoleZStep",0.0);
  holeRadius=Control.EvalDefVar<double>(keyName+"HoleRadius",0.0);


  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");

  return;
}

void
ExperimentalHutch::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
ExperimentalHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,hutIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,hutIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,hutIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,hutIndex+4,Origin+X*ringWidth,X);
  ModelSupport::buildPlane(SMap,hutIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,hutIndex+6,Origin+Z*height,Z);  

  ModelSupport::buildPlane(SMap,hutIndex+15,Origin-Z*(depth+floorThick),Z);

  // Walls
  double extraThick(0.0);
  int HI(hutIndex+10);
  for(const double T : {innerThick,pbThick,outerThick})
    {
      extraThick+=T;
      ModelSupport::buildPlane(SMap,HI+1,
			       Origin-Y*extraThick,Y);
      ModelSupport::buildPlane(SMap,HI+2,
			       Origin+Y*(length+extraThick),Y);
      ModelSupport::buildPlane(SMap,HI+3,
			       Origin-X*(outWidth+extraThick),X);
      ModelSupport::buildPlane(SMap,HI+4,
			       Origin+X*(ringWidth+extraThick),X);
      ModelSupport::buildPlane(SMap,HI+6,
			       Origin+Z*(height+extraThick),Z);  
      HI+=10;
    }

  if (holeRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,hutIndex+107,Origin+X*holeXStep+Z*holeZStep,Y,holeRadius);
  
  return;
}

void
ExperimentalHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createObjects");

  std::string Out;

  Out=ModelSupport::getSetComposite(SMap,hutIndex,"1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  // walls:
  int HI(hutIndex);

  std::list<int> matList({skinMat,pbMat,skinMat});
  for(const std::string& layer : {"Inner","Lead","Outer"})
    {
      const int mat=matList.front();
      matList.pop_front();
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,"1 -2 -3M 13M 5 -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,Out);

      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
					"1 -2  4M -14M 5 -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,Out);
      
      //back wall
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
					"2M -12M 33 -34 5 -6 ");
      makeCell(layer+"BackWall",System,cellIndex++,mat,0.0,Out);

      //front wall
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
					" 11M -1M  33 -34 5 -6 107 ");
      makeCell(layer+"FrontWall",System,cellIndex++,mat,0.0,Out);
      
      // roof
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
					" 31 -32 33 -34 6M -16M ");
      makeCell(layer+"Roof",System,cellIndex++,mat,0.0,Out);
      HI+=10;
    }

  if (holeRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,hutIndex,HI," 1M -1 -107 ");
      makeCell("EnteranceHole",System,cellIndex++,0,0.0,Out);
    }
  
  // floor
  Out=ModelSupport::getSetComposite(SMap,hutIndex,HI,
				    " 1M -2M 3M -4M 15 -5 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);
  
  // Exclude:
  Out=ModelSupport::getComposite(SMap,hutIndex,HI," 1M -2M 3M -4M 15 -6M ");
  addOuterSurf(Out);      

  return;
}

void
ExperimentalHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createLinks");

  const double extraT(innerThick+outerThick+pbThick);
  
  setConnect(0,Origin-Y*extraT,-Y);
  setConnect(1,Origin+Y*(length+extraT),Y);
  
  setLinkSurf(0,-SMap.realSurf(hutIndex+31));
  setLinkSurf(1,SMap.realSurf(hutIndex+32));

  
  return;
}

void
ExperimentalHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
