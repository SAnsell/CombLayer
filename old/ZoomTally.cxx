/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/ZoomTally.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <queue>
#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "surfaceTally.h"
#include "tallyFactory.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "Triangle.h"
#include "Quadrilateral.h"
#include "RecTriangle.h"
#include "MeshGrid.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "bendSection.h"
#include "ZoomBend.h"
#include "makeZoom.h"
#include "ZoomTally.h"


namespace zoomSystem
{


void 
setShutterTally(Simulation& Sim,
		const shutterSystem::GeneralShutter& ZShutter)
/*!
    Set the different positions for the intensity.
    Given from a chipIR shutter. It calculates several poitions along the main 
    breamline. It has been modified to take 
    \param Sim :: Simulation system
    \param ZShutter :: Shutter that zoom uses
  */
{
  ELog::RegMethod RegA("ZoomTally","setBasicPosition");

  Geometry::Vec3D Pt;
  Geometry::Vec3D Axis;
  int tNum(15);
  // Shutter forward position:
  Pt=ZShutter.getFrontPt();
  Axis=ZShutter.getXYAxis();
  tallySystem::addF5Tally(Sim,tNum,Pt-Axis*5.0,
			  std::vector<Geometry::Vec3D>());
  tNum+=10;

  // Shutter Back position:
  Pt=ZShutter.getBackPt();
  tallySystem::addF5Tally(Sim,tNum,Pt+Axis*5.0,
			  std::vector<Geometry::Vec3D>());
  tNum+=10;

  return;
}

void 
setBasicPosition(Simulation& System,
		 const attachSystem::FixedComp& FC,
		 const size_t FCpoint,const double YStep)
  /*!
    Set the different positions for the intensity.
    Given from a chipIR shutter. It calculates several poitions along the main 
    breamline. It has been modified to take 
    \param System :: Simulation system
    \param FC :: Guide unit to create tally after
    \param FCpoint :: Point surface
    \param YStep :: distance to step
  */
{
  ELog::RegMethod RegA("ZoomTally","setBasicPosition");
  const masterRotate& MR=masterRotate::Instance();

  const int tNum=System.nextTallyNum(15);

  // Guide back point
  Geometry::Vec3D Pt=FC.getLinkPt(FCpoint);
  const Geometry::Vec3D TVec=FC.getLinkAxis(FCpoint);
  Pt+=TVec*YStep;      // Add so not on boundary
  ELog::EM<<"Tally "<<tNum<<" (point) = "<<MR.calcRotate(Pt)<<ELog::endDiag;
  tallySystem::addF5Tally(System,tNum,Pt,
			  std::vector<Geometry::Vec3D>());
  return;
}



  
}  // NAMESPACE zoomSystem
