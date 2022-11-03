/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/estia/ESTIAvariables.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "PipeGenerator.h"

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

  setVariable::FocusGenerator FGen;
  setVariable::PipeGenerator PipeGen;

  // Shutter guide section
  FGen.setLayer(1,0.4,"Glass");
  FGen.setLayer(2,1.5,"Void");  
  
  Control.addVariable("estiaAxisXStep",0.0);
  Control.addVariable("estiaAxisYStep",0.0);
  Control.addVariable("estiaAxisZStep",0.0);
  Control.addVariable("estiaAxisZAngle",0.0);   // rotation 
  Control.addVariable("estiaAxisXAngle",-1.0);

  FGen.setYOffset(0.2);
  FGen.generateTaper(Control,"estiaFMono",350.0, 2.0,5.2, 8.0,10.50);

  PipeGen.setPipe(9.0,0.5);      // 8cm radius / 0.5cm wall
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();
  PipeGen.setFlange(-4.0,1.0);

  // VACUUM PIPES for Shutter:
  PipeGen.generatePipe(Control,"estiaPipeA",46.0);
  Control.addVariable("estiaPipeAYStep",6.5);
    
  FGen.setYOffset(7.0);
  FGen.generateTaper(Control,"estiaFA",45.0, 4.8,5.2, 10.15,10.50);

  PipeGen.generatePipe(Control,"estiaPipeB",450.0);
  Control.addVariable("estiaPipeBYStep",2.0);

  FGen.setYOffset(3.0);
  FGen.generateTaper(Control,"estiaFB",448.0, 4.8,5.2, 10.15,10.50);
  

  
  return;
}

}  // NAMESPACE setVariable
