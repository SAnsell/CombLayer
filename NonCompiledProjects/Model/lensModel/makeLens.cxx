/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/makeLens.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "siModerator.h"
#include "candleStick.h"
#include "ProtonFlight.h"
#include "FlightLine.h"
#include "FlightCluster.h"
#include "layers.h"

#include "LensTally.h"
#include "makeLens.h"

namespace lensSystem
{

makeLens::makeLens() :
  SiModObj(new siModerator("siMod")),
  candleObj(new candleStick("candle")),
  layerObj(new layers("layers"))

  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeLens","Constructor");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(SiModObj);
  OR.addObject(candleObj);
  OR.addObject(layerObj);
}


makeLens::~makeLens()
  /*!
    Destructor
   */
{}

void
makeLens::setMaterials(const mainSystem::inputParam&)
  /*!
    Builds all the materials in the objects 
    \param IParam :: Table of information
  */
{
  //  const std::string PName=IParam.getValue<std::string>("polyMat");
  //  const std::string SiName=IParam.getValue<std::string>("siMat");

  //  SiModObj->setMaterial(PName,);
  return;
}


void 
makeLens::build(Simulation* SimPtr)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
   */
{
  // For output stream
  ELog::RegMethod RControl("makeLens","build");

  layerObj->addInsertCell(74123);  

  SiModObj->createAll(*SimPtr,World::masterOrigin(),0);
  candleObj->setBasePoint(SiModObj->getLinkPt(5));
  candleObj->createAll(*SimPtr,*SiModObj,0);

  layerObj->setCutSurf("FrontCandleStick",*candleObj,2);
  layerObj->setCutSurf("CandleStickOuter",candleObj->getOuterSurf());
  layerObj->setCutSurf("CandleStickTop",candleObj->getTopExclude());
  layerObj->build(*SimPtr,*candleObj);
  return;
}
  

}   // NAMESPACE lensSystem
