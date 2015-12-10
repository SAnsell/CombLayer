/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/VacTank.cxx
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
#include <numeric>
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
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "CellMap.h"
#include "VacTank.h"

namespace essSystem
{

VacTank::VacTank(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),attachSystem::CellMap(),
  tankIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tankIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


VacTank::~VacTank() 
 /*!
   Destructor
 */
{}

void
VacTank::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database of variable
 */
{
  ELog::RegMethod RegA("VacTank","populate");
  
  FixedOffset::populate(Control);


  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");


  windowThick=Control.EvalVar<double>(keyName+"WindowThick");
  windowRadius=Control.EvalVar<double>(keyName+"WindowRadius");
  windowInsetLen=Control.EvalVar<double>(keyName+"WindowInsetLen");
  
  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");

  return;
}
  
void
VacTank::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
    \param sideIndex :: sinde track
  */
{
  ELog::RegMethod RegA("VacTank","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);

  applyOffset();
  return;
}
  
void
VacTank::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("VacTank","createSurface");

  // Main cylinder tank
  ModelSupport::buildPlane(SMap,tankIndex+1,
			   Origin+Y*windowInsetLen,Y);
  ModelSupport::buildPlane(SMap,tankIndex+2,
			   Origin+Y*(windowInsetLen+length),Y);
  ModelSupport::buildCylinder(SMap,tankIndex+7,Origin,Y,radius);

  ModelSupport::buildPlane(SMap,tankIndex+12,
			   Origin+Y*(windowInsetLen+length+backThick),Y);
  ModelSupport::buildCylinder(SMap,tankIndex+17,
			   Origin,Y,radius+sideThick);

  //
  // Nose cone:
  //
  ModelSupport::buildPlane(SMap,tankIndex+101,Origin,Y);
  ModelSupport::buildPlane(SMap,tankIndex+102,Origin-Y*windowThick,Y);

  const double coneAngle=180.0*atan((radius-windowRadius)/windowInsetLen)/M_PI;
  const Geometry::Vec3D coneOrigin=Origin-Y*
    (windowRadius*windowInsetLen)/(radius-windowRadius);

  ModelSupport::buildCone(SMap,tankIndex+108,
			  coneOrigin,Y,coneAngle);
  ModelSupport::buildCone(SMap,tankIndex+118,
			  coneOrigin-Y*(frontThick*sin(coneAngle*M_PI/180.0)),
			  Y,coneAngle);
  
  return;
}

void
VacTank::createObjects(Simulation& System)
  /*!
    Adds the vacuum object
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("VacTank","createObjects");
  
  std::string Out;

  // Main voids
  Out=ModelSupport::getComposite(SMap,tankIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Steel layer
  Out=ModelSupport::getComposite(SMap,tankIndex,"1 -12 -17 -118 (2:7)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Steel",cellIndex-1);

  // Nose cone
  Out=ModelSupport::getComposite(SMap,tankIndex,"101 -1 -108");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Nose Steel
  Out=ModelSupport::getComposite(SMap,tankIndex,"101 -1 -118 108");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Steel",cellIndex-1);

  // window
  Out=ModelSupport::getComposite(SMap,tankIndex,"102 -101 -118");
  System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,Out));
  addCell("Window",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,tankIndex,"102 -118 -12 -17");
  addOuterSurf(Out);
  
  return;
}

void
VacTank::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("VacTank","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*windowThick,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(tankIndex+101));
  FixedComp::setConnect(1,Origin+Y*(length+windowInsetLen),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(tankIndex+2));

  return;
}

void
VacTank::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("VacTank","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE zoomSystem
