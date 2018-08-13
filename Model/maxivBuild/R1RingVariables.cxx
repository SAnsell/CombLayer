/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R1RingVariables.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"
#include "maxivVariables.h"

#include "CFFlanges.h"

namespace setVariable
{

namespace R1RingVar
{
 
}  // NAMESPACE R1RingVar
  
void
R1RingVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","R1RingVariables");

  const std::string preName("R1Ring");
  
  Control.addVariable(preName+"FullOuterRadius",7000.0);
  Control.addVariable(preName+"HexRadius",800.0);
  Control.addVariable(preName+"HexWallThick",60.0);
  Control.addVariable(preName+"Height",160.0);
  Control.addVariable(preName+"Depth",180.0);
  Control.addVariable(preName+"RoofThick",60.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"WallMat","Concrete");  
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");

  // construct points and inner
  Control.addVariable(preName+"NPoints",22);
  Control.addVariable(preName+"VPoint0",Geometry::Vec3D(-3.243,1867.968,0.0));
  Control.addVariable(preName+"VPoint1",Geometry::Vec3D(152.421,1605.285,0.0));
  Control.addVariable(preName+"VPoint2",Geometry::Vec3D(444.291,1605.285,0.0));
  Control.addVariable(preName+"VPoint3",Geometry::Vec3D(1640.958,924.255,0.0));
  Control.addVariable(preName+"VPoint4",Geometry::Vec3D(1475.565,671.301,0.0));
  Control.addVariable(preName+"VPoint5",Geometry::Vec3D(1864.725,-29.187,0.0));
  Control.addVariable(preName+"VPoint6",Geometry::Vec3D(1611.771,-175.122,0.0));
  Control.addVariable(preName+"VPoint7",Geometry::Vec3D(1611.771,-972.9,0.0));
  Control.addVariable(preName+"VPoint8",Geometry::Vec3D(1290.714,-972.9,0.0));
  Control.addVariable(preName+"VPoint9",Geometry::Vec3D(1183.695,-1196.667,0.0));
  Control.addVariable(preName+"VPoint10",Geometry::Vec3D(-22.701,-1910.127,0.0));
  Control.addVariable(preName+"VPoint11",Geometry::Vec3D(-149.178,-1634.472,0.0));
  Control.addVariable(preName+"VPoint12",Geometry::Vec3D(-985.872,-1634.472,0.0));
  Control.addVariable(preName+"VPoint13",Geometry::Vec3D(-956.685,-1287.471,0.0));
  Control.addVariable(preName+"VPoint14",Geometry::Vec3D(-1637.715,-953.442,0.0));
  Control.addVariable(preName+"VPoint15",Geometry::Vec3D(-1371.789,-570.768,0.0));
  Control.addVariable(preName+"VPoint16",Geometry::Vec3D(-1890.669,-9.729,0.0));
  Control.addVariable(preName+"VPoint17",Geometry::Vec3D(-1647.444,340.515,0.0));
  Control.addVariable(preName+"VPoint18",Geometry::Vec3D(-1657.173,943.713,0.0));
  Control.addVariable(preName+"VPoint19",Geometry::Vec3D(-1306.929,953.442,0.0));
  Control.addVariable(preName+"VPoint20",Geometry::Vec3D(-937.227,1615.014,0.0));
  Control.addVariable(preName+"VPoint21",Geometry::Vec3D(-664.815,1491.78,0.0));

  Control.addVariable(preName+"OPoint0",Geometry::Vec3D(-3.40515,1961.3664,0.0));
  Control.addVariable(preName+"OPoint1",Geometry::Vec3D(160.04205,1685.54925,0.0));
  Control.addVariable(preName+"OPoint2",Geometry::Vec3D(466.50555,1685.54925,0.0));
  Control.addVariable(preName+"OPoint3",Geometry::Vec3D(1723.0059,970.46775,0.0));
  Control.addVariable(preName+"OPoint4",Geometry::Vec3D(1549.34325,704.86605,0.0));
  Control.addVariable(preName+"OPoint5",Geometry::Vec3D(1957.96125,-30.64635,0.0));
  Control.addVariable(preName+"OPoint6",Geometry::Vec3D(1692.35955,-183.8781,0.0));
  Control.addVariable(preName+"OPoint7",Geometry::Vec3D(1692.35955,-1021.545,0.0));
  Control.addVariable(preName+"OPoint8",Geometry::Vec3D(1355.2497,-1021.545,0.0));
  Control.addVariable(preName+"OPoint9",Geometry::Vec3D(1242.87975,-1256.50035,0.0));
  Control.addVariable(preName+"OPoint10",Geometry::Vec3D(-23.83605,-2005.63335,0.0));
  Control.addVariable(preName+"OPoint11",Geometry::Vec3D(-156.6369,-1716.1956,0.0));
  Control.addVariable(preName+"OPoint12",Geometry::Vec3D(-1035.1656,-1716.1956,0.0));
  Control.addVariable(preName+"OPoint13",Geometry::Vec3D(-1004.51925,-1351.84455,0.0));
  Control.addVariable(preName+"OPoint14",Geometry::Vec3D(-1719.60075,-1001.1141,0.0));
  Control.addVariable(preName+"OPoint15",Geometry::Vec3D(-1440.37845,-599.3064,0.0));
  Control.addVariable(preName+"OPoint16",Geometry::Vec3D(-1985.20245,-10.21545,0.0));
  Control.addVariable(preName+"OPoint17",Geometry::Vec3D(-1729.8162,357.54075,0.0));
  Control.addVariable(preName+"OPoint18",Geometry::Vec3D(-1740.03165,990.89865,0.0));
  Control.addVariable(preName+"OPoint19",Geometry::Vec3D(-1372.27545,1001.1141,0.0));
  Control.addVariable(preName+"OPoint20",Geometry::Vec3D(-984.08835,1695.7647,0.0));
  Control.addVariable(preName+"OPoint21",Geometry::Vec3D(-698.05575,1566.369,0.0));


  return;
}

}  // NAMESPACE setVariable
