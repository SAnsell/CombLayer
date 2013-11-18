/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imat/ChopperPit.cxx
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ChopperPit.h"

namespace imatSystem
{

ChopperPit::ChopperPit(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::TwinComp(Key,6),
  pitIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pitIndex+1),innerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ChopperPit::~ChopperPit() 
 /*!
   Destructor
 */
{}

void
ChopperPit::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("ChopperPit","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  feSide=Control.EvalVar<double>(keyName+"FeSide");
  feBase=Control.EvalVar<double>(keyName+"FeBase");
  feTop=Control.EvalVar<double>(keyName+"FeTop");
  feBack=Control.EvalVar<double>(keyName+"FeBack");
  feFront=Control.EvalVar<double>(keyName+"FeFront");

  wallSide=Control.EvalVar<double>(keyName+"WallSide");
  wallBase=Control.EvalVar<double>(keyName+"WallBase");
  wallTop=Control.EvalVar<double>(keyName+"WallTop");
  wallBack=Control.EvalVar<double>(keyName+"WallBack");
  wallFront=Control.EvalVar<double>(keyName+"WallFront");
 
  feMat=Control.EvalVar<int>(keyName+"FeMat");
  wallMat=Control.EvalVar<int>(keyName+"WallMat");

  return;
}
  
void
ChopperPit::createUnitVector(const attachSystem::TwinComp& TC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param TC :: A second track to use the beam direction as basis
  */
{
  ELog::RegMethod RegA("ChopperPit","createUnitVector");
  attachSystem::TwinComp::createUnitVector(TC);
  FixedComp::applyShift(xStep,yStep,zStep);
  FixedComp::applyAngleRotate(xyAngle,zAngle);
  bEnter=Origin;

  return;
}

void
ChopperPit::createSurfaces()
  /*!
    Create All the surfaces
    -- Outer plane is going 
  */
{
  ELog::RegMethod RegA("ChopperPit","createSurfaces");

  ModelSupport::buildPlane(SMap,pitIndex+1,Origin-Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,pitIndex+2,Origin+Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,pitIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,pitIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,pitIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,pitIndex+6,Origin+Z*height/2.0,Z);


  ModelSupport::buildPlane(SMap,pitIndex+11,
			   Origin-Y*(length/2.0+feBack),Y);
  ModelSupport::buildPlane(SMap,pitIndex+12,
			   Origin+Y*(length/2.0+feFront),Y);
  ModelSupport::buildPlane(SMap,pitIndex+13,
			   Origin-X*(width/2.0+feSide),X);
  ModelSupport::buildPlane(SMap,pitIndex+14,
			   Origin+X*(width/2.0+feSide),X);
  ModelSupport::buildPlane(SMap,pitIndex+15,
			   Origin-Z*(height/2.0+feTop),Z);
  ModelSupport::buildPlane(SMap,pitIndex+16,
			   Origin+Z*(height/2.0+feBase),Z);

  ModelSupport::buildPlane(SMap,pitIndex+21,
			   Origin-Y*(length/2.0+feBack+wallBack),Y);
  ModelSupport::buildPlane(SMap,pitIndex+22,
			   Origin+Y*(length/2.0+feFront+wallFront),Y);
  ModelSupport::buildPlane(SMap,pitIndex+23,
			   Origin-X*(width/2.0+feSide+wallSide),X);
  ModelSupport::buildPlane(SMap,pitIndex+24,
			   Origin+X*(width/2.0+feSide+wallSide),X);
  ModelSupport::buildPlane(SMap,pitIndex+25,
			   Origin-Z*(height/2.0+feTop+wallTop),Z);
  ModelSupport::buildPlane(SMap,pitIndex+26,
			   Origin+Z*(height/2.0+feBase+wallBase),Z);

  return;
}

void
ChopperPit::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("ChopperPit","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,pitIndex,"21 -22 23 -24 25 -26");
  addOuterSurf(Out);
  
  // Inner void
  Out=ModelSupport::getComposite(SMap,pitIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  
  return;
}

void
ChopperPit::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("ChopperPit","createLinks");

  SecondTrack::setBeamExit(Origin+bY*length,bY);
  setExit(Origin+bY*length,bY);

  FixedComp::setConnect(0,Origin,-Y);      // Note always to the reactor

  //  FixedComp::setLinkSurf(1,SMap.realSurf(pitIndex+2));

  return;
}

void
ChopperPit::createAll(Simulation& System,
		      const attachSystem::TwinComp& TC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param TC :: BulkInsert/IMatInsert
  */
{
  ELog::RegMethod RegA("ChopperPit","createAll");
  populate(System);

  createUnitVector(TC);
  createSurfaces();
  createObjects(System);
  insertObjects(System); 
  createLinks();

  return;
}

  
}  // NAMESPACE imatSystem
