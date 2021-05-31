/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exampleBuild/makeExample.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "OutputLog.h"
#include "MatrixBase.h"
#include "inputParam.h"
#include "objectRegister.h"
#include "World.h"

#include "dipoleModel.h"
#include "makeLinacTube.h"
#include "makeExample.h"

namespace exampleSystem
{

makeExample::makeExample() 
  /*!
    Constructor
  */
{}

makeExample::~makeExample()
  /*!
    Destructor
   */
{}

void 
makeExample::build(Simulation& System,
		   const mainSystem::inputParam& IParam)
/*!
  Carry out the full build
  \param SimPtr :: Simulation system
  \param :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makeExample","build");

  const std::string model=IParam.getValue<std::string>("Model");
  if (model=="DIPOLE")
    {
      dipoleModel A;
      A.build(System);
    }
  else if (model=="VACTUBE")
    {
      makeLinacTube A;
      A.build(System,World::masterOrigin(),0);
    }
  else
    {
      throw ColErr::InContainerError<std::string>
	(model,"Model unknown");
    }

  return;
}


}   // NAMESPACE exampleSystem

