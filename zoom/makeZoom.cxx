/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/makeZoom.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

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
#include "Qhull.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "InsertComp.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
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
#include "ZoomStack.h"
#include "ZoomCollimator.h"
#include "ZoomTank.h"
#include "ZoomHutch.h"
#include "ZoomRoof.h"
#include "ZoomPrimary.h"
#include "makeZoom.h"

namespace zoomSystem
{

makeZoom::makeZoom() :
  ZBend(new ZoomBend("zoomBend")),
  ZChopper(new ZoomChopper("zoomChopper")),
  ZCollimator(new ZoomCollimator("zoomCollimator")),
  ZRoof(new ZoomRoof("zoomRoof")),
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
  OR.addObject("zoomCollimator",ZCollimator);
  OR.addObject("zoomPrimary",ZPrim);
  OR.addObject("zoomHutch",ZBend);
}

makeZoom::makeZoom(const makeZoom& A) : 
  ZBend(new ZoomBend(*A.ZBend)),
  ZChopper(new ZoomChopper(*A.ZChopper)),
  ZCollimator(new ZoomCollimator(*A.ZCollimator)),
  ZRoof(new ZoomRoof(*A.ZRoof)),
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
      *ZCollimator=*A.ZCollimator;
      *ZRoof=*A.ZRoof;
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
makeZoom::build(Simulation* SimPtr,
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
  
  int isoFlag(0);
  // Exit if no work to do:
  if (IParam.flag("exclude") && IParam.compValue("E",std::string("Zoom")))
    return;
  if (IParam.flag("isolate") && IParam.compValue("I",std::string("zoom")))
    isoFlag=1;

  const size_t zsNumber(shutterSystem::BulkShield::zoomShutter);

  const shutterSystem::ZoomShutter* ZS=
    dynamic_cast<const shutterSystem::ZoomShutter*>
    (BulkObj.getShutter(zsNumber));
  if (!ZS)
    ELog::EM<<"NO SHUTTER "<<ELog::endErr;

  if (ZS)
    {
      const shutterSystem::BulkInsert* BOPtr=BulkObj.getInsert(zsNumber);
      ZBend->addInsertCell("A",BOPtr->getInnerVoid());
      ZBend->addInsertCell("B",BOPtr->getInnerVoid());
      ZBend->addInsertCell("A",BOPtr->getOuterVoid());
      ZBend->addInsertCell("B",BOPtr->getOuterVoid());
      ZBend->addInsertCell("C",BOPtr->getOuterVoid());
      ZBend->createAll(*SimPtr,*ZS); 

      ZChopper->addInsertCell(74123);
      ZChopper->setMonoSurface(BulkObj.getMonoSurf());
      ZChopper->createAll(*SimPtr,*ZBend,*ZS);

      ZCollimator->addInsertCell(74123);
      ZCollimator->createAll(*SimPtr,*ZChopper);
      
      ZRoof->addInsertCell(74123);
      ZRoof->setMonoSurface(BulkObj.getMonoSurf());
      ZRoof->createAll(*SimPtr,*ZS,*ZChopper,*ZCollimator);

      ZPrim->addInsertCell(74123);
      ZPrim->createAll(*SimPtr,*ZCollimator);
    }
  
  ZHut->addInsertCell(74123);
  ZHut->createAll(*SimPtr,*ZPrim); 

  return;
}

}   // NAMESPACE HutchSystem
