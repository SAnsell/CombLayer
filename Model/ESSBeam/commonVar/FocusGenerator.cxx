/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/FocusGenerator.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "FocusGenerator.h"

namespace setVariable
{

FocusGenerator::FocusGenerator() :
  substrateThick(0.5),voidThick(1.5),yStep(0.0),
  zStep(0.0),guideMat("Aluminium")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FocusGenerator::FocusGenerator(const FocusGenerator& A) : 
  substrateThick(A.substrateThick),voidThick(A.voidThick),
  yStepActive(A.yStepActive),yStep(A.yStep),zStep(A.zStep),
  guideMat(A.guideMat)
  /*!
    Copy constructor
    \param A :: FocusGenerator to copy
  */
{}

FocusGenerator&
FocusGenerator::operator=(const FocusGenerator& A)
  /*!
    Assignment operator
    \param A :: FocusGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      substrateThick=A.substrateThick;
      voidThick=A.voidThick;
      yStepActive=A.yStepActive;
      yStep=A.yStep;
      zStep=A.zStep;
      guideMat=A.guideMat;
    }
  return *this;
}


FocusGenerator::~FocusGenerator() 
 /*!
   Destructor
 */
{}


void
FocusGenerator::generateTaper(FuncDataBase& Control,
			      const std::string& keyName,
			      const double length,
			      const double HS,const double HE,
			      const double VS,const double VE)  const
/*!
    Generate the focus-taper variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param HS :: Horrizontal start
    \param HE :: Horrizontal end
    \param VS :: Vertical start
    \param VE :: Vertical end
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateTaper");
  Control.addVariable(keyName+"Length",length);       
  Control.addVariable(keyName+"XStep",0.0);       
  if (!yStepActive)
    Control.addParse<double>(keyName+"YStep","-"+keyName+"Length/2.0");
  else
    Control.addVariable<double>(keyName+"YStep",yStep);
  Control.copyVar(keyName+"BeamY",keyName+"YStep"); 
  Control.addVariable(keyName+"ZStep",zStep);       
  Control.addVariable(keyName+"XYAngle",0.0);       
  Control.addVariable(keyName+"ZAngle",0.0);

  Control.addVariable(keyName+"NShapes",1);       
  Control.addVariable(keyName+"NShapeLayers",3);
  Control.addVariable(keyName+"ActiveShield",0);

  Control.addVariable(keyName+"LayerThick1",substrateThick);  // glass thick
  Control.addVariable(keyName+"LayerThick2",voidThick);

  Control.addVariable(keyName+"LayerMat0","Void");
  Control.addVariable(keyName+"LayerMat1",guideMat);
  Control.addVariable(keyName+"LayerMat2","Void");       
  
  Control.addVariable(keyName+"0TypeID","Taper");
  Control.addVariable(keyName+"0HeightStart",VS);
  Control.addVariable(keyName+"0HeightEnd",VE);
  Control.addVariable(keyName+"0WidthStart",HS);
  Control.addVariable(keyName+"0WidthEnd",HE);
  Control.copyVar(keyName+"0Length",keyName+"Length");

  return;
}

void
FocusGenerator::generateRectangle(FuncDataBase& Control,
				  const std::string& keyName,
				  const double length,
				  const double H,const double V)  const
/*!
    Generate the focus-taper variables
    \param Control :: Functional data base
    \param keyName :: main name
    \param length :: total length
    \param H :: Horrizontal 
    \param V :: Vertical 

   */
{
  ELog::RegMethod RegA("FocusGenerator","generateTaper");
  Control.addVariable(keyName+"Length",length);       
  Control.addVariable(keyName+"XStep",0.0);       
  if (!yStepActive)
    Control.addParse<double>(keyName+"YStep","-"+keyName+"Length/2.0");
  else
    Control.addVariable<double>(keyName+"YStep",yStep);
  Control.copyVar(keyName+"BeamY",keyName+"YStep"); 
  Control.addVariable(keyName+"ZStep",0.0);       
  Control.addVariable(keyName+"XYAngle",0.0);       
  Control.addVariable(keyName+"ZAngle",0.0);

  Control.addVariable(keyName+"NShapes",1);       
  Control.addVariable(keyName+"NShapeLayers",3);
  Control.addVariable(keyName+"ActiveShield",0);

  Control.addVariable(keyName+"LayerThick1",substrateThick);  // glass thick
  Control.addVariable(keyName+"LayerThick2",voidThick);

  Control.addVariable(keyName+"LayerMat0","Void");
  Control.addVariable(keyName+"LayerMat1",guideMat);
  Control.addVariable(keyName+"LayerMat2","Void");       
  
  Control.addVariable(keyName+"0TypeID","Rectangle");
  Control.addVariable(keyName+"0Height",V);
  Control.addVariable(keyName+"0Width",H);
  Control.copyVar(keyName+"0Length",keyName+"Length");

  return;
}

void
FocusGenerator::generateBender(FuncDataBase& Control,
			       const std::string& keyName,
			       const double length,
			       const double HS,const double HE,
			       const double VS,const double VE,
			       const double radius,const double angleDir) const
  /*!
    Create bender variables
    \param Control :: FuncDatabase to populate
    \param keyName :: principle name
    \param length :: total length
    \param HS :: Horrizontal start
    \param HE :: Horrizontal end
    \param VS :: Vertical start
    \param VE :: Vertical end
    \param radius :: radius of curvature [cm]
    \param angleDir :: angle to rotate primary plane
   */
{
  ELog::RegMethod RegA("FocusGenerator","generateBender");
  
  // Bender in section so use cut system
  Control.addVariable(keyName+"Length",length);
    
  Control.addVariable(keyName+"XStep",0.0);       
  if (!yStepActive)
    Control.addParse<double>(keyName+"YStep","-"+keyName+"Length/2.0");
  else
    Control.addVariable<double>(keyName+"YStep",yStep);
  Control.copyVar(keyName+"BeamY",keyName+"YStep"); 

  Control.addVariable(keyName+"ZStep",0.0);       
  Control.addVariable(keyName+"XYAngle",0.0);
  Control.addVariable(keyName+"ZAngle",0.0);
  Control.addVariable(keyName+"BeamXYAngle",0.0);       


  Control.addVariable(keyName+"NShapes",1);       
  Control.addVariable(keyName+"NShapeLayers",3);
  Control.addVariable(keyName+"ActiveShield",0);

  Control.addVariable(keyName+"LayerThick1",substrateThick);  // glass thick
  Control.addVariable(keyName+"LayerThick2",voidThick);

  Control.addVariable(keyName+"LayerMat0","Void");
  Control.addVariable(keyName+"LayerMat1",guideMat);
  Control.addVariable(keyName+"LayerMat2","Void");       

  Control.addVariable(keyName+"0TypeID","Bend");
  Control.addVariable(keyName+"0AHeight",VS);
  Control.addVariable(keyName+"0BHeight",VE);
  Control.addVariable(keyName+"0AWidth",HS);
  Control.addVariable(keyName+"0BWidth",HE);
  Control.addVariable(keyName+"0Length",length);
  Control.addVariable(keyName+"0AngDir",angleDir);
  Control.addVariable(keyName+"0Radius",radius);
  return;
}
  

  
}  // NAMESPACE setVariable
