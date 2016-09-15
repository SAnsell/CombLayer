/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/MAGIC.cxx
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
#include <array>

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
#include "Aperture.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "CompBInsert.h"
#include "ChopperUnit.h"
#include "ChopperPit.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"
#include "HoleShape.h"

#include "MAGIC.h"

namespace essSystem
{

MAGIC::MAGIC(const std::string& keyName) :
  attachSystem::CopiedComp("magic",keyName),stopPoint(0),
  magicAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  AppA(new constructSystem::Aperture(newName+"AppA"))

 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("MAGIC","MAGIC");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(magicAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeB);
  OR.addObject(AppA);
}

MAGIC::~MAGIC()
  /*!
    Destructor
  */
{}

void
MAGIC::setBeamAxis(const FuncDataBase& Control,
                     const GuideItem& GItem,
                     const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("MAGIC","setBeamAxis");

  magicAxis->populate(Control);
  magicAxis->createUnitVector(GItem);
  magicAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  magicAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  magicAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  magicAxis->setLinkCopy(3,GItem.getKey("Beam"),1);
  
  magicAxis->linkShift(3);
  magicAxis->linkShift(4);
  magicAxis->linkAngleRotate(3);
  magicAxis->linkAngleRotate(4);

  if (reverseZ)
    magicAxis->reverseZ();
  return;
}

  
void 
MAGIC::build(Simulation& System,
               const GuideItem& GItem,
               const Bunker& bunkerObj,
               const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param bunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("MAGIC","build");

  ELog::EM<<"\nBuilding MAGIC on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<" in bunker: "<<bunkerObj.getKeyName()<<ELog::endDiag;
  
  setBeamAxis(Control,GItem,1);
  ELog::EM<<"Beam axis == "<<magicAxis->getSignedLinkPt(3)<<ELog::endDiag;
  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->addFrontCut(GItem.getKey("Beam"),-1);
  FocusA->addEndCut(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*magicAxis,-3,*magicAxis,-3); 
  
  if (stopPoint==1) return;                      // STOP At monolith

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,FocusA->getKey("Guide0"),2);

  AppA->addInsertCell(bunkerObj.getCell("MainVoid"));
  AppA->createAll(System,*VPipeB,2);

  
  return; 
}


}   // NAMESPACE essSystem

