/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Bunker.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "Bunker.h"

namespace essSystem
{

Bunker::Bunker(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  bnkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bnkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bunker::~Bunker() 
  /*!
    Destructor
  */
{}

void
Bunker::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable data base
 */
{
  ELog::RegMethod RegA("Bunker","populate");

  leftPhase=Control.EvalVar<double>(keyName+"LeftPhase");
  rightPhase=Control.EvalVar<double>(keyName+"RightPhase");
  leftAngle=Control.EvalVar<double>(keyName+"LeftAngle");
  rightAngle=Control.EvalVar<double>(keyName+"RightAngle");
  
  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  roofHeight=Control.EvalVar<double>(keyName+"RoofHeight");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}
  
void
Bunker::createUnitVector(const attachSystem::FixedComp& MainCentre,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param FC :: Linked object
    \param sideIndex :: Side for linkage centre
  */
{
  ELog::RegMethod RegA("Bunker","createUnitVector");

  rotCentre=MainCentre.getCentre();
  FixedComp::createUnitVector(FC,sideIndex);
  
  return;
}
  
void
Bunker::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Bunker","createSurface");

  innerRadius=rotCentre.Distance(Origin);
  
  Geometry::Vec3D AWallDir(X);
  Geometry::Vec3D BWallDir(X);
  // rotation of axis:
  Geometry::Quaternion::calcQRotDeg(leftAngle,Z).rotate(AWallDir);
  Geometry::Quaternion::calcQRotDeg(rightAngle,Z).rotate(BWallDir);
  // rotation of phase points:

  // Points on wall
  Geometry::Vec3D AWall(Origin-rotCentre);
  Geometry::Vec3D BWall(Origin-rotCentre);
  Geometry::Quaternion::calcQRotDeg(leftPhase,Z).rotate(AWall);
  Geometry::Quaternion::calcQRotDeg(rightPhase,Z).rotate(BWall);

  // Divider
  ModelSupport::buildPlane(SMap,bnkIndex+1,rotCentre,Y);
  ModelSupport::buildCylinder(SMap,bnkIndex+7,rotCentre,Z,wallRadius);
    
  ModelSupport::buildPlane(SMap,bnkIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+4,BWall,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,bnkIndex+6,Origin-Z*roofHeight,Z);

  // Walls
  ModelSupport::buildCylinder(SMap,bnkIndex+17,rotCentre,
			      Z,wallRadius+wallThick);
    
  ModelSupport::buildPlane(SMap,bnkIndex+13,
			   AWall-AWallDir*wallThick,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+14,
			   BWall+BWallDir*wallThick,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+5,
			   Origin-Z*(floorDepth+floorThick),Z);
  ModelSupport::buildPlane(SMap,bnkIndex+6,
			   Origin-Z*(roofHeight+roofThick),Z);
  
  
  return;
}

void
Bunker::createObjects(Simulation& System,
		      const attachSystem::FixedComp& FC,  
		      const long int sideIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Side of bulk shield + divider(?)
    \param sideIndex :: side of linke point
  */
{
  ELog::RegMethod RegA("Bunker","createObjects");
  
  std::string Out;
  const std::string Inner=FC.getSignedLinkString(sideIndex);
  
  Out=ModelSupport::getComposite(SMap,bnkIndex,"1 -7 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Inner));


  Out=ModelSupport::getComposite(SMap,bnkIndex,
				 " 1 -17 13 -14 15 -16 (7:-3:4:5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
		 

  Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 3 -4 5 -6 ");
  addOuterSurf(Out);
  
  return;
}

void
Bunker::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Bunker","createLinks");
  return;
}


void
Bunker::createAll(Simulation& System,
		  const attachSystem::FixedComp& MainCentre,
		  const attachSystem::FixedComp& FC,
		  const long int linkIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param MainCentre :: Rotatioin Centre
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
  */
{
  ELog::RegMethod RegA("Bunker","createAll");

  populate(System.getDataBase());
  createUnitVector(MainCentre,FC,linkIndex);
  createSurfaces();
  createLinks();
  createObjects(System,FC,linkIndex);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
