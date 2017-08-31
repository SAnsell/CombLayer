/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/ProtonFlight.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ProtonFlight.h"


namespace lensSystem
{

ProtonFlight::ProtonFlight(const std::string& Key)  :
  attachSystem::ContainedGroup("box","line"),
  attachSystem::FixedComp(Key,2),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ProtonFlight::ProtonFlight(const ProtonFlight& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedComp(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
  boxX(A.boxX),boxY(A.boxY),boxZ(A.boxZ),
  backSurf(A.backSurf),targetCell(A.targetCell),Angle(A.Angle),
  YOffset(A.YOffset),width(A.width),
  height(A.height),targetBeThick(A.targetBeThick),
  targetBeWidth(A.targetBeWidth),targetBeHeight(A.targetBeHeight),
  targetWaterThick(A.targetWaterThick),targetWaterWidth(A.targetWaterWidth),
  targetWaterHeight(A.targetWaterHeight),targetHeight(A.targetHeight),
  targetThick(A.targetThick),targetWidth(A.targetWidth),targetMat(A.targetMat),
  targetCoolant(A.targetCoolant),targetSurround(A.targetSurround),
  wallThick(A.wallThick),wallMat(A.wallMat),protonEnergy(A.protonEnergy)
  /*!
    Copy constructor
    \param A :: ProtonFlight to copy
  */
{}

ProtonFlight&
ProtonFlight::operator=(const ProtonFlight& A)
  /*!
    Assignment operator
    \param A :: ProtonFlight to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      boxX=A.boxX;
      boxY=A.boxY;
      boxZ=A.boxZ;
      backSurf=A.backSurf;
      targetCell=A.targetCell;
      Angle=A.Angle;
      YOffset=A.YOffset;
      width=A.width;
      height=A.height;
      targetBeThick=A.targetBeThick;
      targetBeWidth=A.targetBeWidth;
      targetBeHeight=A.targetBeHeight;
      targetWaterThick=A.targetWaterThick;
      targetWaterWidth=A.targetWaterWidth;
      targetWaterHeight=A.targetWaterHeight;
      targetHeight=A.targetHeight;
      targetThick=A.targetThick;
      targetWidth=A.targetWidth;
      targetMat=A.targetMat;
      targetCoolant=A.targetCoolant;
      targetSurround=A.targetSurround;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      protonEnergy=A.protonEnergy;
    }
  return *this;
}


ProtonFlight::~ProtonFlight() 
  /*!
    Destructor
  */
{}

void
ProtonFlight::populate(  const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("ProtonFlight","populate");

  Angle=Control.EvalVar<double>(keyName+"Angle");
  YOffset=Control.EvalVar<double>(keyName+"YOffset");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");

  targetWidth=Control.EvalVar<double>(keyName+"TargetWidth");
  targetHeight=Control.EvalVar<double>(keyName+"TargetHeight");
  targetThick=Control.EvalVar<double>(keyName+"TargetThick");
  targetBeThick=Control.EvalVar<double>(keyName+"TargetBeThick");
  targetBeWidth=Control.EvalVar<double>(keyName+"TargetBeWidth");
  targetBeHeight=Control.EvalVar<double>(keyName+"TargetBeHeight");
  targetWaterThick=Control.EvalVar<double>(keyName+"TargetWaterThick");
  targetWaterWidth=Control.EvalVar<double>(keyName+"TargetWaterWidth");
  targetWaterHeight=Control.EvalVar<double>(keyName+"TargetWaterHeight");

  targetMat=ModelSupport::EvalMat<int>(Control,keyName+"TargetMat");
  targetCoolant=ModelSupport::EvalMat<int>(Control,keyName+"TargetCoolant");
  targetSurround=ModelSupport::EvalMat<int>(Control,keyName+"TargetSurround");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  protonEnergy=Control.EvalVar<double>(keyName+"Energy");

  return;
}

void
ProtonFlight::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ProtonFlight","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  
  boxX=X;
  boxY=Y;
  boxZ=Z;
  
  Origin+=Y*YOffset;
  Geometry::Quaternion::calcQRotDeg(Angle,Z).rotate(Y);
  X=Y*Z;
  
  return;
}


void
ProtonFlight::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("ProtonFlight","createSurface");

  ModelSupport::buildPlane(SMap,protonIndex+1,Origin,boxY);

  SMap.addMatch(protonIndex+2,-backSurf);

  ModelSupport::buildPlane(SMap,protonIndex+3,Origin-X*width,X);
  ModelSupport::buildPlane(SMap,protonIndex+4,Origin+X*width,X);
  ModelSupport::buildPlane(SMap,protonIndex+5,Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,protonIndex+6,Origin+Z*height,Z);
  // Inner layer :

  ModelSupport::buildPlane(SMap,protonIndex+13,
			   Origin-X*(width-wallThick),X);
  ModelSupport::buildPlane(SMap,protonIndex+14,
			   Origin+X*(width-wallThick),X);
  ModelSupport::buildPlane(SMap,protonIndex+15,
			   Origin-Z*(height-wallThick),Z);
  ModelSupport::buildPlane(SMap,protonIndex+16,
			   Origin+Z*(height-wallThick),Z);
  // Proton Target:
  // BE:
  ModelSupport::buildPlane(SMap,protonIndex+102,
			   Origin+boxY*targetBeThick,boxY);
  ModelSupport::buildPlane(SMap,protonIndex+103,
			   Origin-boxX*targetBeWidth/2,boxX);
  ModelSupport::buildPlane(SMap,protonIndex+104,
			   Origin+boxX*targetBeWidth/2,boxX);
  ModelSupport::buildPlane(SMap,protonIndex+105,
			   Origin-boxZ*targetBeHeight/2,boxZ);
  ModelSupport::buildPlane(SMap,protonIndex+106,
			   Origin+boxZ*targetBeHeight/2,boxZ);

  // WATER:  
  ModelSupport::buildPlane(SMap,protonIndex+112,
			   Origin+boxY*targetWaterThick,boxY);
  ModelSupport::buildPlane(SMap,protonIndex+113,
			   Origin-boxX*targetWaterWidth/2,boxX);
  ModelSupport::buildPlane(SMap,protonIndex+114,
			   Origin+boxX*targetWaterWidth/2,boxX);
  ModelSupport::buildPlane(SMap,protonIndex+115,
			   Origin-boxZ*targetWaterHeight/2,boxZ);
  ModelSupport::buildPlane(SMap,protonIndex+116,
			   Origin+boxZ*targetWaterHeight/2,boxZ);
  
  // AL:
  ModelSupport::buildPlane(SMap,protonIndex+121,
			   Origin-boxY*targetThick,boxY);
  ModelSupport::buildPlane(SMap,protonIndex+122,
			   Origin+boxY*targetThick,boxY);
  ModelSupport::buildPlane(SMap,protonIndex+123,
			   Origin-boxX*targetWidth/2,boxX);
  ModelSupport::buildPlane(SMap,protonIndex+124,
			   Origin+boxX*targetWidth/2,boxX);
  ModelSupport::buildPlane(SMap,protonIndex+125,
			   Origin-boxZ*targetHeight/2,boxZ);
  ModelSupport::buildPlane(SMap,protonIndex+126,
			   Origin+boxZ*targetHeight/2,boxZ);
  return;
}

void
ProtonFlight::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ProtonFlight","createObjects");

