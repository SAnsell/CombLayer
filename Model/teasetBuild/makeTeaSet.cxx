/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   teaSetBuild/makeTeaSet.cxx
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
#include "Mug.h"

#include "makeTeaSet.h"

namespace teaSetSystem
{

makeTeaSet::makeTeaSet() :
  ATube(new teaSetSystem::Mug("Mug"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(ATube);

}

makeTeaSet::makeTeaSet(const makeTeaSet& A) : 
  ATube(new teaSetSystem::Mug(*(A.ATube)))
  /*!
    Copy constructor
    \param A :: makeTeaSet to copy
  */
{}

makeTeaSet&
makeTeaSet::operator=(const makeTeaSet& A)
  /*!
    Assignment operator
    \param A :: makeTeaSet to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *ATube=*A.ATube;
    }
  return *this;
}

makeTeaSet::~makeTeaSet()
  /*!
    Destructor
   */
{}

void 
makeTeaSet::build(Simulation* SimPtr,
		       const mainSystem::inputParam&)
/*!
  Carry out the full build
  \param SimPtr :: Simulation system
  \param :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makeTeaSet","build");

  int voidCell(74123);

  ATube->addInsertCell(voidCell);
  ATube->createAll(*SimPtr,World::masterOrigin(),0);
  return;
}


}   // NAMESPACE teaSetSystem

