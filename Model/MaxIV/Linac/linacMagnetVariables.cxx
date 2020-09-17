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
	"Seg3DipoleB L2SPF3FlatB:Void",

	"Seg4QuadA L2SPF4PipeB:Void",
	"Seg4SexuA L2SPF4PipeB:Void",
	"Seg4QuadB L2SPF4PipeB:Void",

        "Seg5DipoleA L2SPF5FlatA:Void",
	"Seg5DipoleB L2SPF5FlatB:Void",

	"Seg7MagHorA L2SPF7PipeA:Void",
	"Seg7QuadA L2SPF7PipeA:Void",
	"Seg7MagVertA L2SPF7PipeB:Void",

	"Seg9MagVertA L2SPF9FPipeA:Void",
	"Seg9MagHorA L2SPF9FlatB:Void",
	"Seg9QuadA L2SPF9FlatA:Void"

    });

  for(const std::string& Item : MUname)
    Control.pushVariable<std::string>("MagUnitList",Item);

  // SEGMENT 3
  MagnetGenerator MUdipole;
  MUdipole.setSize(65.0,15.0,3.0);
  MUdipole.setField(-1.7,0,0,0);
  MUdipole.generate(Control,"Seg3DipoleA","L2SPF3DipoleA","0",0.0);
  MUdipole.setField(1.7,0,0,0);
  MUdipole.generate(Control,"Seg3DipoleB","L2SPF3DipoleB","0",0.0);

  // SEGMENT 4
  MUdipole.generateQuad(Control,4,"QuadA",0.0,0.65);
  MUdipole.generateSexupole(Control,4,"SexuA",0.0,1.85);
  MUdipole.generateQuad(Control,4,"QuadB",0.0,0.65);
  MUdipole.generateCorMag(Control,4,"CMagHorC",0.0);
  MUdipole.generateCorMag(Control,4,"CMagVertC",0.0);

  // SEGMENT 5
  MUdipole.setSize(65.0,15.0,3.0);
  MUdipole.setField(-1.7,0,0,0);
  MUdipole.generate(Control,"Seg5DipoleA","L2SPF5DipoleA","0",0.0);
  MUdipole.setField(1.7,0,0,0);
  MUdipole.generate(Control,"Seg5DipoleB","L2SPF5DipoleB","0",0.0);

  // SEGMENT 6
  
  // SEGMENT 7
  MUdipole.generateCorMag(Control,7,"CMagHorA",0.0);
  MUdipole.generateQuad(Control,7,"QuadA",0.0,0.65);
  MUdipole.generateCorMag(Control,7,"CMagVertA",0.0);

  
  return;
}

} // NAMESPACE setVariable
