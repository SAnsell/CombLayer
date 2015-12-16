/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/estia/ESTIAvariables.cxx
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
ESTIAvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for estia
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("ESTIAvariables[F]","ESTIAvariables");
  
  Control.addVariable("estiaAxisXStep",0.0);
  Control.addVariable("estiaAxisYStep",0.0);
  Control.addVariable("estiaAxisZStep",0.0);
  Control.addVariable("estiaAxisXYAngle",0.0);   // rotation 
  Control.addVariable("estiaAxisZAngle",1.0);


  Control.addVariable("estiaFMonoLength",350.0);       
  Control.addVariable("estiaFMonoNShapes",1);       
  Control.addVariable("estiaFMonoNShapeLayers",3);
  Control.addVariable("estiaFMonoActiveShield",0);

  Control.addVariable("estiaFMonoLayerThick1",0.4);  // glass thick
  Control.addVariable("estiaFMonoLayerThick2",1.5);

  Control.addVariable("estiaFMonoLayerMat0","Void");
  Control.addVariable("estiaFMonoLayerMat1","Glass");
  Control.addVariable("estiaFMonoLayerMat2","Void");       

  Control.addVariable("estiaFMono0TypeID","Tapper");
  Control.addVariable("estiaFMono0HeightStart",2.0); // guess
  Control.addVariable("estiaFMono0HeightEnd",5.2);
  Control.addVariable("estiaFMono0WidthStart",8.0); // NOT Centred
  Control.addVariable("estiaFMono0WidthEnd",10.5);
  Control.addVariable("estiaFMono0Length",350.0);

  // VACUUM PIPES for Shutter:
  Control.addVariable("estiaPipeAYStep",2.0);
  Control.addVariable("estiaPipeARadius",9.0);
  Control.addVariable("estiaPipeALength",46.0);
  Control.addVariable("estiaPipeAFeThick",1.0);
  Control.addVariable("estiaPipeAFlangeRadius",12.0);
  Control.addVariable("estiaPipeAFlangeLength",1.0);
  Control.addVariable("estiaPipeAFeMat","Aluminium");
  Control.addVariable("estiaPipeAVoidMat","Void");

  // Shutter section
  Control.addVariable("estiaFAYStep",3.0);       
  Control.addVariable("estiaFALength",45.0);       
  Control.addVariable("estiaFANShapes",1);       
  Control.addVariable("estiaFANShapeLayers",3);
  Control.addVariable("estiaFAActiveShield",0);

  Control.addVariable("estiaFALayerThick1",0.4);  // glass thick
  Control.addVariable("estiaFALayerThick2",1.5);

  Control.addVariable("estiaFALayerMat0","Void");
  Control.addVariable("estiaFALayerMat1","Glass");
  Control.addVariable("estiaFALayerMat2","Void");       

  Control.addVariable("estiaFA0TypeID","Tapper");
  Control.addVariable("estiaFA0HeightStart",4.8); // guess
  Control.addVariable("estiaFA0HeightEnd",5.2);
  Control.addVariable("estiaFA0WidthStart",10.15); // NOT Centred
  Control.addVariable("estiaFA0WidthEnd",10.5);
  Control.addVariable("estiaFA0Length",47.0);

  // VACUUM PIPES for Light Shutter:
  Control.addVariable("estiaPipeBYStep",2.0);
  Control.addVariable("estiaPipeBRadius",9.0);
  Control.addVariable("estiaPipeBLength",450.0);
  Control.addVariable("estiaPipeBFeThick",1.0);
  Control.addVariable("estiaPipeBFlangeRadius",12.0);
  Control.addVariable("estiaPipeBFlangeLength",1.0);
  Control.addVariable("estiaPipeBFeMat","Aluminium");
  Control.addVariable("estiaPipeBVoidMat","Void");

  
  return;
}

}  // NAMESPACE setVariable
