/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/MagnetU1.cxx
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
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "Dipole.h"
#include "Quadrupole.h"
#include "Sexupole.h"
#include "CorrectorMag.h"
#include "MagnetU1.h"

namespace xraySystem
{

MagnetU1::MagnetU1(const std::string& Key) : 
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Main"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  QFm1(new xraySystem::Quadrupole(keyName+"QFm1")),
  SFm(new xraySystem::Sexupole(keyName+"QFm1")),
  QFm2(new xraySystem::Quadrupole(keyName+"QFm2")),
  cMagVA(new xraySystem::CorrectorMag(keyName+"cMagVA")),
  cMagHA(new xraySystem::CorrectorMag(keyName+"cMagJA")),
  SD1(new xraySystem::Sexupole(keyName+"SD1")),
  DIPm(new xraySystem::Dipole(keyName+"DIPm")),
  SD2(new xraySystem::Sexupole(keyName+"SD2"))
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
  
  OR.addObject(QFm1);
  OR.addObject(SFm);
  OR.addObject(QFm2);
  OR.addObject(cMagVA);
  OR.addObject(cMagHA);
  OR.addObject(SD1);
  OR.addObject(DIPm);
  OR.addObject(SD2);
}


MagnetU1::~MagnetU1() 
  /*!
    Destructor
  */
{}

void
MagnetU1::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MagnetU1","populate");

  FixedRotate::populate(Control);


  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
    
  return;
}

void
MagnetU1::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MagnetU1","createSurface");

  // outer surfaces

  // block is off axis - relative to the primary axis (incoming electrons)

  attachSystem::FixedRotateUnit blockFC(0,"blockFC");
  blockFC.setOffset(blockXStep,blockYStep,0);
  blockFC.setRotation(0,0,blockXYAngle);
  blockFC.createUnitVector(Origin,Y,Z);

  const Geometry::Vec3D& bOrg=blockFC.getCentre();
  const Geometry::Vec3D& bX=blockFC.getX();
  const Geometry::Vec3D& bY=blockFC.getY();
  const Geometry::Vec3D& bZ=blockFC.getZ();
    
  ModelSupport::buildPlane(SMap,buildIndex+1,bOrg,bY);
  ModelSupport::buildPlane(SMap,buildIndex+2,bOrg+bY*blockLength,bY);

  ModelSupport::buildPlane(SMap,buildIndex+3,bOrg-bX*(blockWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,buildIndex+4,bOrg+bX*(blockWidth/2.0),bX);

  ModelSupport::buildPlane(SMap,buildIndex+5,bOrg-bZ*(blockHeight/2.0),bZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,bOrg+bZ*(blockHeight/2.0),bZ);

  return;
}

void
MagnetU1::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MagnetU1","createObjects");

  std::string Out;
  // Construct the inner zone a a innerZone
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6  ");

  return;
}

void 
MagnetU1::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("MagnetU1","createLinks");


  return;
}

void
MagnetU1::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("MagnetU1","createAll");

  int outerCell;
  
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);

  // creation of links 
  createLinks();
  return;
}
  
}  // NAMESPACE xraySystem
