/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/CSPEC.cxx
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
#include "ChopperUnit.h"
#include "DreamHut.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"

#include "CSPEC.h"

namespace essSystem
{

CSPEC::CSPEC(const std::string& keyName) :
  attachSystem::CopiedComp("cspec",keyName),
  stopPoint(0),
  cspecAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  BendC(new beamlineSystem::GuideLine(newName+"BC"))

 /*!
    Constructor
    \param keyName :: keyname of beamline 
 */
{
  ELog::RegMethod RegA("CSPEC","CSPEC");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(cspecAxis);

  OR.addObject(FocusA);

  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(VPipeC);
  OR.addObject(BendC);

}
  
CSPEC::~CSPEC()
  /*!
    Destructor
  */
{}

void
CSPEC::setBeamAxis(const FuncDataBase& Control,
		   const GuideItem& GItem,
                   const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Database of variables
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("CSPEC","setBeamAxis");

  cspecAxis->populate(Control);
  cspecAxis->createUnitVector(GItem,0);
  cspecAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  cspecAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  cspecAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  cspecAxis->setLinkCopy(3,GItem.getKey("Beam"),1);
  
  cspecAxis->linkShift(3);
  cspecAxis->linkShift(4);
  cspecAxis->linkAngleRotate(3);
  cspecAxis->linkAngleRotate(4);

  if (reverseZ)
    cspecAxis->reverseZ();
  return;
}


  
void 
CSPEC::build(Simulation& System,
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
  ELog::RegMethod RegA("CSPEC","build");

  ELog::EM<<"\nBuilding CSPEC on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<ELog::endDiag;
  
  setBeamAxis(Control,GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*cspecAxis,-3,*cspecAxis,-3);

  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge
  
  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,FocusA->getKey("Guide0"),2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,FocusB->getKey("Guide0"),2);

  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*VPipeC,2,*VPipeC,2);


  return;
}


}   // NAMESPACE essSystem

