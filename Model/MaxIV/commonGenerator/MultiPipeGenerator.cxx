/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MultiPipeGenerator.cxx
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
#include "subPipeUnit.h"
#include "MultiPipeGenerator.h"

namespace setVariable
{

MultiPipeGenerator::MultiPipeGenerator() :
  flangeRadius(CF150::flangeRadius),
  flangeLength(CF150::flangeLength),
  voidMat("Void"),pipeMat("Stainless304L"),
  flangeMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

MultiPipeGenerator::~MultiPipeGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
MultiPipeGenerator::setPipe(const Geometry::Vec3D& XZoffset,
			    const double len,const double XYang,
			    const double Zang)
  /*!
    Set pipe x,z position
    \param XZOffset :: Vector offset
    \param len :: length of pipe
    \param XYang :: XYangle
    \param Zang :: XYangle
    \param wMat :: wall Material
  */
{
  tdcSystem::subPipeUnit AUnit("A"+std::to_string(pipes.size()));

  AUnit.xStep=XZoffset[0];
  AUnit.zStep=XZoffset[2];
  AUnit.xyAngle=XYang;
  AUnit.zAngle=Zang;
  AUnit.radius=CF::innerRadius;
  AUnit.length=len;
  AUnit.thick=CF::wallThick;
  AUnit.flangeRadius=CF::flangeRadius;
  AUnit.flangeLength=CF::flangeLength;
  
  pipes.push_back(AUnit);
  return;
}

template<typename CF>
void
MultiPipeGenerator::setFlangeCF()
  /*!
    Set flangeto CF-X format
  */
{
  flangeRadius=CF::flangeRadius;
  flangeLength=CF::flangeLength;

  return;
}

void
MultiPipeGenerator::generateMulti(FuncDataBase& Control,
				  const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param JLen :: length of joined pipe
    \param FLen :: full length of pipes
  */
{
  ELog::RegMethod RegA("MultiPipeGenerator","generatorMulti");

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);
  Control.addVariable(keyName+"FlangeMat",flangeMat);

  Control.addVariable(keyName+"NPipes",pipes.size());
  size_t index(0);
  for(const tdcSystem::subPipeUnit& pUnit : pipes)
    {
      const std::string unitName=
	keyName+"SubPipe"+std::to_string(index);
      
      Control.addVariable(unitName+"XStep",pUnit.xStep);
      Control.addVariable(unitName+"ZStep",pUnit.zStep);
      Control.addVariable(unitName+"XYAngle",pUnit.xyAngle);
      Control.addVariable(unitName+"ZAngle",pUnit.zAngle);
      Control.addVariable(unitName+"Radius",pUnit.radius);
      Control.addVariable(unitName+"Length",pUnit.length);
      Control.addVariable(unitName+"Thick",pUnit.thick);
      Control.addVariable(unitName+"FlangeRadius",pUnit.flangeRadius);
      Control.addVariable(unitName+"FlangeLength",pUnit.flangeLength);
      Control.addVariable(unitName+"VoidMat",voidMat);
      Control.addVariable(unitName+"WallMat",pipeMat);
      Control.addVariable(unitName+"FlangeMat",flangeMat);
      index++;
    }
  return;
}


///\cond TEMPLATE


  template
  void MultiPipeGenerator::setPipe<CF40>(const Geometry::Vec3D&,
					 const double,const double,
					 const double);
  template
  void MultiPipeGenerator::setPipe<CF40_22>(const Geometry::Vec3D&,
					    const double,const double,
					    const double);
  
///\endcond TEMPLATE


  
}  // NAMESPACE setVariable
