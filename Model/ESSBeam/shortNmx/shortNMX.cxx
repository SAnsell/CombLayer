/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/shortNmx/shortNMX.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "debugMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "LineShield.h"

#include "shortNMX.h"

namespace essSystem
{

shortNMX::shortNMX(const std::string& keyName) :
  attachSystem::CopiedComp("nmx",keyName),
  stopPoint(0),
  nmxAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),
  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),
  
  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::GuideLine(newName+"FWall")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA"))
  /*!
    Constructor
   \param keyName :: default beamline to construct from
 */
{
  ELog::RegMethod RegA("shortNMX","shortNMX");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  ELog::EM<<"New name == "<<newName<<" "<<keyName<<ELog::endDiag;
  OR.cell(newName+"Axis");
  OR.addObject(nmxAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeB);
  OR.addObject(FocusB);
  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(BInsert);
  OR.addObject(FocusWall);
}



shortNMX::~shortNMX()
  /*!
    Destructor
  */
{}

void
shortNMX::setBeamAxis(const FuncDataBase& Control,
		 const GuideItem& GItem,
		 const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Data base of info on variables
    \param GItem :: Guide Item to 
   */
{
  ELog::RegMethod RegA("shortNMX","setBeamAxis");

  nmxAxis->populate(Control);
  nmxAxis->createUnitVector(GItem);
  nmxAxis->setLinkSignedCopy(0,GItem.getKey("Main"),1);
  nmxAxis->setLinkSignedCopy(1,GItem.getKey("Main"),2);
  nmxAxis->setLinkSignedCopy(2,GItem.getKey("Beam"),1);
  nmxAxis->setLinkSignedCopy(3,GItem.getKey("Beam"),2);

  // BEAM needs to be rotated:
  nmxAxis->linkAngleRotate(3);
  nmxAxis->linkAngleRotate(4);

  if (reverseZ)
    nmxAxis->reverseZ();
  return;
}
  
void 
shortNMX::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("shortNMX","build");
  ELog::EM<<"\nBuilding shortNMX on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  ELog::EM<<"EVAL == "<<newName+"StopPoint"<<ELog::endDiag;
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;

  setBeamAxis(System.getDataBase(),GItem,0);

  ELog::EM<<"Beam axis == "<<nmxAxis->getLinkPt(3)<<ELog::endDiag;
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*nmxAxis,-3,*nmxAxis,-3); 


  if (stopPoint==1) return;                  // STOP at Monolith
  // PIPE out of monolith

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,FocusA->getKey("Guide0"),2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);


  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,FocusB->getKey("Guide0"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);
		    

  if (stopPoint==2) return;                      // STOP At bunker edge
  // IN WALL
  // Make bunker insert
  // IN WALL
  // Make bunker insert
  BInsert->createAll(System,FocusC->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

    // using 7 : mid point 
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7,*BInsert,7);

  return;
}


}   // NAMESPACE essSystem

