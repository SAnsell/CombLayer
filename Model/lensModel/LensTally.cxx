/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/LensTally.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <memory>

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
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Source.h"
#include "Simulation.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "FrontBackCut.h"
#include "FlightLine.h"
#include "FlightCluster.h"
#include "TallyCreate.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "TallyCreate.h"
#include "LensTally.h"

namespace lensSystem
{

void 
addPointTally(Simulation&,
	      const lensSystem::FlightCluster&,
	      const size_t,
	      const double)
  /*!
    Set the different positions for the intensity.
    \param Sim :: Simulation system
    \param FC :: FLight cluster
    \param FN :: Flight line number
    \param dist :: Distance  of tally down flightline
  */
{
  ELog::RegMethod RegA("TMRSystem","setPointTally<FlightCluster>");
  /*
  const int masterPlane=FC.getFrontSurf();

  std::vector<int> TPlanes;
  FC.getPlanes(FN,TPlanes);
  Geometry::Vec3D Pt; 
  int tallyN=tallySystem::getFarPoint(Sim,Pt)+10;
  if (tallyN==10) tallyN=15;
  tallySystem::addF5Tally(Sim,tallyN,masterPlane,TPlanes[0],TPlanes[1],
   			  TPlanes[2],TPlanes[3],0.0);
  tallySystem::getFarPoint(Sim,Pt);                    // Get centre point
  // Calc rotation:
  Geometry::Vec3D Axis=FC.getAxis(FN);
  const Geometry::Vec3D X=Axis*Geometry::Vec3D(0,0,1);
  Geometry::Quaternion QR=Geometry::Quaternion::calcQRotDeg(angle,X);
  QR.rotate(Axis);
  tallySystem::setF5Position(Sim,tallyN,Pt,
			     Axis,1000.0,1.0);
  tallySystem::pointTally* PT=
    dynamic_cast<tallySystem::pointTally*>(Sim.getTally(tallyN)); 
  if (PT)
    PT->removeWindow();
  */
  return;
}


int
addSurfTally(Simulation& System,
	     const lensSystem::FlightCluster& FC,
	     const size_t FN,const double Dist)
  /*!
    Set the different positions for the intensity.
    Based on surface crossing tallies by inserting a plate
    \param System :: Simulation system
    \param FC :: FLight cluster
    \param FN :: Flight line number
    \param Dist :: Distanceof tally down
    \return tally number
  */
{
  ELog::RegMethod RegA("LensTally","addSurfTally<FlightCluster>");
   
  const attachSystem::FixedComp& FL=FC.getLine(FN);
  
  const Geometry::Vec3D& Y=FL.getLinkAxis(2);
  const Geometry::Vec3D& Org=FL.getCentre();
  
  int TNum=tallySystem::getLastTallyNumber(System,1);
  TNum=(TNum) ? TNum+10 : 1;

  std::ostringstream cx;
  cx<<"Plate"<<FN<<"_"<<TNum;
  insertSystem::insertPlate Pt(cx.str());
  
  // 10x10x10 box 
  Pt.setValues(10.0,1.0,10.0,0);
  Pt.createAll(System,Org+Y*Dist,FL);
  
  tallySystem::addF1Tally(System,TNum,Pt.getLinkSurf(1));
  return TNum;
}

}  // NAMESPACE lensSystem
