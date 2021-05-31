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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "insertPlate.h"

#include "QuadUnit.h"
#include "DipoleChamber.h"
#include "DipoleExtract.h"
#include "DipoleSndBend.h"

#include "MagnetBlock.h"

namespace xraySystem
{

MagnetBlock::MagnetBlock(const std::string& Key) : 
  attachSystem::FixedOffset(Key,8),
  attachSystem::ContainedGroup("Magnet","Dipole","Photon"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  quadUnit(new xraySystem::QuadUnit(keyName+"QuadUnit")),
  dipoleChamber(new xraySystem::DipoleChamber(keyName+"DipoleChamber")),
  dipoleExtract(new xraySystem::DipoleExtract(keyName+"DipoleExtract")),
  dipoleSndBend(new xraySystem::DipoleSndBend(keyName+"DipoleSndBend")),
  dipoleOut(new xraySystem::DipoleExtract(keyName+"DipoleOut")),
  eCutDisk(new insertSystem::insertCylinder(keyName+"ECutDisk")),
  eCutMagDisk(new insertSystem::insertPlate(keyName+"ECutMagDisk")),
  eCutWallDisk(new insertSystem::insertPlate(keyName+"ECutWallDisk"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(0,"Flange");
  nameSideIndex(2,"Photon");
  nameSideIndex(3,"Electron");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(quadUnit);
  OR.addObject(dipoleChamber);
  OR.addObject(dipoleExtract);
  OR.addObject(dipoleSndBend);
  OR.addObject(dipoleOut);
  OR.addObject(eCutDisk);
  OR.addObject(eCutMagDisk);
  OR.addObject(eCutWallDisk);
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

  blockXStep=Control.EvalVar<double>(keyName+"BlockXStep");
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
  
  Geometry::Vec3D POrg(Origin+Y*blockYStep+X*(blockXStep-frontWidth/2.0));
  Geometry::Vec3D QOrg(Origin+Y*blockYStep+X*(blockXStep+frontWidth/2.0));

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
  const HeadRule& bSegment=dipoleChamber->getFullRule(4);
  const HeadRule& cSegment=dipoleExtract->getFullRule(2);
  const HeadRule& dSegment=dipoleSndBend->getFullRule(2);
  const HeadRule& exitSeg=dipoleOut->getFullRule(-2);

  // Construct the outer sectoin [divide later]
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -11 3 -4 5 -6 ");
  makeCell("Front",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 3 13 -4 5 -6 ");
  Out+=aSegment.complement().display();
  makeCell("OuterA",System,cellIndex++,outerMat,0.0,Out);

  if (stopPoint=="Quadrupole")
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 1 3 13 -4 5 -6 ");
      Out+=aSegment.complement().display();
      addOuterSurf("Magnet",Out);
      return;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 13 23  -4 5 -6 ");
  makeCell("OuterB",System,cellIndex++,outerMat,0.0,
	   Out+aSegment.display()+bSegment.complement().display());

  Out=ModelSupport::getComposite(SMap,buildIndex," -12 23 33 -4 5 -6 ");
  makeCell("OuterC",System,cellIndex++,outerMat,0.0,
	   Out+bSegment.display()+cSegment.complement().display());

  Out=ModelSupport::getComposite(SMap,buildIndex," -12 33 43 -4 5 -6 ");
  makeCell("OuterD",System,cellIndex++,outerMat,0.0,
	   Out+cSegment.display()+dSegment.complement().display());

  Out=ModelSupport::getComposite(SMap,buildIndex," -12 33 43 -4 5 -6 ");
  makeCell("OuterE",System,cellIndex++,outerMat,0.0,
	   Out+dSegment.display());

  // Construct the outer sectoin [divide later]
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 43 -4 5 -6 ");
  makeCell("Back",System,cellIndex++,0,0.0,Out+exitSeg.display());
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 3 13 23 33 43 -4 5 -6 ");
  addOuterSurf("Magnet",Out+exitSeg.display());

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

  if (stopPoint!="Quadrupole")
    {
      dipoleChamber->setCutSurf("front",*quadUnit,2);
      dipoleChamber->createAll(System,*quadUnit,2);
      addOuterSurf("Dipole",dipoleChamber->getCC("Main"));
      addOuterSurf("Photon",dipoleChamber->getCC("Exit"));

      dipoleExtract->setCutSurf("front",*dipoleChamber,4);
      dipoleExtract->createAll(System,*dipoleChamber,4);

      HeadRule sideCut(SMap.realSurf(buildIndex+33));
      sideCut.addIntersection(SMap.realSurf(buildIndex+43));
      dipoleSndBend->setCutSurf("front",*dipoleExtract,2);
      dipoleSndBend->setCutSurf("side",sideCut);
      dipoleSndBend->createAll(System,*dipoleExtract,2);

      dipoleOut->setCutSurf("front",*dipoleSndBend,2);
      dipoleOut->createAll(System,*dipoleSndBend,2);
    }

  
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

  if (CellMap::hasCell("OuterB"))
    {
      dipoleChamber->insertInCell("Main",System,CellMap::getCell("OuterB"));
      dipoleChamber->insertInCell("Exit",System,CellMap::getCell("OuterC"));
      dipoleExtract->insertInCell(System,CellMap::getCell("OuterC"));
      dipoleSndBend->insertInCell("Beam",System,CellMap::getCell("OuterD"));
      dipoleSndBend->insertInCell("Extra",System,CellMap::getCell("OuterD"));
      dipoleOut->insertInCell(System,CellMap::getCell("OuterE"));
      dipoleOut->insertInCell(System,CellMap::getCell("Back"));
    }

  quadUnit->insertInCell("FlangeA",System,CellMap::getCell("Front"));
  quadUnit->insertInCell("Main",System,CellMap::getCell("Front"));
  quadUnit->insertInCell("Main",System,CellMap::getCell("OuterA"));
  quadUnit->insertInCell("FlangeB",System,CellMap::getCell("OuterA"));
  quadUnit->createQuads(System,CellMap::getCell("OuterA"));
  
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
      
  if (stopPoint=="Quadrupole")
    {
      FixedComp::setLinkCopy(1,*quadUnit,2);
      FixedComp::setLinkCopy(2,*quadUnit,2);
    }
  else
    {
      FixedComp::setLinkCopy(1,*dipoleChamber,2);
      FixedComp::setLinkCopy(2,*dipoleChamber,3);
    }
  return;
}


void
MagnetBlock::buildElectronCut(Simulation& System)
  /*!
    Adds electron cuts to the model (if required)
    \param System :: Simualtion
  */
{
  ELog::RegMethod RegA("MagnetBlock","buildElectronCut");
  /*
  eCutMagDisk->setNoInsert();
  eCutMagDisk->addInsertCell(dipoleChamber->getCell("MagVoid"));
  eCutMagDisk->createAll(System,*dipoleChamber,
			 -dipoleChamber->getSideIndex("dipoleExit"));

  /*  
  eCutWallDisk->setNoInsert();
  eCutWallDisk->addInsertCell(outerCell);
  eCutWallDisk->createAll(System,*dipoleChamber,
			 dipoleChamber->getSideIndex("dipoleExit"));

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(dipoleChamber->getCell("NonMagVoid"));
  eCutDisk->createAll(System,*dipoleChamber,-2);
  */
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
  createUnitVector(FC,sideIndex);

  createSurfaces();
  buildInner(System);


  createObjects(System);
  createLinks();

  insertInner(System);  
  insertObjects(System);
  buildElectronCut(System);


  
  return;
}
  
}  // NAMESPACE xraySystem
