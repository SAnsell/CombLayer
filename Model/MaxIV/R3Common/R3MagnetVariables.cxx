/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3Common/R3RingMagnetVariables.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MagnetGenerator.h"

#include "maxivVariables.h"


namespace setVariable
{

void
R3MagnetVariables(FuncDataBase& Control,
		  const std::string& beam)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
    \param magField :: Field for the magnet
  */
{
  ELog::RegMethod RegA("R3MagnetVariables[F]","R3MagnetVariables");


  MagnetGenerator MUdipole;
  
  // First set MagUnitList  (areas for magnetic field):
  const std::string magName=beam+"MagnetM1";
  const std::string blockName=beam+"FrontBeamM1Block";


  Control.pushVariable<std::string>
    ("MagUnitList",magName+" "+blockName+"HalfElectron:Void");

  // len/widht/height
  MUdipole.setSize(60.0,20.0,2.5);
  MUdipole.generateR3Dipole(Control,magName,blockName+"DIPm",-90.0,-0.48);

  return;
}

} // NAMESPACE setVariable
