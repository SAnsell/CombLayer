/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/makeExample.cxx
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
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GroupOrigin.h"
#include "World.h"
#include "AttachSupport.h"
#include "dipolePipe.h"

#include "makeExample.h"

namespace pipeSystem
{

makeExample::makeExample() :
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


}

makeExample::makeExample(const makeExample& A) : 
  ATube(new pipeSystem::pipeTube(*(A.ATube))),
  BTube(new pipeSystem::pipeTube(*(A.BTube))),
  CTube(new pipeSystem::pipeTube(*(A.CTube)))
  /*!
    Copy constructor
    \param A :: makeExample to copy
  */
{}

makeExample&
makeExample::operator=(const makeExample& A)
  /*!
    Assignment operator
    \param A :: makeExample to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *ATube=*A.ATube;
      *BTube=*A.BTube;
      *CTube=*A.CTube;
    }
  return *this;
}

makeExample::~makeExample()
  /*!
    Destructor
   */
{}

void 
makeExample::build(Simulation* SimPtr,
		     const mainSystem::inputParam&)
/*!
  Carry out the full build
  \param SimPtr :: Simulation system
  \param :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makeExample","build");

  int voidCell(74123);

  DPipe->addInsertCell(voidCell);
  DPipe->createAll(*SimPtr,World::masterObject(),0);

  return;
}


}   // NAMESPACE exampleSystem