  std::string Out;
  // Inner void:
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 2 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Al - Layer
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "-1 2 3 -4 5 -6 (-13 : 14 : -15 : 16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Make exclude unit
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 2 3 -4 5 -6");
  addOuterSurf("line",Out);
  return;
}

void
ProtonFlight::createTarget(Simulation& System)
  /*!
    Adds the proton target 
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ProtonFlight","createTarget");

  std::string Out;
  // Be slab
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -102 103 -104 105 -106");
  System.addCell(MonteCarlo::Qhull(cellIndex++,targetMat,0.0,Out));
  
  // Water coolant
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "(102 : -103 : 104 : -105 : 106) "
				 "1 -112 113 -114 115 -116");
  System.addCell(MonteCarlo::Qhull(cellIndex++,targetCoolant,0.0,Out));

  // Al - Layer
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "(-1 : 112 : -113 : 114 : -115 : 116) "
				 "121 -122 123 -124 125 -126"
				 "(1 : -3 : 4 : -5 : 6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,targetSurround,0.0,Out));


  // Make exclude unit
  Out=ModelSupport::getComposite(SMap,protonIndex,"121 -122 123 -124 125 -126");
  addOuterSurf("box",Out);  
  return;
}

int
ProtonFlight::exitWindow(const double Dist,
			 std::vector<int>& window,
			 Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(SMap.realSurf(protonIndex+3));
  window.push_back(SMap.realSurf(protonIndex+4));
  window.push_back(SMap.realSurf(protonIndex+5));
  window.push_back(SMap.realSurf(protonIndex+6));
  Pt=Origin+Y*Dist;  
  return SMap.realSurf(protonIndex+1);
}

  
void
ProtonFlight::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed system for origin
    \param sideIndex :: Surface for out
  */
{
  ELog::RegMethod RegA("ProtonFlight","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createTarget(System);

  insertObjects(System);   
  return;
}
  
}  // NAMESPACE lensSystem
