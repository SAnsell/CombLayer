/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/WallLead.cxx
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
#include "FrontBackCut.h"

#include "WallLead.h"

namespace xraySystem
{

WallLead::WallLead(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}
  
WallLead::~WallLead() 
  /*!
    Destructor
  */
{}

void
WallLead::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("WallLead","populate");
  
  FixedOffset::populate(Control);

  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  frontWidth=Control.EvalVar<double>(keyName+"FrontWidth");
  frontHeight=Control.EvalVar<double>(keyName+"FrontHeight");  
  backWidth=Control.EvalVar<double>(keyName+"BackWidth");
  backHeight=Control.EvalVar<double>(keyName+"BackHeight");

  backLength=Control.EvalDefVar<double>(keyName+"BackLength",0.0);
  
  // Void 
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  midMat=ModelSupport::EvalDefMat<int>(Control,keyName+"MidMat".wallMat);

  return;
}

void
WallLead::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("WallLead","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
WallLead::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("WallLead","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive() || !backActive())
    throw ColErr::EmptyContainer("back/front not set for:"+keyName);

  FrontBackCut::getShiftedFront(SMap,buildIndex+11,1,Y,frontLength);

  if (backLength>Geometry::ZeroTol)
    FrontBackCut::getShiftedBack(SMap,buildIndex+12,1,Y,backLength);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(frontWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(frontWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(frontHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(frontHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(backWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(backWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(backHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(backHeight/2.0),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,voidRadius);
  
  return;
}

void
WallLead::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("WallLead","createObjects");

  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 3 -4 5 -6 7 ");
  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,frontSurf+Out);

  if (backLength>Geometry::ZeroTol)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 11 -12 13 -14 15 -16 7 ");
      makeCell("MidWall",System,cellIndex++,midMat,0.0,Out);
      
      Out=ModelSupport::getComposite(SMap,buildIndex," 12 13 -14 15 -16 7 ");
      makeCell("BackWall",System,cellIndex++,wallMat,0.0,backSurf+Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 13 -14 15 -16 7 ");
      makeCell("BackWall",System,cellIndex++,wallMat,0.0,backSurf+Out);
    }
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,frontSurf+backSurf+Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 3 -4 5 -6 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 13 -14 15 -16 ");
  addOuterUnionSurf(Out);

  return;
}

void
WallLead::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("WallLead","createLinks");

  // port centre
  
  FrontBackCut::createFrontLinks(*this,Origin,Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  
  
  return;
}

void
WallLead::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("WallLead","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE xraySystem
