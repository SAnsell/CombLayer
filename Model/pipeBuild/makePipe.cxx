/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/makePipe.cxx
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GroupOrigin.h"
#include "World.h"
#include "AttachSupport.h"
#include "pipeTube.h"

#include "makePipe.h"

namespace pipeSystem
{

makePipe::makePipe() :
  ATube(new pipeSystem::pipeTube("ATube")),
  BTube(new pipeSystem::pipeTube("BTube")),
  CTube(new pipeSystem::pipeTube("CTube"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ATube);
  OR.addObject(BTube);
  OR.addObject(CTube);

}

makePipe::makePipe(const makePipe& A) : 
  ATube(new pipeSystem::pipeTube(*(A.ATube))),
  BTube(new pipeSystem::pipeTube(*(A.BTube))),
  CTube(new pipeSystem::pipeTube(*(A.CTube)))
  /*!
    Copy constructor
    \param A :: makePipe to copy
  */
{}

makePipe&
makePipe::operator=(const makePipe& A)
  /*!
    Assignment operator
    \param A :: makePipe to copy
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

makePipe::~makePipe()
  /*!
    Destructor
   */
{}

void 
makePipe::build(Simulation* SimPtr,
		       const mainSystem::inputParam&)
/*!
  Carry out the full build
  \param SimPtr :: Simulation system
  \param :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makePipe","build");

  int voidCell(74123);

  ATube->addInsertCell(voidCell);
  ATube->createAll(*SimPtr,World::masterOrigin(),0);

  BTube->addInsertCell(voidCell);
  BTube->createAll(*SimPtr,*ATube,2);

  CTube->addInsertCell(voidCell);
  CTube->createAll(*SimPtr,*BTube,2);
  return;
}


}   // NAMESPACE pipeSystem

