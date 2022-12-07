/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essConstruct/TwinChopper.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedRotate.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "InnerPort.h"
#include "boltRing.h"
#include "TwinBase.h"
#include "TwinChopper.h"

namespace essConstruct
{

TwinChopper::TwinChopper(const std::string& Key) :
  TwinBase(Key),
  frontFlange(new essConstruct::boltRing(Key,"FrontFlange")),
  backFlange(new essConstruct::boltRing(Key,"BackFlange")),
  IPA(new essConstruct::InnerPort(Key+"IPortA")),
  IPB(new essConstruct::InnerPort(Key+"IPortB"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(frontFlange);
  OR.addObject(backFlange);
  OR.addObject(IPA);
  OR.addObject(IPB);
}

TwinChopper::~TwinChopper() 
  /*!
    Destructor
  */
{}


void
TwinChopper::buildPorts(Simulation& System)
  /*!
    Build ports
    \param System :: simulation
  */
{
    // Front ring seal
  ELog::RegMethod RegA("TwinChopper","buildPort");

  HeadRule HR;
  
  const attachSystem::FixedComp& Main=getKey("Main");
  const attachSystem::FixedComp& Beam=getKey("Beam");

  frontFlange->setInnerExclude();
  frontFlange->addInsertCell(getCell("FrontCase"));
  frontFlange->setFront(SMap.realSurf(buildIndex+1));
  frontFlange->setBack(-SMap.realSurf(buildIndex+11));
  frontFlange->createAll(System,Main,0);

  // Back ring seal
  backFlange->setInnerExclude();
  backFlange->addInsertCell(getCell("BackCase"));
  backFlange->setFront(SMap.realSurf(buildIndex+12));
  backFlange->setBack(-SMap.realSurf(buildIndex+2));
  backFlange->createAll(System,Main,0);

  // Ports in front/back seal void
  // -----------------------------
  const HeadRule innerFSurf=frontFlange->getSurfRule("#innerRing");
  const HeadRule innerBSurf=backFlange->getSurfRule("#innerRing");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11");
  makeCell("PortVoid",System,cellIndex++,0,0.0,HR*innerFSurf);

  IPA->addInnerCell(getCell("PortVoid",0));
  IPA->setCutSurf("Boundary",HR*innerFSurf);
  IPA->createAll(System,Beam,0);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -2");
  makeCell("PortVoid",System,cellIndex++,0,0.0,HR*innerBSurf);

  IPB->addInnerCell(getCell("PortVoid",1));
  IPB->setCutSurf("Boundary",HR*innerBSurf);
  IPB->createAll(System,Beam,0);
  return;
}

    
void
TwinChopper::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("TwinChopper","createAll(FC)");

  TwinBase::populate(System.getDataBase());
  TwinBase::createUnitVector(beamFC,FIndex);
  TwinBase::createSurfaces();    
  TwinBase::createObjects(System);
  buildPorts(System);
  
  TwinBase::createLinks();

  TwinBase::processInsert(System);
  TwinBase::buildMotors(System);

  return;
}
  
}  // NAMESPACE constructSystem
