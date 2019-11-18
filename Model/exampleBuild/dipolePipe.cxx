/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Dipole.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "dipolePipe.h"

namespace exampleSystem
{

dipolePipe::dipolePipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(6,"Centre");
}

dipolePipe::dipolePipe(const std::string& Base,
	       const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Base)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


dipolePipe::~dipolePipe() 
  /*!
    Destructor
  */
{}

void
dipolePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("dipolePipe","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  angle=Control.EvalVar<double>(keyName+"Angle");
 

  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");
  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");

  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}

void
dipolePipe::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("dipolePipe","createSurfaces");

  // pole pieces  

  const Geometry::Vec3D endAxis=
    X*sin(angle*M_PI/180.0)+Y*cos(angle*M_PI/180.0);
  const Geometry::Vec3D midAxis=
    X*sin(angle*M_PI/360.0)+Y*cos(angle*M_PI/360.0);

  
  const Geometry::Vec3D cylCentre=Origin+X*radius;

  // END plane
  const Geometry::Vec3D cylEnd=cylCentre
    -X*(radius*cos(M_PI*angle/180.0))
    +Y*(radius*sin(M_PI*angle/180.0));


  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,cylEnd,endAxis);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(outerHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+100,cylCentre,Y);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,cylCentre+X*(width/2.0),Z,radius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+27,cylCentre-X*(width/2.0),Z,radius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+117,cylCentre+X*(outerWidth/2.0),Z,radius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+127,cylCentre-X*(outerWidth/2.0),Z,radius);

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,cylEnd,endAxis);

  return;
}

void
dipolePipe::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("dipolePipe","createObjects");


  std::string Out;

  // side voids
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 5 -6 17 -27 100 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 15 -16  117 -127 (-5 : 6 : -17:27 )  100 ");
  makeCell("Body",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 15 -16  117 -127 100 ");
  addOuterSurf(Out);
  
  return;
}

void 
dipolePipe::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("dipolePipe","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  
  return;
}

void
dipolePipe::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("dipolePipe","createAll");

  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE exampleSystem
