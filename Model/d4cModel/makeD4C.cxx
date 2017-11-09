/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModel/makeD4C.cxx
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
#include <boost/format.hpp>

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
#include "support.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "CylSample.h"
#include "BellJar.h"
#include "DetectorArray.h"
#include "DetectorBank.h"
#include "Detector.h"
#include "DetGroup.h"
#include "SimMonte.h"
#include "makeD4C.h"

namespace d4cSystem
{

makeD4C::makeD4C() :
  NDet(9),BellObj(new BellJar("belljar")),
  CellObj(new instrumentSystem::CylSample("cell"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for(size_t i=0;i<NDet;i++)
    {
      DetObj[i]=std::shared_ptr<DetectorBank> 
	(new DetectorBank(i,"DetBank"));
      OR.addObject(DetObj[i]);
    }

  OR.addObject(BellObj);
  OR.addObject(CellObj);
}

makeD4C::~makeD4C()
  /*!
    Destructor
   */
{}

void
makeD4C::setMaterials(const mainSystem::inputParam& IParam)
  /*!
    Builds all the materials in the objects 
    \param IParam :: Table of information
  */
{
  const std::string PName=IParam.getValue<std::string>("polyMat");
  const std::string SiName=IParam.getValue<std::string>("siMat");

  //  SiModObj->setMaterial(PName,);
  return;
}


void 
makeD4C::build(Simulation* SimPtr,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
  */
{
  // For output stream
  ELog::RegMethod RControl("makeD4C","build");

  BellObj->setInsertCell(74123);
  BellObj->createAll(*SimPtr,World::masterOrigin());
  CellObj->setInsertCell(BellObj->innerCell());
  CellObj->createAll(*SimPtr,*BellObj,0);
  SimMonte* SM=dynamic_cast<SimMonte*>(SimPtr);
  for(size_t i=0;i<NDet;i++)
    {
      DetObj[i]->setInsertCell(BellObj->outerCell());
      DetObj[i]->createAll(*SimPtr,*BellObj);
      DetObj[i]->createTally(*SimPtr);
      
      if (SM)
	DetObj[i]->createTally(SM->getDU());
    }

  return;
}



}   // NAMESPACE lensSystem
