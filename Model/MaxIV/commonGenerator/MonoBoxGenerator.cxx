/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MonoBoxGenerator.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"

namespace setVariable
{

MonoBoxGenerator::MonoBoxGenerator() :
  VacBoxGenerator(),
  overHang(3.0),baseThick(1.0),roofThick(1.0)
  /*!
    Constructor and defaults
  */
{}


MonoBoxGenerator::~MonoBoxGenerator() 
 /*!
   Destructor
 */
{}

void
MonoBoxGenerator::setLids(const double OH,
			  const double BT,
			  const double RT)
  /*!
    Set Mono varibles				
    \param OH :: OverHang
    \param BT :: Base thick
    \param RT :: Roof thick
  */
{
  overHang=OH;
  baseThick=BT;
  roofThick=RT;
  return;
}  

void
MonoBoxGenerator::generateBox(FuncDataBase& Control,const std::string& keyName,
			     const double yStep,const double width,const
			     double height,const double depth,
			     const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param height :: height of box
    \param depth :: depth of box
    \param width :: width of box (full)
    \param length :: length of box - ports
  */
{
  ELog::RegMethod RegA("MonoBoxGenerator","generateBox");
  

  VacBoxGenerator::generateBox(Control,keyName,yStep,width,
			       height,depth,length);

  Control.addVariable(keyName+"OverHang",overHang);
  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"RoofThick",roofThick);
  return;

}
  
}  // NAMESPACE setVariable
