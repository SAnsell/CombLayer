/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/HeatDumpGenerator.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "HeatDumpGenerator.h"

namespace setVariable
{

HeatDumpGenerator::HeatDumpGenerator() :
  radius(4.0),height(20.0),width(3.0),
  thick(8.0),lift(6.0),cutHeight(2.0),
  cutAngle(30.0),cutDepth(1.0),
  topInnerRadius(CF100::innerRadius),
  topFlangeRadius(CF100::flangeRadius),
  bellowLength(2.0),bellowThick(CF100::bellowThick),
  outRadius(CF100::flangeRadius),
  outLength(CF100::flangeLength),
  mat("Copper"),flangeMat("Stainless304"),
  bellowMat("Stainless304%Void%10.0")
  /*!
    Constructor and defaults
  */
{}


HeatDumpGenerator::HeatDumpGenerator(const HeatDumpGenerator& A) : 
  radius(A.radius),height(A.height),width(A.width),
  thick(A.thick),lift(A.lift),cutHeight(A.cutHeight),
  cutAngle(A.cutAngle),cutDepth(A.cutDepth),topInnerRadius(A.topInnerRadius),
  topFlangeRadius(A.topFlangeRadius),topFlangeLength(A.topFlangeLength),
  bellowLength(A.bellowLength),bellowThick(A.bellowThick),
  outRadius(A.outRadius),outLength(A.outLength),
  mat(A.mat),flangeMat(A.flangeMat),bellowMat(A.bellowMat)
  /*!
    Copy constructor
    \param A :: HeatDumpGenerator to copy
  */
{}

HeatDumpGenerator&
HeatDumpGenerator::operator=(const HeatDumpGenerator& A)
  /*!
    Assignment operator
    \param A :: HeatDumpGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      radius=A.radius;
      height=A.height;
      width=A.width;
      thick=A.thick;
      lift=A.lift;
      cutHeight=A.cutHeight;
      cutAngle=A.cutAngle;
      cutDepth=A.cutDepth;
      topInnerRadius=A.topInnerRadius;
      topFlangeRadius=A.topFlangeRadius;
      topFlangeLength=A.topFlangeLength;
      bellowLength=A.bellowLength;
      bellowThick=A.bellowThick;
      outRadius=A.outRadius;
      outLength=A.outLength;
      mat=A.mat;
      flangeMat=A.flangeMat;
      bellowMat=A.bellowMat;
    }
  return *this;
}

  
HeatDumpGenerator::~HeatDumpGenerator() 
 /*!
   Destructor
 */
{}



void
HeatDumpGenerator::setMat(const std::string& M,const double T)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param M :: Main material
    \param T :: Percentage of material
  */
{
  flangeMat=M;
  bellowMat=M+"%Void%"+std::to_string(T);
  return;
}
  
				  
void
HeatDumpGenerator::generateHD(FuncDataBase& Control,
			      const std::string& keyName,
			      const bool upFlag) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param upFlag :: Heat Dum up
  */
{
  ELog::RegMethod RegA("HeatDumpGenerator","generateColl");
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Thick",thick);
  Control.addVariable(keyName+"Lift",lift);
  Control.addVariable(keyName+"UpFlag",static_cast<int>(upFlag));

  Control.addVariable(keyName+"CutHeight",cutHeight);
  Control.addVariable(keyName+"CutAngle",cutAngle);
  Control.addVariable(keyName+"CutDepth",cutDepth);

  
  Control.addVariable(keyName+"TopInnerRadius",topInnerRadius);
  Control.addVariable(keyName+"TopFlangeRadius",topFlangeRadius);
  Control.addVariable(keyName+"TopFlangeLength",topFlangeLength);

  Control.addVariable(keyName+"BellowLength",bellowLength);
  Control.addVariable(keyName+"BellowThick",bellowThick);

  Control.addVariable(keyName+"OutLength",outLength);
  Control.addVariable(keyName+"OutRadius",outRadius);

  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"FlangeMat",flangeMat);
  Control.addVariable(keyName+"BellowMat",bellowMat);

       
  return;

}

  
}  // NAMESPACE setVariable
