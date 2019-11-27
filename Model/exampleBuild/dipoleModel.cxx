/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/dipoleModel.cxx
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
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "GroupOrigin.h"
#include "World.h"
#include "AttachSupport.h"
#include "dipolePipe.h"
#include "quadPipe.h"

#include "dipoleModel.h"

namespace exampleSystem
{

dipoleModel::dipoleModel() :
  DPipe(new dipolePipe("DPipe")),
  QuadA(new quadPipe("QuadA")),
  QuadB(new quadPipe("QuadB")),
  QuadC(new quadPipe("QuadC")),
  QuadD(new quadPipe("QuadD"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.addObject(DPipe);
  OR.addObject(QuadA);
  OR.addObject(QuadB);
  OR.addObject(QuadC);
  OR.addObject(QuadD);

}

dipoleModel::~dipoleModel()
  /*!
    Destructor
   */
{}

void 
dipoleModel::build(Simulation& System)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param :: Input parameters
  */
{
  // For output stream
  ELog::RegMethod RControl("dipoleModel","build");

  int voidCell(74123);

  DPipe->addInsertCell(voidCell);
  DPipe->createAll(System,World::masterOrigin(),0);

  QuadA->addInsertCell(voidCell);
  QuadA->createAll(System,*DPipe,"back");

  QuadB->addInsertCell(voidCell);
  QuadB->createAll(System,*DPipe,"back");

  QuadC->addInsertCell(voidCell);
  QuadC->createAll(System,*DPipe,"back");

  QuadD->addInsertCell(voidCell);
  QuadD->createAll(System,*DPipe,"back");

  return;
}


}   // NAMESPACE exampleSystem

