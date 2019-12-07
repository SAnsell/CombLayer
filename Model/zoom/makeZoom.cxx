/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/makeZoom.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <array>

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
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "World.h"
#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "ZoomShutter.h"
#include "BulkInsert.h"
#include "BulkShield.h"
#include "bendSection.h"
#include "ZoomBend.h"
#include "ZoomChopper.h"
#include "DiskChopper.h"
#include "ZoomStack.h"
#include "ZoomOpenStack.h"
#include "ZoomCollimator.h"
#include "ZoomTank.h"
#include "ZoomHutch.h"
#include "ZoomPrimary.h"
#include "makeZoom.h"

namespace zoomSystem
{

makeZoom::makeZoom() :
  ZBend(new ZoomBend("zoomBend")),
  ZChopper(new ZoomChopper("zoomChopper")),
  ZDisk(new constructSystem::DiskChopper("zoomDisk")),
  ZCollimator(new ZoomCollimator("zoomCollimator")),
  ZColInsert(new ZoomOpenStack("zoomCollInsert")),
  ZPrim(new ZoomPrimary("zoomPrimary")),
  ZHut(new ZoomHutch("zoomHutch"))
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeZoom","Constructor");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject("zoomBend",ZBend);
  OR.addObject("zoomChopper",ZChopper);
  OR.addObject(ZDisk);
  OR.addObject("zoomCollimator",ZCollimator);
  OR.addObject(ZColInsert);
  OR.addObject(ZPrim);
  OR.addObject("zoomHutch",ZBend);
}

makeZoom::makeZoom(const makeZoom& A) : 
  ZBend(new ZoomBend(*A.ZBend)),
  ZChopper(new ZoomChopper(*A.ZChopper)),
  ZDisk(new constructSystem::DiskChopper(*A.ZDisk)),
  ZCollimator(new ZoomCollimator(*A.ZCollimator)),
  ZColInsert(new ZoomOpenStack(*A.ZColInsert)),
  ZPrim(new ZoomPrimary(*A.ZPrim)),
  ZHut(new ZoomHutch(*A.ZHut))
  /*!
    Copy constructor
    \param A :: makeZoom to copy
  */
{}

makeZoom&
makeZoom::operator=(const makeZoom& A)
  /*!
    Assignment operator
    \param A :: makeZoom to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *ZBend= *A.ZBend;
      *ZChopper=*A.ZChopper;
      *ZDisk=*A.ZDisk;
      *ZCollimator=*A.ZCollimator;
      *ZColInsert=*A.ZColInsert;
      *ZPrim=*A.ZPrim;
      *ZHut= *A.ZHut;
    }
  return *this;
}

makeZoom::~makeZoom()
  /*!
    Destructor
   */
{}

void
makeZoom::buildIsolated(Simulation& System)
  /*!
    Carry out the build with no linkage
    \param System :: Simulation system
   */
{
  ELog::RegMethod RegA("makeZoom","buildIsolated");
  const int voidCell(74123);
  
  ZBend->addInsertCell("A",voidCell);
  ZBend->addInsertCell("B",voidCell);
  ZBend->addInsertCell("C",voidCell);
  ZBend->addInsertCell("D",voidCell);

  ZBend->createAll(System,World::masterOrigin(),0); 

  ZChopper->addInsertCell(voidCell);
  ZChopper->createAll(System,*ZBend);

  ZDisk->addInsertCell(ZChopper->getVoidCell());
  ZDisk->createAll(System,*ZChopper,0);

  ZCollimator->addInsertCell(voidCell);
  ZCollimator->setCutSurf("front",ZChopper->getLinkSurf(2));
  ZCollimator->createAll(System,*ZChopper,0);

  ZColInsert->addInsertCell(ZCollimator->getVoidCell());
  //  ZColInsert->setCutSurf("front",ZCollimator->getLinkSurf(2));
  ZColInsert->createAll(System,*ZCollimator,0);


  ZPrim->addInsertCell(voidCell);
  ZPrim->createAll(System,*ZCollimator,0);
  
  return;
}


void 
makeZoom::build(Simulation& System,
		const mainSystem::inputParam& IParam,
		const shutterSystem::BulkShield& BulkObj)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
    \param BulkObj :: BulkShield object
   */
{
  // For output stream
  ELog::RegMethod RControl("makeZoom","build");
  
  const int voidCell(74123);
  // Exit if no work to do:
  if (IParam.flag("exclude") && IParam.compValue("E",std::string("Zoom")))
      return;
  
  const size_t zsNumber(shutterSystem::BulkShield::zoomShutter);
  const shutterSystem::ZoomShutter* ZS=
    dynamic_cast<const shutterSystem::ZoomShutter*>
    (BulkObj.getShutter(zsNumber));
  if (!ZS)
    throw ColErr::InContainerError<size_t>(zsNumber,"Shutter number");
  
  const shutterSystem::BulkInsert* BOPtr=BulkObj.getInsert(zsNumber);

  ZBend->addInsertCell("A",BOPtr->getInnerVoid());
  ZBend->addInsertCell("B",BOPtr->getInnerVoid());
  ZBend->addInsertCell("A",BOPtr->getOuterVoid());
  ZBend->addInsertCell("B",BOPtr->getOuterVoid());
  ZBend->addInsertCell("C",BOPtr->getOuterVoid());
  ZBend->createAll(System,*ZS,0);
  
  ZChopper->addInsertCell(voidCell);
  ZChopper->setMonoSurface(BulkObj.getMonoSurf());
  ZChopper->createAll(System,*ZBend,*ZS);
  
  ZDisk->addInsertCell(ZChopper->getVoidCell());
  ZDisk->createAll(System,*ZChopper,0);
  
  ZCollimator->addInsertCell(voidCell);
  ZCollimator->createAll(System,*ZChopper,0);

  ZColInsert->addInsertCell(voidCell);
  ZColInsert->createAll(System,*ZCollimator,0);
  
  ZPrim->addInsertCell(voidCell);
  ZPrim->createAll(System,*ZCollimator,0);
  
  ZHut->addInsertCell(voidCell);
  ZHut->createAll(System,*ZPrim,0); 

  return;
}

}   // NAMESPACE HutchSystem
