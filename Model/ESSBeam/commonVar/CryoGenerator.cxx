/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/CryoGenerator.cxx
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
#include "CryoGenerator.h"

namespace setVariable
{

CryoGenerator::CryoGenerator() 
  /*!
    Constructor : All variables are set for 35cm radius disks
    with an overlap of 5cm. Values are scaled appropiately for
    most changes
  */
{}


CryoGenerator::~CryoGenerator() 
 /*!
   Destructor
 */
{}

  
void
CryoGenerator::generateFridge(FuncDataBase& Control,
                              const std::string& keyName,
                              const double yStep,
                              const double zStep,
			      const double sampleZ)
  /*!
    Generate the chopper variables
    \param Control :: Functional data base
    \param keyName :: Base name for chopper variables
    \param yStep :: main y-step
    \param zStep :: Main +/- of z position
    \param sampleZ :: Sample stick +/- Z positon
   */
{
  ELog::RegMethod RegA("CryoGenerator","generateCryostate");

  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",zStep);


  Control.addVariable(keyName+"SampleZOffset",sampleZ);

  Control.addVariable(keyName+"SampleRadius",0.75);
  Control.addVariable(keyName+"SampleHeight",2.0);
  
  Control.addVariable(keyName+"VoidRadius",5.0);
  Control.addVariable(keyName+"VoidHeight",8.0);
  Control.addVariable(keyName+"VoidDepth",10.0);
  Control.addVariable(keyName+"VoidWallThick",0.5);

  Control.addVariable(keyName+"StickLen",105.0);
  Control.addVariable(keyName+"StickRadius",0.6);

  Control.addVariable(keyName+"StickBoreRadius",1.8);
  Control.addVariable(keyName+"StickBoreHeight",95.0);
  Control.addVariable(keyName+"StickBoreThick",0.5);

  Control.addVariable(keyName+"HeatRadius",8.0);
  Control.addVariable(keyName+"HeatHeight",13.0);
  Control.addVariable(keyName+"HeatDepth",12.0);
  Control.addVariable(keyName+"HeatThick",0.5);

  Control.addVariable(keyName+"TailRadius",11.0);
  Control.addVariable(keyName+"TailHeight",9.0);
  Control.addVariable(keyName+"TailDepth",14.5);
  Control.addVariable(keyName+"TailThick",0.5);
  Control.addVariable(keyName+"MainThick",1.0);
  Control.addVariable(keyName+"RoofThick",1.5);

  Control.addVariable(keyName+"HeatOuterRadius",13.5);
  Control.addVariable(keyName+"HeatOuterLift",17.0);

  Control.addVariable(keyName+"LiqN2InnerRadius",15.6);
  Control.addVariable(keyName+"LiqN2OuterRadius",21.2);
  Control.addVariable(keyName+"LiqN2WallThick",0.5);
  Control.addVariable(keyName+"LiqN2Height",53.0);
  
  Control.addVariable(keyName+"LiqHeInnerRadius",5.7);
  Control.addVariable(keyName+"LiqHeOuterRadius",12.85);
  Control.addVariable(keyName+"LiqHeWallThick",0.5);
  Control.addVariable(keyName+"LiqHeHeight",51.0);
  Control.addVariable(keyName+"LiqHeExtraLift",2.0);

  Control.addVariable(keyName+"TailOuterRadius",22.80);
  Control.addVariable(keyName+"TailOuterHeight",89.0);
  Control.addVariable(keyName+"TailOuterLift",14.0);

  Control.addVariable(keyName+"SampleMat","H2O");
  Control.addVariable(keyName+"WallMat","Aluminium");
  Control.addVariable(keyName+"StickMat","Stainless304");
  Control.addVariable(keyName+"LiqN2Mat","LiqN2");
  Control.addVariable(keyName+"LiqHeMat","LiqHelium");

  return;
}


  
}  // NAMESPACE setVariable
