/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/CrossGenerator.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "CrossGenerator.h"

namespace setVariable
{

CrossGenerator::CrossGenerator() :
  wallThick(0.5),topThick(1.0),baseThick(1.0),
  frontLen(5.0),backLen(5.0),
  flangeRadius(1.0),flangeLen(1.0),
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

CrossGenerator::CrossGenerator(const CrossGenerator& A) : 
  wallThick(A.wallThick),topThick(A.topThick),baseThick(A.baseThick),
  frontLen(A.frontLen),backLen(A.backLen),flangeRadius(A.flangeRadius),
  flangeLen(A.flangeLen),voidMat(A.voidMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: CrossGenerator to copy
  */
{}

CrossGenerator&
CrossGenerator::operator=(const CrossGenerator& A)
  /*!
    Assignment operator
    \param A :: CrossGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      wallThick=A.wallThick;
      topThick=A.topThick;
      baseThick=A.baseThick;
      frontLen=A.frontLen;
      backLen=A.backLen;
      flangeRadius=A.flangeRadius;
      flangeLen=A.flangeLen;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}

CrossGenerator::~CrossGenerator() 
 /*!
   Destructor
 */
{}

void
CrossGenerator::setPlates(const double WT,const double TT,const double BT)
  /*!
    Set the wall and top/base plate thickness
    \param WT :: Wall thick
    \param TT :: Top thick
    \param BT :: Base thick
  */
{
  wallThick=WT;
  topThick=TT;
  baseThick=BT;
  return;
}


void
CrossGenerator::setPorts(const double LenF,const double LenB)
  /*!
    Set both the ports extension on horrizontal
    \param LenF :: length of port tube
    \param LenB :: Thickness of port tube
   */
{
  frontLen= -LenF;
  backLen=  -LenB;
  return;
}

void
CrossGenerator::setFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeRadius=R;
  flangeLen=L;
  return;
}




template<typename HCF>
void
CrossGenerator::generateCF
    (FuncDataBase& Control,const std::string& keyName,
     const double yStep,const double VRad,
     const double height,const double depth) 
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param VRad :: vertical radius
    \param height :: height of box
    \param depth :: depth of box
    \param width :: width of box (full)
  */
{
  ELog::RegMethod RegA("CrossGenerator","generatorCF");

  flangeRadius=HCF::flangeRadius;
  flangeLen=HCF::flangeLength;
  generateCross(Control,keyName,yStep,
		HCF::innerRadius,VRad,
		height,depth);
  return;
}

template<typename HCF,typename VCF>
void
CrossGenerator::generateDoubleCF
    (FuncDataBase& Control,const std::string& keyName,
     const double yStep, const double height,const double depth) 
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param height :: height of box
    \param depth :: depth of box
    \param width :: width of box (full)
  */
{
  ELog::RegMethod RegA("CrossGenerator","generatorDoubleCF");

  flangeRadius=HCF::flangeRadius;
  flangeLen=HCF::flangeLength;
  generateCross(Control,keyName,yStep,HCF::innerRadius,
		VCF::innerRadius,height,depth);
  return;
}
  
void
CrossGenerator::generateCross(FuncDataBase& Control,const std::string& keyName,
			      const double yStep,const double HRad,
			      const double VRad,const double height,
			      const double depth) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param HRad :: horrizontal radius
    \param VRad :: vertical radius
    \param height :: height of box
    \param depth :: depth of box
    \param width :: width of box (full)
  */
{
  ELog::RegMethod RegA("CrossGenerator","generatorCross");
  
  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"HorrRadius",HRad);
  Control.addVariable(keyName+"VertRadius",VRad);
  
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  const double FL=(frontLen<0.0) ? VRad-frontLen : frontLen;
  const double BL=(backLen<0.0) ? VRad-backLen : backLen;
  Control.addVariable(keyName+"FrontLength",FL);
  Control.addVariable(keyName+"BackLength",BL);

  Control.addVariable(keyName+"FeThick",wallThick);
  Control.addVariable(keyName+"TopPlate",topThick);
  Control.addVariable(keyName+"BasePlate",baseThick);

  ELog::EM<<"FL["<<keyName<<"] == "<<flangeRadius<<ELog::endDiag;
  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLen);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"FeMat",wallMat);
       
  return;

}

///\cond TEMPLATE

  template void CrossGenerator::generateCF<CF40>
  (FuncDataBase&,const std::string&,const double,const double,
   const double,const double);
  template void CrossGenerator::generateCF<CF63>
  (FuncDataBase&,const std::string&,const double,const double,
   const double,const double);
  template void CrossGenerator::generateCF<CF100>
  (FuncDataBase&,const std::string&,const double,const double,
   const double,const double);
  
  template void CrossGenerator::generateDoubleCF<CF40,CF63>
  (FuncDataBase&,const std::string&,const double,const double,const double);

  
///\endcond TEMPLATE
  
}  // NAMESPACE setVariable
