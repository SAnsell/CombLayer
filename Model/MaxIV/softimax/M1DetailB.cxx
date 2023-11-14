/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimax/M1DetailB.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "surfRegister.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"

#include "M1Mirror.h"
#include "M1BackPlate.h"
#include "M1FrontShield.h"
#include "M1Connectors.h"
#include "M1Frame.h"
#include "M1DetailB.h"

#include "Importance.h"
#include "Object.h"

namespace xraySystem
{

M1DetailB::M1DetailB(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  mirror(new M1Mirror(keyName+"Mirror")),
  cClamp(new M1BackPlate(keyName+"CClamp")),
  connectors(new M1Connectors(keyName+"Connect")),
  frontShield(new M1FrontShield(keyName+"FPlate")),
  frame(new M1Frame(keyName+"Frame"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(mirror);
  OR.addObject(cClamp);
  OR.addObject(connectors);
  OR.addObject(frontShield);
  OR.addObject(frame);
}

M1DetailB::~M1DetailB()
  /*!
    Destructor
   */
{}

void
M1DetailB::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1DetailB","populate");

  FixedRotate::populate(Control);

  return;
}


void
M1DetailB::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1DetailB","createSurfaces");

  
  return; 
}

void
M1DetailB::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1DetailB","createObjects");

  MonteCarlo::Object* OPtr=System.findObject(1900001);
  mirror->createAll(System,*this,0);

  if (isActive("TubeRadius"))
    cClamp->copyCutSurf("TubeRadius",*this,"TubeRadius");

  cClamp->setCutSurf("FarEnd",*mirror,"back");
  cClamp->setCutSurf("NearEnd",*mirror,"front");
  cClamp->setCutSurf("Back",*mirror,"outSide");
  cClamp->setCutSurf("Mirror",*mirror,"mirrorSide");
  cClamp->setCutSurf("Top",*mirror,"top");
  cClamp->setCutSurf("Base",*mirror,"base");
  cClamp->createAll(System,*mirror,"backPlateOrg");

  
  connectors->setCell("slotA",mirror->getCell("Slot",0));
  connectors->setCell("slotB",mirror->getCell("Slot",1));
  
  connectors->setCell("gapA",cClamp->getCell("PlateGap",1));
  connectors->setCell("gapB",cClamp->getCell("PlateGap",0));
  
  connectors->setCutSurf("slotBase",*mirror,"slotBase");
  connectors->setCutSurf("slotAMid",*mirror,"slotAMid");
  connectors->setCutSurf("slotBMid",*mirror,"slotBMid");
  connectors->setCutSurf("MTop",*mirror,"top");
  connectors->setCutSurf("MBase",*mirror,"base");

  connectors->setCutSurf("CInnerTop",*cClamp,"innerTop");
  connectors->setCutSurf("CInnerBase",*cClamp,"innerBase");
  
  connectors->createAll(System,*mirror,"backPlateOrg");
  
  frontShield->addInsertCell(getInsertCells());
  frontShield->setCutSurf("Front",*cClamp,"front");
  frontShield->setCutSurf("Base",*cClamp,"innerSide");
  frontShield->createAll(System,*cClamp,"front");
  
  frame->setCell("BackCVoid",*cClamp,"CVoid");
  frame->setCell("LowCVoid",*cClamp,"CVoid",1);
  frame->setCell("TopCVoid",*cClamp,"CVoid",2);
  frame->setCell("PlateVoid",*cClamp,"PlateVoid");
  frame->setCell("OuterVoid",*cClamp,"OuterVoid",1);
  frame->setCell("FaceVoid",*cClamp,"FaceVoid");
    
  frame->setSurf("InnerRadius",*cClamp,"CylRadius");
  frame->setSurf("RingRadius",*cClamp,"RingRadius");
  frame->setSurf("FSurf",*cClamp,"FCylInner");
  frame->setSurf("BSurf",*cClamp,"BCylInner");
  frame->setSurf("BeamEdge",*cClamp,"BeamEdge");
  frame->setSurf("FarEdge",*cClamp,"FarEdge");
  
  frame->createAll(System,*mirror,"centreAxis");

  cClamp->adjustExtraVoids(System,
			   frame->getSurf("mirrorVoid"),
			   frame->getSurf("outVoid"),
			   frame->getSurf("baseVoid"));
  cClamp->insertInCell(System,getInsertCells());

  return;
}

void
M1DetailB::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1DetailB","createLinks");

  
  return;
}

void
M1DetailB::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
  */
{
  ELog::RegMethod RegA("M1DetailB","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  return;
}

}  // NAMESPACE xraySystem
