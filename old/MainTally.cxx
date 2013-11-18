/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/MainTally.cxx
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
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/array.hpp>

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
#include "support.h"
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
#include "inputParam.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "shutterBlock.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "InsertComp.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "BulkShield.h"
#include "TS2target.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "FlightLine.h"
#include "PreMod.h"
#include "HWrapper.h"
#include "Decoupled.h"
#include "VacVessel.h"
#include "RefCutOut.h"
#include "Bucket.h"
#include "CoolPad.h"
#include "Reflector.h"
#include "MainTally.h"


namespace TMRSystem
{

void
removeTallyWindows(Simulation& Sim)
  /*!
    Remove all the tally windows from the sim
    \param Sim :: Simulation
  */
{
  ELog::RegMethod RegA("MainTally","removeTallyWindows");
  typedef std::map<int,tallySystem::Tally*> tmTYPE;
  tmTYPE& TMap= Sim.getTallyMap();
  tmTYPE::iterator mc;
  for(mc=TMap.begin();mc!=TMap.end();mc++)
    {
      tallySystem::pointTally* PPtr=
	dynamic_cast<tallySystem::pointTally*>(mc->second);
      if (PPtr)
	PPtr->removeWindow();
    }
  return;
}

void
addDisplacement(Simulation& System,const int tNumber,
		const Geometry::Vec3D& DVec)
  /*!
    Add a specific displacement to the point tally
    \param System :: Simulation system
    \param tNumber :: Tally number
    \param DVec :: Displacement vector
   */
{
  ELog::RegMethod RegA("MainTally","addDisplacement");
  tallySystem::pointTally* TX=
    dynamic_cast<tallySystem::pointTally*>(System.getTally(tNumber)); 
  if (!TX)
    {
      ELog::EM<<"Error finding tally number "<<tNumber<<ELog::endErr;
      exit(1);
    }
  TX->setCentre(TX->getCentre()+DVec);
  return;
}

void 
setRadPosition(Simulation& Sim,
	       const shutterSystem::GeneralShutter& GS,
	       const shutterSystem::BulkInsert& BI)
  /*!
    Set the different positions for the intensity.
    Given from a chipIR shutter. It calculates several poitions along the main 
    beamline. It has been modified to take 
    \param Sim :: Simulation system
    \param GS :: General shutter
    \param BI :: Bulk Insert
  */
{
  ELog::RegMethod RegA("MainTally","setRadPosition");

  const masterRotate& MR=masterRotate::Instance();
  // Get next tally point:

  Geometry::Vec3D Pt;
  int tNum=tallySystem::getFarPoint(Sim,Pt);
  if (!tNum) tNum+=5;
  tNum+=10;
  tallySystem::addF5Tally(Sim,tNum);
  
  Pt=GS.getExit();  
  Geometry::Vec3D Axis=GS.getXYAxis();

  Pt-=Axis*5.0;
  tallySystem::setF5Position(Sim,tNum,Pt);
  ELog::EM<<"Tally "<<tNum<<" at position "<<MR.calcRotate(Pt)<<ELog::endDiag;


  // Tally at beginning BULK INSERT
  tNum+=10;
  
  Pt=BI.getExit();  
  Axis=BI.getExitNorm();
  Pt-=Axis*5.0;
  tallySystem::addF5Tally(Sim,tNum);
  tallySystem::setF5Position(Sim,tNum,Pt);
  ELog::EM<<"Tally "<<tNum<<" at position "<<MR.calcRotate(Pt)<<ELog::endDiag;
  
  return;
}




}  // NAMESPACE shutterSystem
