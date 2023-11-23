/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/LegoBrickGenerator.cxx
 *
 * Modified: Isabel Llamas-Jansa, November 2023 
 * - added comments for self
 * - added copy of constructor
 * - added 
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "LegoBrickGenerator.h"

namespace setVariable
{

LegoBrickGenerator::LegoBrickGenerator() :
  width(100.0),height(20.0),depth(35.0),
  mat("Stainless304")

  /*!
    Constructor and defaults
  */
{}

LegoBrickGenerator::LegoBrickGenerator(const LegoBrickGenerator& A) : 
  width(A.width),height(A.height),
  depth(A.depth),mat(A.mat)
  /*!
    This is a copy of the constructor above, with A as key.
    Added some parameters for the brick: width, height, depth and material.
    \param A :: LegoBrick to copy; each copy of the brick is identified by parameter A
    \param A :: LegoBrickGenerator to copy
  */
{}

LegoBrickGenerator&
LegoBrickGenerator::operator=(const LegoBrickGenerator& A)
  /*!
    Assignment operator as function of the LegoBrick
    variable and the parameter A
    If the condition: this is not equal to the A pointer,
    the operator assigns the values and returns
    \return *this
    \param A :: LegoBrickGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      width=A.width;
      height=A.height;
      depth=A.depth;
      mat=A.mat;      
    }
  return *this;
}

LegoBrickGenerator::~LegoBrickGenerator() 
 /*!
   Destructor
 */
{}
  
void

LegoBrickGenerator::generateBrick
(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("LegoBrickGenerator","generatorBrick");
  
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Mat",mat);
  
  return;
}

}  // NAMESPACE setVariable
