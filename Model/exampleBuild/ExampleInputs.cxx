/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ExampleInputs.cxx
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
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>
#include <iterator>
#include <memory>


#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "MainInputs.h"

namespace mainSystem
{

void
createExampleInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for the ESS
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createESSInputs");
  createInputs(IParam);
  IParam.regMulti("Model","Model",1000,0);
  IParam.setValue("Model",std::string("None"));  

  return;
}

} // NAMESPACE Main Inputs
