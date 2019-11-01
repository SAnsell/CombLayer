/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   saxsModel/makeSAXS.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"

#include "insertObject.h"
#include "insertCylinder.h"
#include "CylSample.h"
#include "Capillary.h"
#include "AreaPlate.h"
#include "makeSAXS.h"

namespace saxsSystem
{

makeSAXS::makeSAXS() :
  sampleObj(new Capillary("SampleCap")),
  waterObj(new Capillary("WaterCap")),
  energyObj(new Capillary("EnergyCap")),
  areaPlate(new AreaPlate("AreaPlate")),
  eCutDisk(new insertSystem::insertCylinder("ECutDisk"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(sampleObj);
  OR.addObject(waterObj);
  OR.addObject(energyObj);
  OR.addObject(areaPlate);
  OR.addObject(eCutDisk);
    
}

makeSAXS::~makeSAXS()
  /*!
    Destructor
   */
{}


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

  const int voidCell(74123); 
  
  sampleObj->setInsertCell(voidCell);
  sampleObj->createAll(System,World::masterOrigin(),0);

  waterObj->setInsertCell(voidCell);
  waterObj->createAll(System,*sampleObj,0);

  energyObj->setInsertCell(voidCell);
  energyObj->createAll(System,*waterObj,0);

  ELog::EM<<"Area Plate "<<ELog::endDiag;
  areaPlate->setInsertCell(voidCell);
  areaPlate->createAll(System,*sampleObj,0);

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(voidCell);
  eCutDisk->createAll(System,World::masterOrigin(),0);

  return;
}



}   // NAMESPACE saxsSystem
