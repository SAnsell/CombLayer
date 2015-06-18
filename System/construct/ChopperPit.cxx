/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperPit.cxx
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

#include "ChopperPit.h"

namespace constructSystem
{

ChopperPit::ChopperPit(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  pitIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pitIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ChopperPit::~ChopperPit() 
  /*!
    Destructor
  */
{}

void
ChopperPit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ChopperPit","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidWidth");


  feHeight=Control.EvalVar<double>(keyName+"FeHeight");
  feDepth=Control.EvalVar<double>(keyName+"FeDepth");
  feWidth=Control.EvalVar<double>(keyName+"FeWidth");
  feFront=Control.EvalVar<double>(keyName+"FeFront");
  feBack=Control.EvalVar<double>(keyName+"FeBack");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  
  return;
}

void
ChopperPit::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ChopperPit","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  // set origin central
  yStep=voidLength/2.0+feFront;
  applyOffset();
  
  return;
}


void
ChopperPit::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ChopperPit","createSurfaces");


  // Inner void
  ModelSupport::buildPlane(SMap,pitIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,pitIndex+6,Origin+Z*voidHeight,Z);  

  // Fe system [front face is link surf]
  ModelSupport::buildPlane(SMap,pitIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+13,
			   Origin-X*((feWidth+voidWidth)/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+14,
			   Origin+X*((feWidth+voidWidth)/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+15,
			   Origin-Z*(voidDepth+feDepth),Z);
  ModelSupport::buildPlane(SMap,pitIndex+16,
			   Origin+Z*(voidHeight+feHeight),Z);  

  return;
}

void
ChopperPit::createObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
    \param FC :: FixedComp of front face
    \param sideIndex :: Fixed link point of object [signed]
   */
{
  ELog::RegMethod RegA("ChopperPit","createObjects");

  std::string Out;

  const std::string frontFace=
    FC.getSignedLinkString(sideIndex);
  
  // Void 
  Out=ModelSupport::getComposite(SMap,pitIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,pitIndex,
				 " -12 13 -14 15 -16 (-1:2:-3:4:-5:6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+frontFace));
  

  Out=ModelSupport::getComposite(SMap,pitIndex," -12 13 -14 15 -16 ");
  addOuterSurf(Out+frontFace);      

  return;
}

void
ChopperPit::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ChopperPit","createLinks");
  
  return;
}

void
ChopperPit::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("ChopperPit","createAll(FC)");
  return;
  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System,FC,FIndex);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
