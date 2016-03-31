/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperUnit.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "ChopperUnit.h"

namespace constructSystem
{

ChopperUnit::ChopperUnit(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  houseIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(houseIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}



ChopperUnit::~ChopperUnit() 
  /*!
    Destructor
  */
{}

void
ChopperUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ChopperUnit","populate");

  FixedOffsetGroup::populate(Control);
  //  + Fe special:
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  shortWidth=Control.EvalVar<double>(keyName+"ShortWidth");
  const double diff(width-shortWidth);
  shortHeight=Control.EvalDefVar<double>(keyName+"ShortHeight",height-diff);

  mainRadius=Control.EvalVar<double>(keyName+"MainRadius");
  mainThick=Control.EvalVar<double>(keyName+"MainThick");

  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  motorRadius=Control.EvalVar<double>(keyName+"MotorRadius");
  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
ChopperUnit::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ChopperUnit","createUnitVector");

  attachSystem::FixedComp& Main=getKey("Main");
  attachSystem::FixedComp& Beam=getKey("Beam");

  Beam.createUnitVector(FC,sideIndex);
  Main.createUnitVector(FC,sideIndex);
  applyOffset();
  //  Main.applyShift(0.0,0,0,beamZStep);
  setDefault("Main");
  
  ELog::EM<<"Origin[M] == "<<Main.getCentre()<<ELog::endDiag;
  ELog::EM<<"Origin == "<<Beam.getCentre()<<ELog::endDiag;
  return;
}


void
ChopperUnit::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ChopperUnit","createSurfaces");

  const Geometry::Vec3D XLong=X*(width/2.0);
  const Geometry::Vec3D ZLong=Z*(height/2.0);
  const Geometry::Vec3D XShort=X*(shortWidth/2.0);
  const Geometry::Vec3D ZShort=Z*(shortHeight/2.0);

  ModelSupport::buildPlane(SMap,houseIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+3,Origin-XLong,X);
  ModelSupport::buildPlane(SMap,houseIndex+4,Origin+XLong,X);
  ModelSupport::buildPlane(SMap,houseIndex+5,Origin-ZLong,Z);
  ModelSupport::buildPlane(SMap,houseIndex+6,Origin+ZLong,Z);
      // triangular corners

      // All triangle edges point to centre:
  ModelSupport::buildPlane(SMap,houseIndex+7,
                           Origin-XLong-ZShort,
                           Origin-XShort-ZLong,
                           Origin-XShort-ZLong+Y,
                           Z);
  ModelSupport::buildPlane(SMap,houseIndex+8,
                           Origin-XLong+ZShort,
                           Origin-XShort+ZLong,
                           Origin-XShort+ZLong+Y,
                           -Z);
  ModelSupport::buildPlane(SMap,houseIndex+9,
                           Origin+XLong+ZShort,
                           Origin+XShort+ZLong,
                           Origin+XShort+ZLong+Y,
                           -Z);
  ModelSupport::buildPlane(SMap,houseIndex+10,
                           Origin+XLong-ZShort,
                           Origin+XShort-ZLong,
                           Origin+XShort-ZLong+Y,
                           Z);


  // CREATE INNER VOID:
  ModelSupport::buildPlane(SMap,houseIndex+11,Origin-Y*(mainThick/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+12,Origin+Y*(mainThick/2.0),Y);
  ModelSupport::buildCylinder(SMap,houseIndex+17,Origin,Y,mainRadius);

  // Construct beamport:
  setDefault("Beam");

  ModelSupport::buildCylinder(SMap,houseIndex+27,Origin,Y,portRadius);
  
  return;
}

void
ChopperUnit::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    */
{
  ELog::RegMethod RegA("ChopperUnit","createObjects");

  std::string Out;

  // Void 
  Out=ModelSupport::getComposite(SMap,houseIndex,
                                 "1 -2 3 -4 5 -6 7 8 9 10 (-11:12:17)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Wall",cellIndex-1);

  // Wall
  Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 -17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -2 3 -4 5 -6 7 8 9 10");
  addOuterSurf(Out);


  
  
  return;
}

void
ChopperUnit::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ChopperUnit","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.setConnect(0,Origin-Y*(length/2.0),-Y);
  mainFC.setConnect(1,Origin+Y*(length/2.0),Y);
  mainFC.setConnect(2,Origin-X*(width/2.0),-X);
  mainFC.setConnect(3,Origin+X*(width/2.0),X);
  mainFC.setConnect(4,Origin-Y*(height/2.0),-Z);
  mainFC.setConnect(5,Origin-Y*(height/2.0),Z);

  mainFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  mainFC.setLinkSurf(2,-SMap.realSurf(houseIndex+3));
  mainFC.setLinkSurf(3,SMap.realSurf(houseIndex+4));
  mainFC.setLinkSurf(4,-SMap.realSurf(houseIndex+5));
  mainFC.setLinkSurf(5,SMap.realSurf(houseIndex+6));
  
  return;
}

void
ChopperUnit::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("ChopperUnit","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(beamFC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
