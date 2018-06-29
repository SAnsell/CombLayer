/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/PortChicaneGenerator.cxx
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

#include "CFFlanges.h"
#include "PortChicaneGenerator.h"

namespace setVariable
{

PortChicaneGenerator::PortChicaneGenerator() :
  width(60.0),height(45.0),
  clearGap(8.0),wallThick(0.8),
  mat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
PortChicaneGenerator::~PortChicaneGenerator() 
 /*!
   Destructor
 */
{}

void
PortChicaneGenerator::setSize(const double G,
			      const double W,
			      const double H)
  /*!
    Set length/width/height
    \param G :: Gap 
    \param W :: width 
    \param H :: height
   */
{
  clearGap=G;
  width=W;
  height=H;    
  return;
}

void
PortChicaneGenerator::setWall(const double WT,const std::string& WMat)
/*!
    Set wall paramaters
    \param WT :: Wall thickness
    \param WMat :: Wall material
   */
{
  wallThick=WT;
  mat=WMat;
  return;
}
				  
void
PortChicaneGenerator::generatePortChicane(FuncDataBase& Control,
					  const std::string& keyName,
					  const double xStep,
					  const double zStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param xStep :: Step left/right fixed centre point
    \param zStep :: Step up/down fixed centre point
  */
{
  ELog::RegMethod RegA("PortChicaneGenerator","generatePortChicane");
  
  Control.addVariable(keyName+"XStep",xStep);
  Control.addVariable(keyName+"YStep",0.0);
  Control.addVariable(keyName+"ZStep",zStep);

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"BaseThick",wallThick);
  Control.addVariable(keyName+"ClearGap",clearGap);

  Control.addVariable(keyName+"WallMat",mat);

       
  return;

}

  
}  // NAMESPACE setVariable
