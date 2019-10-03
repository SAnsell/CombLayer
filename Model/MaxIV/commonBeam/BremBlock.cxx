/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BremBlock.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "BremBlock.h"

namespace xraySystem
{

BremBlock::BremBlock(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}
  
BremBlock::~BremBlock() 
  /*!
    Destructor
  */
{}

void
BremBlock::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("BremBlock","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  width=Control.EvalDefVar<double>(keyName+"Width",-1.0);
  height=Control.EvalDefVar<double>(keyName+"Height",-1.0);

  length=Control.EvalVar<double>(keyName+"Length");  

  holeXStep=Control.EvalDefVar<double>(keyName+"HoleXStep",0.0);
  holeZStep=Control.EvalDefVar<double>(keyName+"HoleZStep",0.0);
  holeAHeight=Control.EvalHead<double>(keyName,"HoleAHeight","HoleHeight");
  holeAWidth=Control.EvalHead<double>(keyName,"HoleAWidth","HoleWidth");
  holeBHeight=Control.EvalDefVar<double>(keyName+"HoleBHeight",holeAHeight);
  holeBWidth=Control.EvalDefVar<double>(keyName+"HoleBWidth",holeAWidth);
  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  if (radius<Geometry::zeroTol &&
      (width<Geometry::zeroTol || height<Geometry::zeroTol))
    throw ColErr::SizeError<double>(radius,0.0,"Radius and W/H beloww zero");

  return;
}

void
BremBlock::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("BremBlock","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
BremBlock::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("BremBlock","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }

  // hole [front]:
  const Geometry::Vec3D holeFront=Origin+X*holeXStep+Z*holeZStep;
  const Geometry::Vec3D holeBack=
    Origin+X*holeXStep+Z*holeZStep+Y*length;

  ModelSupport::buildPlane(SMap,buildIndex+1003,
			   holeFront-X*(holeAWidth/2.0),
			   holeBack-X*(holeBWidth/2.0),
			   holeBack-X*(holeBWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,
			   holeFront+X*(holeAWidth/2.0),
			   holeBack+X*(holeBWidth/2.0),
			   holeBack+X*(holeBWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+1005,
			   holeFront-Z*(holeAHeight/2.0),
			   holeBack-Z*(holeBHeight/2.0),
			   holeBack-Z*(holeBHeight/2.0)+X,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,
			   holeFront+Z*(holeAHeight/2.0),
			   holeBack+Z*(holeBHeight/2.0),
			   holeBack+Z*(holeBHeight/2.0)+X,
			   Z);
  


  if (radius>Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
    }
  return;
}

void
BremBlock::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BremBlock","createObjects");

  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 1005 -1006 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,frontSurf+backSurf+Out);
  
  Out=ModelSupport::getSetComposite
    (SMap,buildIndex," 3 -4 5 -6 -7 (-1003: 1004 : -1005: 1006)");
  makeCell("Shield",System,cellIndex++,mainMat,0.0,Out+frontSurf+backSurf);

  // If front back set then don't add to exclude --
  // thus buildIndex+1 or buildIndex+2 will not exist.
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 -7 3 -4 5 -6 ");
  addOuterSurf(Out);

  return;
}

void
BremBlock::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("BremBlock","createLinks");

  // port centre
  
  FrontBackCut::createFrontLinks(*this,Origin,Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  
  
  return;
}

void
BremBlock::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("BremBlock","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE xraySystem
