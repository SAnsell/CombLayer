/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/linacVariables.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell/Konstantin Batkov
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
LINACmagnetVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("linacMangnetVariables[F]","LINACmagnetVariables");

  // active units : Void space for field
  const std::vector<std::string> MUname
    ({
        "Seg3DipoleA L2SPF3FlatA:Void",
	"Seg3DipoleB L2SPF3FlatB:Void"
     });

  for(const std::string& Item : MUname)
    Control.pushVariable<std::string>("MagUnitList",Item);


  MagnetGenerator MUdipole;
  MUdipole.setField(-1.7,0,0,0);
  MUdipole.generate(Control,"Seg3DipoleA","L2SPF3DipoleA","0",0.0);
  MUdipole.setField(1.7,0,0,0);
  MUdipole.generate(Control,"Seg3DipoleB","L2SPF3DipoleB","0",0.0);
  
  return;
}

} // NAMESPACE setVariable
