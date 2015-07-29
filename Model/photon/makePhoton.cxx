/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   photon/makePhoton.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "CylContainer.h"
#include "CylLayer.h"
#include "TubeMod.h"
#include "makePhoton.h"

#include "Debug.h"

namespace photonSystem
{

makePhoton::makePhoton() :
  CatcherObj(new CylLayer("Catcher")),
  OuterObj(new CylContainer("Outer")),
  PrimObj(new CylLayer("PrimMod")),
  CarbonObj(new CylLayer("Carbon")),
  ModObj(new TubeMod("D2OMod"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(CatcherObj);
  OR.addObject(OuterObj);
  OR.addObject(PrimObj);
  OR.addObject(CarbonObj);
  OR.addObject(ModObj);

}

makePhoton::makePhoton(const makePhoton& A) : 
  OuterObj(new CylContainer(*A.OuterObj)),
  PrimObj(new CylLayer(*A.PrimObj)),
  CarbonObj(new CylLayer(*A.CarbonObj))
  /*!
    Copy constructor
    \param A :: makePhoton to copy
  */
{}

makePhoton&
makePhoton::operator=(const makePhoton& A)
  /*!
    Assignment operator
    \param A :: makePhoton to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *OuterObj = *A.OuterObj;
      *PrimObj = *A.PrimObj;
    }
  return *this;
}


makePhoton::~makePhoton()
  /*!
    Destructor
   */
{}


void 
makePhoton::build(Simulation* SimPtr,
		  const mainSystem::inputParam& IParam)
/*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makePhoton","build");

  int voidCell(74123);

  
  OuterObj->addInsertCell(voidCell);
  OuterObj->createAll(*SimPtr,World::masterOrigin());

  CatcherObj->addInsertCell(voidCell);
  CatcherObj->createAll(*SimPtr,*OuterObj,-1);

  PrimObj->createAll(*SimPtr,*OuterObj,-1);
  attachSystem::addToInsertLineCtrl(*SimPtr,*OuterObj,*PrimObj);

  //  CarbonObj->createAll(*SimPtr,*PrimObj,2);
  //  attachSystem::addToInsertLineCtrl(*SimPtr,*OuterObj,*CarbonObj);

  ModObj->addInsertCell(voidCell);
  ModObj->createAll(*SimPtr,*PrimObj,2);

  attachSystem::addToInsertLineCtrl(*SimPtr,*OuterObj,*ModObj);
  return;
}


}   // NAMESPACE photonSystem

