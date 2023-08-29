/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/variables.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include <cassert>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "DuctGenerator.h"


namespace setVariable
{
  void BuildingBVariables(FuncDataBase& Control, const std::string& name)
  {
    // References:
    // [0] Bjorn's drawings (have higher priority than other refs)
    // http://localhost:8080/maxiv/work-log/100hz/gtf/guntestfacility-bjornnilsson.djvu/view
    // [1] 221413_radiation_caclulation.STEP
    // [2] K_20-1_08C6b4
    // [3] K_20-1_08C6a3

    constexpr double length = 650; // [1]
    constexpr double length2 = length/2.0;
    constexpr double gunRoomWidth = 460.0; // [1]
    constexpr double depth  = 130; // [1]
    constexpr double backWallThick = 100.0;  // [1]
    constexpr double outerWallThick = 40.0; // [1] [2]

    Control.addVariable(name+"GunRoomLength",length);
    Control.addVariable(name+"GunRoomWidth",gunRoomWidth);
    Control.addVariable(name+"KlystronRoomWidth",460.0); // [1]
    Control.addVariable(name+"InternalWallThick",20.0); // [1]
    Control.addVariable(name+"Depth",depth); // [1]
    Control.addVariable(name+"Height",230.0); // [1]
    Control.addVariable(name+"BackWallThick",backWallThick);
    Control.addVariable(name+"GunRoomEntranceWidth",160.0); // [1] [2]
    Control.addVariable(name+"MidWallThick",100.0); // [1]
    Control.addVariable(name+"OuterWallThick",outerWallThick);
    Control.addVariable(name+"WallMat","Concrete"); // guess
    Control.addVariable(name+"MazeWidth",100.0); // [1] [2]
    Control.addVariable(name+"MazeEntranceOffset",4.5); // [0], page 6, but [2]: 4250-3800-400=5 cm
    Control.addVariable(name+"MazeEntranceWidth",100.0); // [0]: 100, but [2]: 101 (outer)
    Control.addVariable(name+"MazeEntranceHeight",210.0); // [0], page 6
    Control.addVariable(name+"HallLength",500.0); // [2] 10900-5700-200 = 500 cm
    Control.addVariable(name+"FloorThick",40.0); // [0]
    Control.addVariable(name+"Roof1Thick",100.0); // [0]
    Control.addVariable(name+"TRSPRoomWidth",415.0); // [2] and [3]
    Control.addVariable(name+"StairRoomWidth",550.0); // [3]: 1450+4050 = 550 cm
    Control.addVariable(name+"StairRoomLength",440.0); // [3]: 1600 + 2800 = 440 cm
    Control.addVariable(name+"ElevatorWidth",250.0); // [3]: 1450+1050 = 250 cm
    Control.addVariable(name+"ElevatorLength",180.0); // [3]: 180 cm

    setVariable::DuctGenerator DuctGen;
    DuctGen.setSize(10.0); // [0], page 4 (diameter 20.0)
    DuctGen.generate(Control,name+"Duct1",90,0,length2-136.5,260-depth); //[0], pages 2, 4

    DuctGen.setSize(5.0); // [0], page 4
    DuctGen.generate(Control,name+"Duct2",90,0,length2-161.5,265.5-depth); //[0], pages 2, 4
    DuctGen.generate(Control,name+"Duct3",90,0,length2-191.5,265.5-depth); //[0], pages 2, 4

    DuctGen.setSize(7.5); // [0], page 4 (diameter 15.0)
    DuctGen.generate(Control,name+"Duct4",90,0,length2-189.0,11.5-depth); //[0], pages 2, 4
    DuctGen.generate(Control,name+"Duct5",90,0,length2-219.0,11.5-depth); //[0], pages 2, 4

    DuctGen.setSize(10.0); // [0], page 4
    DuctGen.generate(Control,name+"DuctVentillation",90,
		     0.0,length2+backWallThick+77.0,224.5-depth); //[0], pages 2, 4

    DuctGen.setSize(7.5); // [0], page 3
    DuctGen.generate(Control,name+"DuctLaser",0,
		     gunRoomWidth/2.0+outerWallThick-55.0,0.0,263-depth); //[0], pages 2, 3
  }

  void GunTestFacilityVariables(FuncDataBase& Control)
  /*!
    Set Gun Test Facility Building variables
    \param Control :: Database to use
    \param name :: name prefix
  */
  {
    ELog::RegMethod RegA("", "GunTestFacilityVariables");

    const std::string name = "BldB";
    BuildingBVariables(Control, name);

  }
}
