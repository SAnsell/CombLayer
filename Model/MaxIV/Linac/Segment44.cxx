/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/Segment44.cxx
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"

#include "TriGroup.h"
#include "CurveMagnet.h"

#include "TDCsegment.h"
#include "Segment44.h"

namespace tdcSystem
{

// Note currently uncopied:


Segment44::Segment44(const std::string& Key) :
  TDCsegment(Key,6),
  triBend(new tdcSystem::TriGroup(keyName+"TriBend")),
  cMag(new tdcSystem::CurveMagnet(keyName+"CMag"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(triBend);
  OR.addObject(cMag);

  setFirstItems(triBend);
}

Segment44::~Segment44()
  /*!
    Destructor
   */
{}

void
Segment44::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment44","buildObjects");

  int outerCellA,outerCellB,outerCellC,outerCellD;

  if (isActive("front"))
    {
      ELog::EM<<"Front active "<<ELog::endDiag;
      triBend->copyCutSurf("front",*this,"front");
    }

  triBend->createAll(System,*this,0);
  // extra for bending curve and view port
  outerCellA=buildZone->createUnit(System,*triBend,"EndWall");
  outerCellB=buildZone->createUnit(System,*triBend,2);
  outerCellC=buildZone->createUnit(System,*triBend,3);
  outerCellD=buildZone->createUnit(System,*triBend,4);

  cMag->createAll(System,*this,0);
  cMag->insertInCell("Front", System, outerCellA);
  cMag->insertInCell("Mid", System, outerCellB);
  cMag->insertInCell("Mid", System, outerCellC);
  cMag->insertInCell("Back", System, outerCellD);

  triBend->insertInCell("Main",System,outerCellA);
  triBend->insertInCell("FFlange",System,outerCellA);

  triBend->insertInCell("TFlange",System,outerCellB);
  triBend->insertInCell("Top",System,outerCellB);
  triBend->insertInCell("Mid",System,outerCellB);

  triBend->insertInCell("MFlange",System,outerCellC);
  triBend->insertInCell("Mid",System,outerCellC);

  triBend->insertInCell("BendStr",System,outerCellD);
  triBend->insertInCell("BFlange",System,outerCellD);

  //  triBend->insertInCell("Main",System,cMag->getCell("VoidFront"));
  triBend->insertInCell("Main",System,cMag->getCell("Void"));
  triBend->insertInCell("Mid",System,cMag->getCell("Void"));
  triBend->insertInCell("Bend",System,cMag->getCell("Void"));
  triBend->insertInCell("BendStr",System,cMag->getCell("Void"));
  triBend->insertInCell("BendStr",System,cMag->getCell("VoidBack"));

  // transfer to segment 45 and 46
  CellMap::addCell("LastCell",outerCellC);
  CellMap::addCell("LastCell",outerCellD);

  CellMap::addCell("OverLap",outerCellD);

  return;
}

void
Segment44::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkCopy(0,*triBend,1);

  setLinkCopy(1,*triBend,2);  // straight exit
  setLinkCopy(2,*triBend,3);  // mid exit
  setLinkCopy(3,*triBend,4);  // bend exit
  setLinkCopy(4,*triBend,5);  // bend exit

  FixedComp::nameSideIndex(1,"straightExit");
  FixedComp::nameSideIndex(2,"midExit");
  FixedComp::nameSideIndex(3,"magnetExit");
  FixedComp::nameSideIndex(4,"splitPoint");

  joinItems.push_back(FixedComp::getFullRule(2));
  joinItems.push_back(FixedComp::getFullRule(3));
  joinItems.push_back(FixedComp::getFullRule(4));

  return;
}

void
Segment44::writePoints() const
  /*!
    Writes out points to allow tracking through magnets
  */
{
  ELog::RegMethod RegA("Segment44","writePoints");

  const std::vector<std::shared_ptr<attachSystem::FixedComp>> Items
    (
     {triBend}
     );
  TDCsegment::writeBasicItems(Items);

  return;
}

void
Segment44::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("Segment44","createAll");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem
