/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   filter/makeFilter.cxx
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
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BoundOuter.h"
#include "LayerComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "CylContainer.h"
#include "CylLayer.h"
#include "makeFilter.h"

#include "Debug.h"

namespace filterSystem
{

makeFilter::makeFilter() :
  SiPrimary(new photonSystem::CylLayer("SiPrimary")),
  SiSecond(new photonSystem::CylLayer("SiSecond")),
  Lead(new photonSystem::CylLayer("Lead")),
  SiThird(new photonSystem::CylLayer("SiThird"))
  
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(SiPrimary);
  OR.addObject(SiSecond);
  OR.addObject(Lead);
  OR.addObject(SiThird);

}


makeFilter::~makeFilter()
  /*!
    Destructor
   */
{}


void 
makeFilter::build(Simulation& System,
		  const mainSystem::inputParam& IParam)
/*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeFilter","build");

  int voidCell(74123);
  
  SiPrimary->addInsertCell(voidCell);
  SiPrimary->createAll(System,World::masterOrigin(),0);

  SiSecond->addInsertCell(voidCell);
  SiSecond->createAll(System,*SiPrimary,0);

  Lead->addInsertCell(voidCell);
  Lead->createAll(System,*SiSecond,0);

  SiThird->addInsertCell(voidCell);
  SiThird->createAll(System,*Lead,0);

  return;
}


}   // NAMESPACE filterSystem

