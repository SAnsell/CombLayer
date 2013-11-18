/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/ChipTally.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
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
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Triangle.h"
#include "Quadrilateral.h"
#include "RecTriangle.h"
#include "MeshGrid.h"
#include "Rules.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "shutterBlock.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "InsertComp.h"
#include "GeneralShutter.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "BulkInsert.h"
#include "BulkShield.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "Collimator.h"
#include "ColBox.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "InnerWall.h"
#include "Hutch.h"
#include "inputParam.h"
#include "ChipTally.h"


extern MTRand Rand;

namespace TMRSystem
{

int
setGrid(Simulation& System,
	const hutchSystem::ChipIRGuide& GObj,
	const mainSystem::inputParam& IParam)
/*!
  Calculate a grid of positions about the table 1.
  Given from a chipIR shutter. It calculates several poitions along the main 
  beamline. 
  
  Note numbers:
   - 1 :: Filter Set
   - 2 :: End

  \param System :: Simulation system
  \param GObj :: Guide oubect object
  \param IParam :: Master input parameters
  \return true if variables allow this to be run
*/
{
  ELog::RegMethod RegA("ChipTally","setGrid<ChipGuide>");

  const FuncDataBase& Control=System.getDataBase();


  const int placeIndex=IParam.getValue<int>("tally",2)-1;
  const int initNPD=IParam.getValue<int>("TGrid",0);   // First point to used
  const int NPD=IParam.getValue<int>("TGrid",1);       // Total 

  const double hshift=Control.EvalVar<double>("gridTallyHShift");
  const double vshift=Control.EvalVar<double>("gridTallyVShift");
  const double hOffset=Control.EvalVar<double>("gridTallyHOffset");
  const double vOffset=Control.EvalVar<double>("gridTallyVOffset");
  
  if (placeIndex>3 || placeIndex<0) 
    {
      ELog::EM<<"Failed to get correct table number : "
	      <<placeIndex+1<<ELog::endErr;
      return 0;
    }
 

  return 1;
}

void 
setMultiPosition(Simulation& Sim,
		 const hutchSystem::ChipIRGuide& GObj,
		 const hutchSystem::ChipIRHutch& HObj,
		 const shutterSystem::BulkShield& BS,
		 const int noWindowFlag)
  /*!
    Set the different positions for the intensity.
    Given from a chipIR shutter. It calculates several poitions along the main 
    breamline. It has been modified to take 
    \param Sim :: Simulation system
    \param GObj :: Guide object
    \param HObj :: Hutch object
    \param BS :: BulkShield
    \param noWindowFlag :: Flag to not use window
  */
{
  ELog::RegMethod RegA("ChipTally","setMultiPosition");

  const masterRotate& MR=masterRotate::Instance();

  const int NPD(2);
  int tNum;
  std::vector<int> Planes;  
  std::vector<int> SPlanes;  
  std::vector<int> TPlanes;  
  int outerDivide;
  int torpedoDivide;
  int shutterDivide;
  const int masterPlane=BS.calcOuterPlanes(0,Planes,outerDivide);
  const int shutterPlane=BS.calcShutterPlanes(0,SPlanes,shutterDivide);
  const int torpedoPlane=BS.calcShutterPlanes(0,TPlanes,torpedoDivide);
    
  int tallyNum(15);
  // SHUTTER POSITION : MUST NOT BEYOND START OF INSERT
  tallySystem::addF5Tally(Sim,tallyNum,torpedoPlane,TPlanes[0],TPlanes[1],
			  TPlanes[2],TPlanes[3],5.0,TPlanes[4]);
  tallyNum+=10;
  // SHUTTER POSITION : END OF INSERT
  tallySystem::addF5Tally(Sim,tallyNum,shutterPlane,TPlanes[0],TPlanes[1],
			  TPlanes[2],TPlanes[3],5.0,TPlanes[4]);
  // IMPACT POINT ::
  //  tallySystem::setF5Position(Sim,15,PipeCent,PipeAxis,8.0);

  //tallySystem::writePlanes(0,shutterPlane,SPlanes);
  //  tallySystem::writePlanes(0,shutterPlane,TPlanes);

  // ANGLE POSITIONS ::
  for(int i=0;i<NPD;i++)
    {
      if (noWindowFlag)
	tallySystem::addF5Tally(Sim,tallyNum);
      else
	tallySystem::addF5Tally(Sim,tallyNum,masterPlane,Planes[0],Planes[1],
				Planes[2],Planes[3],1.0,Planes[4]);
      
      const Geometry::Vec3D IP=HObj.getImpactPoint();
      Geometry::Vec3D TPos=HObj.calcIndexPosition(i)-IP;
      
      Geometry::Quaternion::calcQRotDeg(HObj.getBeamAngle(),
					HObj.getX()).rotate(TPos);
      TPos+=IP;
      tallySystem::setF5Position(Sim,tallyNum,TPos);
      RegA.incIndent();
      ELog::FM<<"TABLE TALLY POSITION "<<i+1<<" : "
	      <<MR.calcRotate(TPos)
	      <<ELog::endDiag;

      ELog::EM<<"F"<<tallyNum<<" Pos == "<<MR.calcRotate(TPos)
	      <<" from table pos "<<MR.calcRotate(HObj.calcIndexPosition(i))
	      <<ELog::endDiag;
      RegA.decIndent();
      tallyNum+=10;
    }

  // Detector in shutter position: 
  tNum=25+NPD*10;
  const int cutPlane=GObj.calcTallyPlanes(1,TPlanes);
  tallySystem::addF5Tally(Sim,tNum,cutPlane,TPlanes[0],TPlanes[1],
  			  TPlanes[2],TPlanes[3],5.0,TPlanes[4]);
 

  //  tallySystem::addF5Tally(Sim,tNum,masterPlane,TPlanes[0],TPlanes[1],
  //			  TPlanes[2],TPlanes[3],5.0,TPlanes[4]);
  /// Output
  const tallySystem::pointTally *TPtr=
    dynamic_cast<const tallySystem::pointTally*>(Sim.getTally(tNum));
  if (TPtr)
    {
      ELog::EM<<"  Beamline TALLY POSITION : "
	      <<MR.calcRotate(TPtr->getCentre())
	  <<ELog::endDiag;
      ELog::EM<<" -- Plane == "<<masterPlane<<ELog::endDiag;
    }

  
  // Detector in the middle of the server space
  tNum=35+NPD*10;
  tallySystem::addF5Tally(Sim,tNum);
  tallySystem::setF5Position(Sim,tNum,HObj.getServerCentre());

  ELog::EM<<"  SERVER TALLY POSITION : "
	  <<MR.calcRotate(HObj.getServerCentre())
	  <<ELog::endDiag;

  
  return;
}


}  // NAMESPACE shutterSystem
