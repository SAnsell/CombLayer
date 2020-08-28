/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/MagnetBlock.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
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
#include "ContainedGroup.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "InnerZone.h"
#include "Quadrupole.h"
#include "QuadUnit.h"
#include "DipoleChamber.h"

#include "MagnetBlock.h"

namespace xraySystem
{

MagnetBlock::MagnetBlock(const std::string& Key) : 
  attachSystem::FixedOffset(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  quadUnit(new xraySystem::QuadUnit(keyName+"QuadUnit")),
  dipoleChamber(new xraySystem::DipoleChamber(keyName+"DipoleChamber"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(1,"Flange");
  nameSideIndex(2,"Photon");
  nameSideIndex(3,"Electron");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(quadUnit);
  OR.addObject(dipoleChamber);

}


MagnetBlock::~MagnetBlock() 
  /*!
    Destructor
  */
{}

void
MagnetBlock::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MagnetBlock","populate");

  FixedOffset::populate(Control);

  blockYStep=Control.EvalVar<double>(keyName+"BlockYStep");
  aLength=Control.EvalVar<double>(keyName+"ALength");
  bLength=Control.EvalVar<double>(keyName+"BLength");
  midLength=Control.EvalVar<double>(keyName+"MidLength");
  sectorAngle=Control.EvalVar<double>(keyName+"SectorAngle");
  height=Control.EvalVar<double>(keyName+"Height");
  frontWidth=Control.EvalVar<double>(keyName+"FrontWidth");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
    
  return;
}

void
MagnetBlock::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MagnetBlock","createSurface");

  const Geometry::Quaternion QXYa=
    Geometry::Quaternion::calcQRotDeg(sectorAngle/4.0,-Z);
  const Geometry::Quaternion QXYmid=
    Geometry::Quaternion::calcQRotDeg(sectorAngle/2.0,-Z);

  // Do outer surfaces (vacuum ports)
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*blockYStep,Y);
  ExternalCut::setCutSurf("frontBlock",SMap.realSurf(buildIndex+11));
  
  Geometry::Vec3D POrg(Origin+Y*blockYStep-X*(frontWidth/2.0));
  Geometry::Vec3D QOrg(Origin+Y*blockYStep+X*(frontWidth/2.0));

  int index(buildIndex+3);
  Geometry::Vec3D PX(X);
  Geometry::Vec3D PY(Y);
  const Geometry::Vec3D midX=QXYmid.makeRotate(X);
  const Geometry::Vec3D midY=QXYmid.makeRotate(Y);
  for(const double Len : {aLength,bLength,midLength,bLength,aLength} )
    {
      ModelSupport::buildPlane(SMap,index,POrg,PX);
      POrg+=PY*Len;
      QXYa.rotate(PX);
      QXYa.rotate(PY);
      index+=10;
    }

  ModelSupport::buildPlane(SMap,buildIndex+4,QOrg,midX);
  // Project POrg across 
  ModelSupport::buildPlane(SMap,buildIndex+12,POrg,midY);
  ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+12));
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  FixedComp::setConnect(1,POrg,midY);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  
  return;
}

void
MagnetBlock::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MagnetBlock","createObjects");

  std::string Out;

  const HeadRule& aSegment=quadUnit->getFullRule(2);

  // Construct the outer sectoin [divide later]
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -11 3 -4 5 -6 ");
  makeCell("Front",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 3 13 -4 5 -6 ");
  makeCell("OuterA",System,cellIndex++,outerMat,0.0,
	   Out+aSegment.complement().display());

  Out=ModelSupport::getComposite(SMap,buildIndex," -12 13 23 33 43 -4 5 -6 ");
  makeCell("OuterB",System,cellIndex++,outerMat,0.0,Out+aSegment.display());

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -12 3 13 23 33 43 -4 5 -6 ");
  addOuterSurf(Out);

  return;
}

void
MagnetBlock::buildInner(Simulation& System)
  /*!
    Build the inner part of the magnet block
    \param System :: Simulation to buse
  */
{
  ELog::RegMethod RegA("MagnetBlock","buildInner");
  
  //  quadUnit->setCutSurf("front",undulatorFC,2);
  
  quadUnit->createAll(System,*this,0);


  dipoleChamber->setCutSurf("front",*quadUnit,2);
  dipoleChamber->createAll(System,*quadUnit,2);
 
  return;
}

void
MagnetBlock::insertInner(Simulation& System)
  /*!
    Insert inner components
    \param System :: Simulation to buse
   */
{
  ELog::RegMethod RegA("MagnetBlock","insertInner");
  
  dipoleChamber->insertInCell("Main",System,CellMap::getCell("OuterB"));
  dipoleChamber->insertInCell("Exit",System,CellMap::getCell("OuterB"));

  dipoleChamber->insertInCell("Main",System,this->getInsertCells());
  dipoleChamber->insertInCell("Exit",System,this->getInsertCells());

  for(const int CN : this->getInsertCells())
    ELog::EM<<"Get main cells = "<<CN<<ELog::endDiag;
  
  //  dipoleChamber->insertInCell("Exit",System,CellMap::getCell("OuterB"));

  quadUnit->insertInCell(System,CellMap::getCell("Front"));
  quadUnit->insertInCell(System,CellMap::getCell("OuterA"));
  
  return;
}


void 
MagnetBlock::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("MagnetBlock","createLinks");

  // link 0 / 1 from PreDipole / EPCombine
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  
  return;
}


void
MagnetBlock::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("MagnetBlock","createAll");

  int outerCell;
  
  populate(System.getDataBase());

  buildInner(System);
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertInner(System);
  insertObjects(System);


  
  return;
}
  
}  // NAMESPACE xraySystem
