/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/LOKIvariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

void
LOKIvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for loki
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("LOKIvariables[F]","LOKIvariables");

  // Bender in section so use cut system
  Control.addVariable("G1BLine2Filled",1);
  
  Control.addVariable("lokiBAXStep",0.0);       
  Control.addVariable("lokiBAYStep",0.0);       
  Control.addVariable("lokiBAZStep",0.0);       
  Control.addVariable("lokiBAXYAngle",0.0);
  Control.addVariable("lokiBAZAngle",0.0);
  Control.addVariable("lokiBABeamXYAngle",0.0);       

  Control.addVariable("lokiBALength",400.0);       
  Control.addVariable("lokiBANShapes",1);       
  Control.addVariable("lokiBANShapeLayers",3);
  Control.addVariable("lokiBAActiveShield",0);

  Control.addVariable("lokiBALayerThick1",0.4);  // glass thick
  Control.addVariable("lokiBALayerThick2",1.5);

  Control.addVariable("lokiBALayerMat0","Void");
  Control.addVariable("lokiBALayerMat1","Glass");
  Control.addVariable("lokiBALayerMat2","Void");       

  Control.addVariable("lokiBA0TypeID","Bend");
  Control.addVariable("lokiBA0AHeight",3.0);
  Control.addVariable("lokiBA0BHeight",3.0);
  Control.addVariable("lokiBA0AWidth",3.0);
  Control.addVariable("lokiBA0BWidth",3.0);
  Control.addVariable("lokiBA0Length",400.0);
  Control.addVariable("lokiBA0AngDir",180.0);
  Control.addVariable("lokiBA0Radius",6600.0);



  // GUIDE EXITING BEND [Straight A]
  Control.addVariable("lokiGAXStep",0.0);       
  Control.addVariable("lokiGAYStep",0.0);       
  Control.addVariable("lokiGAZStep",0.0);       
  Control.addVariable("lokiGAXYAngle",0.0);       
  Control.addVariable("lokiGAZAngle",0.0);       
  Control.addVariable("lokiGALength",50.0);       
  
  Control.addVariable("lokiGANShapes",1);       
  Control.addVariable("lokiGANShapeLayers",3);
  Control.addVariable("lokiGAActiveShield",0);

  Control.addVariable("lokiGALayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGALayerThick2",1.5);

  Control.addVariable("lokiGALayerMat0","Void");
  Control.addVariable("lokiGALayerMat1","Glass");
  Control.addVariable("lokiGALayerMat2","Void");       

  Control.addVariable("lokiGA0TypeID","Rectangle");
  Control.addVariable("lokiGA0Height",3.0);
  Control.addVariable("lokiGA0Width",3.0);
  Control.addVariable("lokiGA0Length",50.0);
  Control.addVariable("lokiGA0ZAngle",0.0);


  // Double Blade chopper
  Control.addVariable("lokiDBladeXStep",0.0);
  Control.addVariable("lokiDBladeYStep",2.0);
  Control.addVariable("lokiDBladeZStep",0.0);
  Control.addVariable("lokiDBladeXYangle",0.0);
  Control.addVariable("lokiDBladeZangle",0.0);

  Control.addVariable("lokiDBladeGap",3.0);
  Control.addVariable("lokiDBladeInnerRadius",10.0);
  Control.addVariable("lokiDBladeOuterRadius",22.50);
  Control.addVariable("lokiDBladeNDisk",2);

  Control.addVariable("lokiDBlade0Thick",1.0);
  Control.addVariable("lokiDBlade1Thick",1.0);
  Control.addVariable("lokiDBladeInnerMat","Inconnel");
  Control.addVariable("lokiDBladeOuterMat","Aluminium");
  
  Control.addVariable("lokiDBladeNBlades",2);
  Control.addVariable("lokiDBlade0PhaseAngle0",95.0);
  Control.addVariable("lokiDBlade0OpenAngle0",30.0);
  Control.addVariable("lokiDBlade1PhaseAngle0",95.0);
  Control.addVariable("lokiDBlade1OpenAngle0",30.0);

  Control.addVariable("lokiDBlade0PhaseAngle1",275.0);
  Control.addVariable("lokiDBlade0OpenAngle1",30.0);
  Control.addVariable("lokiDBlade1PhaseAngle1",275.0);
  Control.addVariable("lokiDBlade1OpenAngle1",30.0);

  // GUIDE BETWEEN BENDERS
  Control.addVariable("lokiGInnerXStep",0.0);       
  Control.addVariable("lokiGInnerYStep",2.0);       
  Control.addVariable("lokiGInnerZStep",0.0);       
  Control.addVariable("lokiGInnerXYAngle",0.0);       
  Control.addVariable("lokiGInnerZAngle",0.0);       
  Control.addVariable("lokiGInnerLength",40.0);       

  Control.addVariable("lokiGInnerBeamYStep",4.0);
  
  Control.addVariable("lokiGInnerNShapes",1);       
  Control.addVariable("lokiGInnerNShapeLayers",3);
  Control.addVariable("lokiGInnerActiveShield",0);

  Control.addVariable("lokiGInnerLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGInnerLayerThick2",1.5);

  Control.addVariable("lokiGInnerLayerMat0","Void");
  Control.addVariable("lokiGInnerLayerMat1","Glass");
  Control.addVariable("lokiGInnerLayerMat2","Void");       

  Control.addVariable("lokiGInner0TypeID","Rectangle");
  Control.addVariable("lokiGInner0Height",3.0);
  Control.addVariable("lokiGInner0Width",3.0);
  Control.addVariable("lokiGInner0Length",40.0);
  Control.addVariable("lokiGInner0ZAngle",0.0);

  // Single Blade chopper
  Control.addVariable("lokiSBladeXStep",0.0);
  Control.addVariable("lokiSBladeYStep",2.0);
  Control.addVariable("lokiSBladeZStep",0.0);
  Control.addVariable("lokiSBladeXYangle",0.0);
  Control.addVariable("lokiSBladeZangle",0.0);

  Control.addVariable("lokiSBladeInnerRadius",10.0);
  Control.addVariable("lokiSBladeOuterRadius",22.50);
  Control.addVariable("lokiSBladeNDisk",1);

  Control.addVariable("lokiSBlade0Thick",1.0);
  Control.addVariable("lokiSBladeInnerMat","Inconnel");
  Control.addVariable("lokiSBladeOuterMat","B4C");
  
  Control.addVariable("lokiSBladeNBlades",2);
  Control.addVariable("lokiSBlade0PhaseAngle0",95.0);
  Control.addVariable("lokiSBlade0OpenAngle0",30.0);

  Control.addVariable("lokiSBlade0PhaseAngle1",275.0);
  Control.addVariable("lokiSBlade0OpenAngle1",30.0);

  // GUIDE EXITING Single Chopper [Straight A]
  Control.addVariable("lokiGBXStep",0.0);       
  Control.addVariable("lokiGBYStep",2.0);
  Control.addVariable("lokiGBBeamYStep",3.0);
  Control.addVariable("lokiGBZStep",0.0);       
  Control.addVariable("lokiGBXYAngle",0.0);       
  Control.addVariable("lokiGBZAngle",0.0);       
  Control.addVariable("lokiGBLength",45.0);       
  
  Control.addVariable("lokiGBNShapes",1);       
  Control.addVariable("lokiGBNShapeLayers",3);
  Control.addVariable("lokiGBActiveShield",0);

  Control.addVariable("lokiGBLayerThick1",0.4);  // glass thick
  Control.addVariable("lokiGBLayerThick2",1.5);

  Control.addVariable("lokiGBLayerMat0","Void");
  Control.addVariable("lokiGBLayerMat1","Glass");
  Control.addVariable("lokiGBLayerMat2","Void");       

  Control.addVariable("lokiGB0TypeID","Rectangle");
  Control.addVariable("lokiGB0Height",3.0);
  Control.addVariable("lokiGB0Width",3.0);
  Control.addVariable("lokiGB0Length",45.0);
  Control.addVariable("lokiGB0ZAngle",0.0);


  
  return;
}

}  // NAMESPACE setVariable
