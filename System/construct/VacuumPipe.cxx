/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VacuumPipe.cxx
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
#include <array>

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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "CellMap.h"

#include "VacuumPipe.h"

namespace constructSystem
{

VacuumPipe::VacuumPipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VacuumPipe::~VacuumPipe() 
  /*!
    Destructor
  */
{}

void
VacuumPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumPipe","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"radius");
  length=Control.EvalVar<double>(keyName+"length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");
  flangeThick=Control.EvalVar<double>(keyName+"FlangeThick");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  return;
}

void
VacuumPipe::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VacuumPipe","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  Origin+=Y*(flangeLength+length/2.0);
  return;
}


void
VacuumPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumPipe","createSurfaces");


  // Inner void

  ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,radius);

  ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,radius+feThick);

  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,flangeRadius);

  ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);
  
  ModelSupport::buildPlane(SMap,vacIndex+101,
			   Origin-Y*(length/2.0-flangeLength),Y);
  ModelSupport::buildPlane(SMap,vacIndex+102,
			   Origin+Y*(length/2.0-flangeLength),Y);
  
  return;
}

void
VacuumPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    \param FC :: FixedComp of front face
   */
{
  ELog::RegMethod RegA("VacuumPipe","createObjects");

  std::string Out;

  // Void 
  Out=ModelSupport::getComposite(SMap,vacIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -1 -107 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex," 2 -102 -107 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,
				 " 1 -2 13 -14 15 -16 (-3:4:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Box",cellIndex-1);
  // front/back faces
  Out=ModelSupport::getComposite(SMap,vacIndex,"11 -1 13 -14 15 -16 107");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Box",cellIndex-1);
  Out=ModelSupport::getComposite(SMap,vacIndex,"2 -12 13 -14 15 -16 107");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Box",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,vacIndex," 101 -11 107 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("FrontBellow",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex," 12 -102 107 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("BackBellow",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,vacIndex,
				 "(11 -12 13 -14 15 -16):(101 -102 -117)");
  addOuterSurf(Out);      
  return;
}

void
VacuumPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VacuumPipe","createLinks");

  FixedComp::setConnect(0,Origin-Y*(feFront+voidLength/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(feBack+voidLength/2.0),Y);
  FixedComp::setConnect(2,Origin-X*((feWidth+voidWidth)/2.0),-X);
  FixedComp::setConnect(3,Origin+X*((feWidth+voidWidth)/2.0),-X);
  FixedComp::setConnect(4,Origin-Z*(feDepth+voidDepth),-Z);
  FixedComp::setConnect(5,Origin+Z*(feHeight+voidHeight),Z);  

  FixedComp::setLinkSurf(0,-SMap.realSurf(vacIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(vacIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(vacIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+16));
  
  return;
}

void
VacuumPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
