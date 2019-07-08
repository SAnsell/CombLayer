/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BremMonoColl.cxx
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"

#include "BremMonoColl.h"

namespace xraySystem
{

BremMonoColl::BremMonoColl(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedGroup("Main","Flange"),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  inFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}
    
BremMonoColl::~BremMonoColl() 
  /*!
    Destructor
  */
{}

void
BremMonoColl::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("BremMonoColl","populate");
  
  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  gap=Control.EvalVar<double>(keyName+"Gap");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  holeXStep=Control.EvalVar<double>(keyName+"HoleXStep");
  holeZStep=Control.EvalVar<double>(keyName+"HoleZStep");

  holeWidth=Control.EvalVar<double>(keyName+"HoleWidth");
  holeHeight=Control.EvalVar<double>(keyName+"HoleHeight");

  inRadius=Control.EvalVar<double>(keyName+"InRadius");
  if (!inFlag)
    {
      inXStep=Control.EvalVar<double>(keyName+"InXStep");
      inZStep=Control.EvalVar<double>(keyName+"InZStep");
    }

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

 
  return;
}

void
BremMonoColl::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("BremMonoColl","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  if (!inFlag)
    inOrg=Origin+X*inXStep+Z*inZStep;

  return;
}

void
BremMonoColl::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("BremMonoColl","createSurfaces");

  // main tube:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+101,Origin-Y*(flangeLength+gap+length/2.0),Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ExternalCut::makeShiftedSurf
    (SMap,"front",buildIndex+111,1,Y,flangeLength);

  if (!isActive("wallRadius"))
    {
      ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeRadius);
      setCutSurf("wallRadius",-SMap.realSurf(buildIndex+107));
    }

  
  // hole [front]:
  const Geometry::Vec3D holeOrg=Origin+X*holeXStep+Z*holeZStep;

  ModelSupport::buildPlane(SMap,buildIndex+1003,holeOrg-X*(holeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,holeOrg+X*(holeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1005,holeOrg-Z*(holeHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,holeOrg+Z*(holeHeight/2.0),Z);

  // Inlet hole
  ModelSupport::buildCylinder(SMap,buildIndex+2007,inOrg,Y,inRadius);
  return;
}

void
BremMonoColl::createObjects(Simulation& System)
  /*!
    Builds the brem-collimator
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BremMonoColl","createObjects");

  const std::string frontSurf(getRuleStr("front"));
  const std::string wallSurf(getRuleStr("wallRadius"));

  std::string Out;

  // main hole
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 1003 -1004 1005 -1006 ");
  makeCell("Hole",System,cellIndex++,voidMat,0.0,Out);
  // front void volume
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -7 111 ");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,Out);

  // main body:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 -7 (-1003:1004:-1005:1006) ");
  makeCell("Main",System,cellIndex++,innerMat,0.0,Out);

  // main tube:
  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -2 7 -17 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -111 2007 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,Out+frontSurf+wallSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -111 -2007 ");
  makeCell("FlangeHole",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 111 -2 -17");
  addOuterSurf("Main",Out);

  // simple as contained by front/wall radius
  Out=ModelSupport::getComposite(SMap,buildIndex," -111 ");
  addOuterSurf("Flange",Out);

  return;
}

void
BremMonoColl::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("BremMonoColl","createLinks");

  // port centre
  
  ExternalCut::createLink("front",*this,0,Origin,-Y); 
  //  ExternalCut::createLink("back",*this,1,Origin,Y);  
  
  return;
}

void
BremMonoColl::setInOrg(const Geometry::Vec3D& O)
  /*!
    Set the inlet origin
    \param O :: Origin
   */
{
  inFlag=1;
  inOrg=O;
  return;
}
   
void
BremMonoColl::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("BremMonoColl","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  
  insertObjects(System);   


  return;
}
  
}  // NAMESPACE xraySystem
