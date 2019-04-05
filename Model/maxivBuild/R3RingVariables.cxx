/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R3RingVariables.cxx
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

void
R3RingVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    for the R3 concrete shielding walls
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3RingVariables");

  const std::string preName("R3Ring");
  
  Control.addVariable(preName+"FullOuterRadius",7000.0);
  Control.addVariable(preName+"HexRadius",8030.0);       // U
  Control.addVariable(preName+"HexWallThick",60.0);
  Control.addVariable(preName+"Height",160.0);
  Control.addVariable(preName+"Depth",130.0);
  Control.addVariable(preName+"RoofThick",60.0);
  Control.addVariable(preName+"RoofExtraVoid",40.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"WallMat","Concrete");  
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");

  // construct points and inner
  Control.addVariable(preName+"NPoints",22);

  Control.addVariable(preName+"VPoint0",Geometry::Vec3D(7.94,1916.24,0.0));
  Control.addVariable(preName+"VPoint1",Geometry::Vec3D(158.46,1655.54,0.0));
  Control.addVariable(preName+"VPoint2",Geometry::Vec3D(443.6,1655.54,0.0));
  Control.addVariable(preName+"VPoint3",Geometry::Vec3D(1663.48,951.24,0.0));
  Control.addVariable(preName+"VPoint4",Geometry::Vec3D(1512.97,690.54,0.0));
  Control.addVariable(preName+"VPoint5",Geometry::Vec3D(1916.24,-7.94,0.0));
  Control.addVariable(preName+"VPoint6",Geometry::Vec3D(1655.54,-158.46,0.0));
  Control.addVariable(preName+"VPoint7",Geometry::Vec3D(1655.54,-965.0,0.0));
  Control.addVariable(preName+"VPoint8",Geometry::Vec3D(1354.51,-965.0,0.0));
  Control.addVariable(preName+"VPoint9",Geometry::Vec3D(1211.94,-1211.94,0.0));
  Control.addVariable(preName+"VPoint10",Geometry::Vec3D(-7.94,-1916.24,0.0));
  Control.addVariable(preName+"VPoint11",Geometry::Vec3D(-158.46,-1655.54,0.0));
  Control.addVariable(preName+"VPoint12",Geometry::Vec3D(-965.0,-1655.54,0.0));
  Control.addVariable(preName+"VPoint13",Geometry::Vec3D(-965.0,-1354.51,0.0));
  Control.addVariable(preName+"VPoint14",Geometry::Vec3D(-1663.48,-951.24,0.0));
  Control.addVariable(preName+"VPoint15",Geometry::Vec3D(-1512.97,-690.54,0.0));
  Control.addVariable(preName+"VPoint16",Geometry::Vec3D(-1916.24,7.94,0.0));
  Control.addVariable(preName+"VPoint17",Geometry::Vec3D(-1655.54,158.46,0.0));
  Control.addVariable(preName+"VPoint18",Geometry::Vec3D(-1655.54,965.0,0.0));
  Control.addVariable(preName+"VPoint19",Geometry::Vec3D(-1354.51,965.0,0.0));
  Control.addVariable(preName+"VPoint20",Geometry::Vec3D(-951.24,1663.48,0.0));
  Control.addVariable(preName+"VPoint21",Geometry::Vec3D(-690.54,1512.97,0.0));


  Control.addVariable(preName+"OPoint0",Geometry::Vec3D(103.83,  2110.16,0.0));
  Control.addVariable(preName+"OPoint1",Geometry::Vec3D(297.02,  1775.54,0.0));
  Control.addVariable(preName+"OPoint2",Geometry::Vec3D(475.75,  1775.54,0.0));
  Control.addVariable(preName+"OPoint3",Geometry::Vec3D(1878.37,  965.16,0.0));
  Control.addVariable(preName+"OPoint4",Geometry::Vec3D(1663.08,  590.54,0.0));
  Control.addVariable(preName+"OPoint5",Geometry::Vec3D(2055.52,  -89.19,0.0));
  Control.addVariable(preName+"OPoint6",Geometry::Vec3D(1735.54, -273.93,0.0));
  Control.addVariable(preName+"OPoint7",Geometry::Vec3D(1735.54, -1105.0,0.0));
  Control.addVariable(preName+"OPoint8",Geometry::Vec3D(1366.06, -1105.0,0.0));
  Control.addVariable(preName+"OPoint9",Geometry::Vec3D(1270.05, -1270.50,0.0));
  Control.addVariable(preName+"OPoint10",Geometry::Vec3D(-89.19,  -2055.52,0.0));
  Control.addVariable(preName+"OPoint11",Geometry::Vec3D(-273.93, -1735.54,0.0));
  Control.addVariable(preName+"OPoint12",Geometry::Vec3D(-1105.0, -1735.54,0.0));
  Control.addVariable(preName+"OPoint13",Geometry::Vec3D(-1105.0, -1366.06,0.0));
  Control.addVariable(preName+"OPoint14",Geometry::Vec3D(-1824.73, -950.52,0.0));
  Control.addVariable(preName+"OPoint15",Geometry::Vec3D(-1639.99, -630.54,0.0));
  Control.addVariable(preName+"OPoint16",Geometry::Vec3D(-2055.52,   89.19,0.0));
  Control.addVariable(preName+"OPoint17",Geometry::Vec3D(-1735.54,  273.93,0.0));
  Control.addVariable(preName+"OPoint18",Geometry::Vec3D(-1735.54, 1105.0,0.0));
  Control.addVariable(preName+"OPoint19",Geometry::Vec3D(-1366.06, 1105.0,0.0));
  Control.addVariable(preName+"OPoint20",Geometry::Vec3D(-950.52,  1824.73,0.0));
  Control.addVariable(preName+"OPoint21",Geometry::Vec3D(-670.54,  1663.08,0.0));


  return;
}

}  // NAMESPACE setVariable
