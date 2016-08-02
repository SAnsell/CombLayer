/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/dream/DREAMvariables.cxx
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
#include "essVariables.h"

namespace setVariable
{

void
shortDREAMvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("SHORTDREAMvariables[F]","SHORTDREAMvariables");


  Control.addVariable("shortDreamFDLength",150.0);       
  Control.addVariable("shortDreamPipeDLength",40.0);
  Control.addVariable("shortDreamFELength",140.0);      //
  Control.addParse<double>("shortDreamFD0Length","shortDreamFDLength");
  Control.addParse<double>("shortDreamFE0Length","shortDreamFELength");
  Control.addVariable("shortDreamVacCYStep",153.0);  // -60
  Control.addVariable("shortDreamVacDYStep",165.0);
  
  return;
}

void
shortDREAM2variables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("SHORTDREAMvariables[F]","SHORTDREAM2variables");

  Control.addVariable("shortDream2FDLength",150.0);       
  Control.addVariable("shortDream2PipeDLength",40.0);
  Control.addVariable("shortDream2FELength",140.0);      //
  Control.addParse<double>("shortDream2FD0Length","shortDream2FDLength");
  Control.addParse<double>("shortDream2FE0Length","shortDream2FELength");
  Control.addVariable("shortDream2VacCYStep",153.0);  // -60
  Control.addVariable("shortDream2VacDYStep",165.0);
  
  return;
}
 
}  // NAMESPACE setVariable
