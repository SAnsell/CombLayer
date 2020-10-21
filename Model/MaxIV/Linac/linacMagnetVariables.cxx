/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/linacMagnetVariables.cxx
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

	"Seg7CMagHorA L2SPF7PipeA:Void",
	"Seg7QuadA L2SPF7PipeA:Void",
	"Seg7CMagVertA L2SPF7PipeB:Void",

	"Seg9CMagVertA L2SPF9PipeA:Void",
	"Seg9CMagHorA L2SPF9PipeA:Void",
	"Seg9QuadA L2SPF9PipeB:Void",

	"Seg10QuadA L2SPF10PipeC:Void",
	"Seg10CMagVertA L2SPF10PipeC:Void",

	"Seg11QuadA L2SPF11PipeA:Void",
	"Seg11CMagHorA L2SPF11PipeB:Void",

	"Seg12DipoleA L2SPF12FlatA:Void",
	"Seg12DipoleB L2SPF12PipeLA:Void L2SPF12FlatB:Void",

	"Seg13CMagHorA L2SPF13PipeA:Void",
	"Seg13QuadA L2SPF13PipeB:Void",
	"Seg13SexuA L2SPF13PipeB:Void",
	"Seg13QuadB L2SPF13PipeB:Void",
	"Seg13CMagVerC L2SPF13PipeC:Void",

	"Seg14DM1 TDC14FlatB:Void",
	"Seg14DM2 TDC14FlatB:Void",

	"Seg16Quad TDC16PipeA:Void",
	"Seg16CMagH TDC16PipeB:Void",
	"Seg16CMagV TDC16PipeB:Void",

	"Seg18Quad TDC18PipeA:Void",
	"Seg18CMagH TDC18PipeB:Void",
	"Seg18CMagV TDC18PipeB:Void",

	"Seg21Quad TDC21PipeA:Void",
	"Seg21CMagH TDC21PipeB:Void",
	"Seg21CMagV TDC21PipeB:Void",

	"Seg23Quad TDC23PipeA:Void",
	"Seg23CMagH TDC23PipeB:Void",
	"Seg23CMagV TDC23PipeB:Void",

	"Seg24CMagH TDC24PipeB:Void",
	"Seg24CMagV TDC24PipeB:Void",
	"Seg24Quad TDC24PipeC:Void",

	"Seg25DipoleA TDC25TriPipeA:Void",

	"Seg30CMagV SPF30PipeB:Void",

	"Seg31Quad SPF31PipeA:Void",
	"Seg31CMagH SPF31PipeB:Void",

	"Seg32DMA SPF32FlatA:Void",
	"Seg32DMB SPF32FlatB:Void",

	"Seg33CMagHorA SPF31PipeA:Void",
	"Seg33QuadA SPF33PipeB:Void",
	"Seg33SexuA SPF33PipeB:Void",
	"Seg33QuadB SPF33PipeB:Void",
	"Seg33CMagVerC SPF33PipeC:Void",

	"Seg34DMA SPF34FlatA:Void",
	"Seg34DMB SPF34FlatB:Void",

	"Seg35QuadA SPF35PipeA:Void",
	"Seg35QuadB SPF35PipeB:Void",
	"Seg35CMagH SPF35PipeB:Void",
	"Seg35CMagV SPF35PipeB:Void",

	"Seg36QuadA SPF36PipeA:Void",
	"Seg36CMagH SPF36PipeA:Void",
	"Seg36CMagV SPF35PipeA:Void",
	"Seg36QuadB SPF36PipeA:Void",

	"Seg43CMagH SPF43Pipe:Void"

    });

  for(const std::string& Item : MUname)
    Control.pushVariable<std::string>("MagUnitList",Item);

  // SEGMENT 3
  MagnetGenerator MUdipole;
  MUdipole.setSize(65.0,15.0,3.0);
  MUdipole.setField(-1.7/2.0, 0.0, 0.0, 0.0);
  MUdipole.generate(Control,"Seg3DipoleA","L2SPF3DipoleA","0",0.0);
  MUdipole.setField(1.7/2.0, 0.0, 0.0, 0.0);
  MUdipole.generate(Control,"Seg3DipoleB","L2SPF3DipoleB","0",0.0);

  // SEGMENT 4
  MUdipole.generateQuad(Control,4,"QuadA",0.0,0.0);
  MUdipole.generateSexupole(Control,4,"SexuA",0.0,0.0);
  MUdipole.generateQuad(Control,4,"QuadB",0.0,0.0);
  MUdipole.generateCorMag(Control,4,"CMagHorC",0.0);
  MUdipole.generateCorMag(Control,4,"CMagVertC",0.0);

  // SEGMENT 5
  MUdipole.setSize(65.0,15.0,3.0);
  MUdipole.setField(0.859,0,0,0);
  MUdipole.generate(Control,"Seg5DipoleA","L2SPF5DipoleA","0",-90.0);
  MUdipole.setField(0.859,0,0,0);  // 0.66 : 0.8
  MUdipole.generate(Control,"Seg5DipoleB","L2SPF5DipoleB","0",-90.0);

  // SEGMENT 6

  // // SEGMENT 7
  MUdipole.generateCorMag(Control,7,"CMagHorA",0.0);
  MUdipole.generateQuad(Control,7,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,7,"CMagVertA",0.0);

  // // SEGMENT 9
  MUdipole.generateCorMag(Control,9,"CMagVertA",0.0);
  MUdipole.generateCorMag(Control,9,"CMagHorA",90.0,0.00);
  MUdipole.generateQuad(Control,9,"QuadA",0.0,0.0);

  // SEGMENT 10
  MUdipole.generateQuad(Control,10,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,10,"CMagVertA",90.0,0.0);

  // SEGMENT 11
  MUdipole.generateQuad(Control,11,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,11,"CMagHorA",90.0,0.0);

  // SEGMENT 12
  MUdipole.generateDipole(Control,12,"DipoleA",0.0,0.0);
  MUdipole.generateDipole(Control,12,"DipoleB",0.0,0.0);

  // SEGMENT 13
  MUdipole.generateCorMag(Control,13,"CMagHorA",0.0);
  MUdipole.generateQuad(Control,13,"QuadA",0.0,0.65);
  MUdipole.generateSexupole(Control,13,"SexuA",0.0,0.65);
  MUdipole.generateQuad(Control,13,"QuadB",0.0,0.65);
  MUdipole.generateCorMag(Control,13,"CMagVerC",0.0);

  // SEGMENT 14
  MUdipole.setPreName("TDC");
  MUdipole.generateDipole(Control,14,"DM1",0.0,1.7);
  MUdipole.generateDipole(Control,14,"DM2",0.0,1.7);

  // SEGMENT 15

  // SEGMENT 16
  MUdipole.generateQuad(Control,16,"Quad",0.0,0.65);
  MUdipole.generateCorMag(Control,16,"CMagH",0.0);
  MUdipole.generateCorMag(Control,16,"CMagV",0.0);

  // SEGMENT 17

  // SEGMENT 18
  MUdipole.generateQuad(Control,18,"Quad",0.0,0.65);
  MUdipole.generateCorMag(Control,18,"CMagH",0.0);
  MUdipole.generateCorMag(Control,18,"CMagV",0.0);

  // SEGMENT 19
  // SEGMENT 20

  // SEGMENT 21
  MUdipole.generateQuad(Control,21,"Quad",0.0,0.65);
  MUdipole.generateCorMag(Control,21,"CMagH",0.0);
  MUdipole.generateCorMag(Control,21,"CMagV",0.0);

  // SEGMENT 23
  MUdipole.generateQuad(Control,23,"Quad",0.0,0.65);
  MUdipole.generateCorMag(Control,23,"CMagH",0.0);
  MUdipole.generateCorMag(Control,23,"CMagV",0.0);

  // SEGMENT 24
  MUdipole.generateCorMag(Control,24,"CMagH",0.0);
  MUdipole.generateCorMag(Control,24,"CMagV",0.0);
  MUdipole.generateQuad(Control,24,"Quad",0.0,0.65);

  // SEGMENT 25
  MUdipole.generateDipole(Control,25,"DipoleA",0.0,1.7);

  // SEGMENT 26
  // SEGMENT 27
  // SEGMENT 28
  // SEGMENT 29

  // SEGMENT 30
  MUdipole.setPreName("SPF");
  MUdipole.generateCorMag(Control,30,"CMagV",0.0);

  // SEGMENT 31
  MUdipole.generateQuad(Control,31,"Quad",0.0,0.0);
  MUdipole.generateCorMag(Control,31,"CMagH",0.0);

  // SEGMENT 32
  MUdipole.generateDipole(Control,32,"DMA",90.0,0.860);
  MUdipole.generateDipole(Control,32,"DMB",90.0,0.860);

  // SEGMENT 33
  MUdipole.generateCorMag(Control,33,"CMagHorA",0.0);
  MUdipole.generateQuad(Control,33,"QuadA",0.0,0.0);
  MUdipole.generateSexupole(Control,33,"SexuA",0.0,0.0);
  MUdipole.generateQuad(Control,33,"QuadB",0.0,0.0);
  MUdipole.generateCorMag(Control,33,"CMagVerC",0.0);

  // SEGMENT 34
  MUdipole.generateDipole(Control,34,"DMA",90.0,0.858);
  MUdipole.generateDipole(Control,34,"DMB",90.0,0.858);


  // SEGMENT 35
  MUdipole.generateQuad(Control,35,"QuadA",0.0,0.0);
  MUdipole.generateQuad(Control,35,"QuadB",0.0,0.0);
  MUdipole.generateCorMag(Control,35,"CMagH",0.0);
  MUdipole.generateCorMag(Control,35,"CMagV",0.0);

  // SEGMENT 36
  MUdipole.generateQuad(Control,36,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,36,"CMagH",0.0);
  MUdipole.generateCorMag(Control,36,"CMagV",0.0);
  MUdipole.generateQuad(Control,36,"QuadB",0.0,0.0);

  // SEGMENT 37/38/39/40/41/42


  // SEGMENT 43
  MUdipole.generateCorMag(Control,43,"CMagH",0.0);

  // SEGMENT 44 [THIS IS a curved dipole -- check sized]
  MUdipole.generateDipole(Control,44,"CMag",0.0,1.7);

  // SEGMENT 45
  // SEGMENT 46
  // SEGMENT 47
  // SEGMENT 48
  // SEGMENT 49


  return;
}

} // NAMESPACE setVariable
