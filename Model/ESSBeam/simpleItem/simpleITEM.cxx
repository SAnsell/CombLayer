/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/simpleItem/SimpleITEM.cxx
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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "insertObject.h"
#include "insertPlate.h"

#include "simpleITEM.h"

namespace essSystem
{

simpleITEM::simpleITEM(const std::string& keyN) :
  attachSystem::CopiedComp("simple",keyN),stopPoint(0),
  simpleAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  Plate(new insertSystem::insertPlate(newName+"Plate"))
  /*!
    Constructor
    \param keyN :: keyName
  */
{
  ELog::RegMethod RegA("simpleITEM","simpleITEM");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  OR.cell(newName+"Axis");
  OR.addObject(simpleAxis);
  OR.addObject(Plate);
}



simpleITEM::~simpleITEM()
  /*!
    Destructor
  */
{}

void
simpleITEM::setBeamAxis(const FuncDataBase& Control,
		  const GuideItem& GItem,
		  const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Data base of info on variables
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse the z-direction 
   */
{
  ELog::RegMethod RegA("simpleITEM","setBeamAxis");

  simpleAxis->populate(Control);
  simpleAxis->createUnitVector(GItem);
  simpleAxis->setLinkSignedCopy(0,GItem.getKey("Main"),1);
  simpleAxis->setLinkSignedCopy(1,GItem.getKey("Main"),2);

  simpleAxis->setLinkSignedCopy(2,GItem.getKey("Beam"),1);
  simpleAxis->setLinkSignedCopy(3,GItem.getKey("Beam"),2);
  // BEAM needs to be rotated:
  simpleAxis->linkAngleRotate(3);
  simpleAxis->linkAngleRotate(4);
  
  if (reverseZ)
    simpleAxis->reverseZ();
  return;
}
  
void 
simpleITEM::build(Simulation& System,
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
  ELog::RegMethod RegA("simpleITEM","build");
  ELog::EM<<"\nBuilding simpleITEM on : "<<GItem.getKeyName()<<ELog::endDiag;

  FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(Control);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);

  setBeamAxis(System.getDataBase(),GItem,0);
  

  if (stopPoint==1) return;                // STOP At monolith edge

  ELog::EM<<"Bunker unit = "<<bunkerObj.getKeyName()<<ELog::endDiag;
  Plate->addInsertCell(bunkerObj.getCell("MainVoid"));
  Plate->createAll(System,GItem.getKey("Beam"),2);

  
  return;
}


}   // NAMESPACE essSystem

