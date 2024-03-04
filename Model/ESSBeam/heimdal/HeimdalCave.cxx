/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   heimdal/LegoBrick.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"

#include "HeimdalCave.h"

namespace essSystem
{

HeimdalCave::HeimdalCave(const std::string& Key)  :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedGroup("Main","Front"),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


HeimdalCave::~HeimdalCave() 
  /*!
    Destructor
  */
{}


void
HeimdalCave::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("HeimdalCave","populate");
  
  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  corridorWidth=Control.EvalVar<double>(keyName+"CorridorWidth");
  corridorLength=Control.EvalVar<double>(keyName+"CorridorLength");
  corridorTLen=Control.EvalVar<double>(keyName+"CorridorTLen");
  doorLength=Control.EvalVar<double>(keyName+"DoorLength");
  roofGap=Control.EvalVar<double>(keyName+"RoofGap");

  beamWidth=Control.EvalVar<double>(keyName+"BeamWidth");
  beamHeight=Control.EvalVar<double>(keyName+"BeamHeight");

  midZStep=Control.EvalVar<double>(keyName+"MidZStep");
  midThick=Control.EvalVar<double>(keyName+"MidThick");
  midHoleRadius=Control.EvalVar<double>(keyName+"MidHoleRadius");

  
  mainThick=Control.EvalVar<double>(keyName+"MainThick");
  subThick=Control.EvalVar<double>(keyName+"SubThick");

  extGap=Control.EvalVar<double>(keyName+"ExtGap");
  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}
  
void
HeimdalCave::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("HeimdalCave","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  if (!isActive("floor"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
      setCutSurf("floor",SMap.realSurf(buildIndex+5));
    }
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(length/2.0+mainThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length/2.0+mainThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(width/2.0+mainThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(width/2.0+mainThick),X);
  
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height/2.0+mainThick),Z);
  // extension gap for covering

  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*(length/2.0+mainThick+extGap),Y);

  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(width/2.0+mainThick+extGap),X);
  
  // corridor gap
  ModelSupport::buildPlane(SMap,buildIndex+112,
			   Origin+Y*(length/2.0-doorLength),Y);

  // inner corridor
  // relative to front wall
  ModelSupport::buildPlane(SMap,buildIndex+121,
			   Origin+Y*(corridorLength-length/2.0),Y);

  // relative to front wall
  ModelSupport::buildPlane(SMap,buildIndex+151,
			   Origin+Y*(corridorLength-length/2.0-subThick),Y);


  ModelSupport::buildPlane(SMap,buildIndex+124,
			   Origin+X*(width/2.0+mainThick+corridorWidth),X);
  // corridor walls
  ModelSupport::buildPlane(SMap,buildIndex+131,
			   Origin+Y*(corridorLength-length/2.0+subThick),Y);
      
  ModelSupport::buildPlane(SMap,buildIndex+134,Origin+X*
			   (width/2.0+2.0*mainThick+corridorWidth),X);

  // thin/thick corridor wall
  ModelSupport::buildPlane(SMap,buildIndex+1021,
			   Origin+Y*(corridorTLen-length/2.0),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+1034,Origin+X*
     (width/2.0+mainThick+subThick+corridorWidth),X);

  // Roof:
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(roofGap/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(roofGap/2.0),X);
  // step have half thickness
  ModelSupport::buildPlane(SMap,buildIndex+206,
			   Origin+Z*(height/2.0+subThick),Z);

  
  // beam gap
  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*(beamWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*(beamWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+305,Origin-Z*(beamHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,Origin+Z*(beamHeight/2.0),Z);

  // mid floor
  ModelSupport::buildPlane(SMap,buildIndex+405,
			   Origin+Z*(midZStep-midThick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+406,
			   Origin+Z*(midZStep+midThick/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+407,
			      Origin,Z,midHoleRadius);
  
  return;
}

void 
HeimdalCave::createObjects(Simulation& System)
  /*!
    Create a window object
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("HeimdalCave","createObjects");

  const HeadRule floorHR=getRule("floor");
  HeadRule HR;

  //  std::ostringstream cx;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 -6 (-405:406)");
  makeCell("MainVoid",System,cellIndex++,voidMat,0.0,HR*floorHR);  

  // walls/roof individually as we are going to make complex soon:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -3 13 -6");
  makeCell("leftWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  

     
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -112 4 -14 -6");
  makeCell("rightWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"112 -2 4 -14 -6");
  makeCell("rightGap",System,cellIndex++,voidMat,0.0,HR*floorHR);  
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -1 13 -134 -6 (-303:304:-305:306)");
  makeCell("frontWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -1 303 -304 305 -306");
  makeCell("frontVoid",System,cellIndex++,voidMat,0.0,HR*floorHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 13 -14 -6");
  makeCell("backWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  


 // Wall extension:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -13 23 -6");
  makeCell("leftExtGap",System,cellIndex++,voidMat,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -11 13 -134 -6 (-303:304:-305:306)");
  makeCell("frontExtGap",System,cellIndex++,voidMat,0.0,HR*floorHR); 
  
  // corridor
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -151 14 -124 -6");
  makeCell("corridor",System,cellIndex++,voidMat,0.0,HR*floorHR);  

    // corridor
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-121 151 14 -124 -6");
  makeCell("corridorWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -1021 14 124 -134 -6");
  makeCell("corridorWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  

  // thin corridor wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1021 -121 14 124 -1034 -6");
  makeCell("corridorWall",System,cellIndex++,wallMat,0.0,HR*floorHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1021 -121 14 1034 -134 -16");
  makeCell("corridorSpace",System,cellIndex++,voidMat,0.0,HR*floorHR);  
  
  // Roof
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "11 -12 13 -14 6 -16 (-203:204)");
  makeCell("mainRoof",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -121 14 -1034 6 -16");
  makeCell("corrRoof",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1021 1034 -134 6 -16");
  makeCell("corrRoof",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 203 -204 6 -16");
  makeCell("roofGap",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 203 -204 6 -206");
  makeCell("roofSup",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 203 -204 206 -16");
  makeCell("roofSupVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 203 -204 6 -206");
  makeCell("roofSup",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 203 -204 206 -16");
  makeCell("roofSupVoid",System,cellIndex++,voidMat,0.0,HR);  

  // ---------
  // MID LAYER
  // ---------
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 405 -406 407");
  makeCell("MidLayer",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"405 -406 -407");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,HR);  
  
  // back void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -121 13 -14 -16");
  makeCell("backVoid",System,cellIndex++,voidMat,0.0,HR*floorHR);  

  // here is where we difine the outer box/boundary:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -121 23 -134 -16");
  addOuterSurf("Main",HR*floorHR);
  return;
}

void
HeimdalCave::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("HeimdalCave","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System

