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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Line.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "BlankTube.h"
#include "Bellows.h"
#include "TriGroup.h"
#include "CurveMagnet.h"

#include "LObjectSupport.h"
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

  int outerCell,outerCellB,outerCellC;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  if (isActive("front"))
    {
      ELog::EM<<"Front active "<<ELog::endDiag;
      triBend->copyCutSurf("front",*this,"front");
    }

  triBend->createAll(System,*this,0);
  // extra for bending curve and view port
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*triBend,2);
  outerCellB=buildZone->createOuterVoidUnit(System,masterCell,*triBend,3);
  outerCellC=buildZone->createOuterVoidUnit(System,masterCell,*triBend,4);

  cMag->addInsertCell(outerCell);
  cMag->addInsertCell(outerCellB);
  cMag->addInsertCell(outerCellC);
  cMag->createAll(System,*this,0);

  triBend->insertInCell("Main",System,outerCell);
  triBend->insertInCell("FFlange",System,outerCell);
  triBend->insertInCell("TFlange",System,outerCell);
  triBend->insertInCell("Top",System,outerCell);
  triBend->insertInCell("Mid",System,outerCell);

  triBend->insertInCell("MFlange",System,outerCellB);
  triBend->insertInCell("Mid",System,outerCellB);


  triBend->insertInCell("Bend",System,outerCellC);
  triBend->insertInCell("BendStr",System,outerCellC);
  triBend->insertInCell("BendStr",System,outerCellC);
  triBend->insertInCell("BFlange",System,outerCellC);
  triBend->insertAllInCell(System,cMag->getCell("Void"));
  
  // transfer to segment 45 and 46
  CellMap::addCell("LastCell",outerCellB);
  CellMap::addCell("LastCell",outerCellC);

  buildZone->removeLastMaster(System);  
  return;
}

void
Segment44::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*triBend,1);

  setLinkSignedCopy(1,*triBend,2);  // straight exit
  setLinkSignedCopy(2,*triBend,3);  // mid exit
  setLinkSignedCopy(3,*triBend,4);  // bend exit
  setLinkSignedCopy(4,*triBend,5);  // bend exit

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

