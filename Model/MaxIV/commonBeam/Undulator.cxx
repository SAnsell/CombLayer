/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Undulator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "Undulator.h"

namespace xraySystem
{

Undulator::Undulator(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Undulator::~Undulator() 
  /*!
    Destructor
  */
{}

void
Undulator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Undulator","populate");
  
  FixedOffset::populate(Control);

  vGap=Control.EvalVar<double>(keyName+"VGap");

  length=Control.EvalVar<double>(keyName+"Length");
  magnetWidth=Control.EvalVar<double>(keyName+"MagnetWidth");
  magnetDepth=Control.EvalVar<double>(keyName+"MagnetDepth");

  supportWidth=Control.EvalVar<double>(keyName+"SupportWidth");
  supportThick=Control.EvalVar<double>(keyName+"SupportThick");
  supportLength=Control.EvalVar<double>(keyName+"SupportLength");
  if (supportLength<length-Geometry::zeroTol)
    supportLength+=length;
  sVOffset=Control.EvalVar<double>(keyName+"SupportVOffset");

  standHeight=Control.EvalVar<double>(keyName+"StandHeight");
  standWidth=Control.EvalVar<double>(keyName+"StandWidth");


  magnetMat=ModelSupport::EvalMat<int>(Control,keyName+"MagnetMat");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  standMat=ModelSupport::EvalMat<int>(Control,keyName+"StandMat");

  return;
}

void
Undulator::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Undulator","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
Undulator::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Undulator","createSurfaces");

  // Build Magnet first
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(magnetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(magnetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(vGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(vGap/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(magnetDepth+vGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+
			   Z*(magnetDepth+vGap/2.0),Z);
  

  // Build Support
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(supportLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(supportLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(supportWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(supportWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(sVOffset+vGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(sVOffset+vGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+115,Origin
			   -Z*(supportThick+sVOffset+vGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,Origin
			   +Z*(supportThick+sVOffset+vGap/2.0),Z);


  // Build stands
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(standWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(standWidth/2.0),X);
    
  ModelSupport::buildPlane(SMap,buildIndex+205,Origin
			   -Z*(standHeight+supportThick+sVOffset+vGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin
			   +Z*(standHeight+supportThick+sVOffset+vGap/2.0),Z);

  
  return;
}

void
Undulator::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Undulator","createObjects");

  std::string Out;

  // Main inner void
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"101 -102 103 -104 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  // corner voids
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 -3 103 -5 105");
  makeCell("VoidCut",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 4 -104 -5 105");
  makeCell("VoidCut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 -3 103 6 -106");
  makeCell("VoidCut",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 4 -104 6 -106");
  makeCell("VoidCut",System,cellIndex++,0,0.0,Out);

  // front /back void
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"101 -1 103 -104 -106 6 ");
  makeCell("FVoidCut",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"101 -1 103 -104 -5 105 ");
  makeCell("FVoidCut",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"2 -102 103 -104 -106 6 ");
  makeCell("BVoidCut",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"2 -102 103 -104 -5 105 ");
  makeCell("BVoidCut",System,cellIndex++,0,0.0,Out);

  // MAGNET
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 3 -4 -5 15");
  makeCell("BaseMagnet",System,cellIndex++,magnetMat,0.0,Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 3 -4 6 -16");
  makeCell("TopMagnet",System,cellIndex++,magnetMat,0.0,Out);

  // Support
  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 103 -104 -105 115 (-1:2:-3:4:-15) ");
  makeCell("BaseSupport",System,cellIndex++,supportMat,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 103 -104 106 -116 (-1:2:-3:4:16) ");
  makeCell("TopSupport",System,cellIndex++,supportMat,0.0,Out);

  // Stand
  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 203 -204 -115 205 ");
  makeCell("BaseStand",System,cellIndex++,standMat,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 203 -204 116 -206 ");
  makeCell("TopStand",System,cellIndex++,standMat,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 103 -203 116 -206 ");
  makeCell("TopStandVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 204 -104 116 -206 ");
  makeCell("TopStandVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 103 -203 -115 205 ");
  makeCell("BaseStandVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"101 -102 204 -104 -115 205 ");
  makeCell("BaseStandVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 103 -104 205 -206 ");
  addOuterSurf(Out);      

  return;
}

void
Undulator::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Undulator","createLinks");
  
  setConnect(0,Origin-Y*(supportLength/2.0),-Y);
  setConnect(1,Origin+Y*(supportLength/2.0),Y);
  
  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
Undulator::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Undulator","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
