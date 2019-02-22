/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MazeGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "MazeGenerator.h"

namespace setVariable
{

MazeGenerator::MazeGenerator() :
  height(180.0),width(100.0),
  mainOut(118.0),mainThick(100.0),mainXWidth(162.0),
  catchOut(298.0),catchThick(30.0),catchXGap(262.0),
  wallMat("Concrete")
  /*!
    Constructor and defaults
  */
{}


MazeGenerator::~MazeGenerator() 
 /*!
   Destructor
 */
{}

void
MazeGenerator::setMain(const double L,
		       const double T,
		       const double W)
  
  /*!
    Set the mirror plate
    \param L :: Length of mirror [Y]
    \param T :: Thickness [Z] of mirror plate
    \param W :: Widht [X]  mirror plate
   */
{
  mainOut=L;
  mainThick=T;
  mainXWidth=W;
  return;
}

				  
void
MazeGenerator::generateMaze(FuncDataBase& Control,
			    const std::string& keyName,
			    const double xStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param xStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("MazeGenerator","generateMaze");
  
  Control.addVariable(keyName+"Active",1);
  Control.addVariable(keyName+"XStep",xStep);

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Width",width);
  
  Control.addVariable(keyName+"MainOutLength",mainOut);
  Control.addVariable(keyName+"MainXWidth",mainXWidth);
  Control.addVariable(keyName+"MainThick",mainThick);
  
  Control.addVariable(keyName+"CatchOutLength",catchOut);
  Control.addVariable(keyName+"CatchXGap",catchXGap);
  Control.addVariable(keyName+"CatchThick",catchThick);


  Control.addVariable(keyName+"WallMat",wallMat);
       
  return;

}

  
}  // NAMESPACE setVariable
