/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   saxsModel/makeSAXS.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "World.h"
#include "CylSample.h"
#include "BellJar.h"
#include "DetectorArray.h"
#include "DetectorBank.h"
#include "Detector.h"
#include "DetGroup.h"
#include "SimMonte.h"
#include "makeSAXS.h"

namespace saxsSystem
{

makeSAXS::makeSAXS() :
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

makeSAXS::~makeSAXS()
  /*!
    Destructor
   */
{}

void
makeSAXS::setMaterials(const mainSystem::inputParam& IParam)
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
makeSAXS::build(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
  */
{
  // For output stream
  ELog::RegMethod RControl("makeSAXS","build");

  BellObj->setInsertCell(74123);
  BellObj->createAll(System,World::masterOrigin(),0);
  CellObj->setInsertCell(BellObj->innerCell());
  CellObj->createAll(System,*BellObj,0);

  SimMonte* SM=dynamic_cast<SimMonte*>(&System);
  for(size_t i=0;i<NDet;i++)
    {
      DetObj[i]->setInsertCell(BellObj->outerCell());
      DetObj[i]->createAll(System,*BellObj,0);
      DetObj[i]->createTally(System);
      
      if (SM)
	DetObj[i]->createTally(SM->getDU());
    }

  return;
}



}   // NAMESPACE lensSystem
