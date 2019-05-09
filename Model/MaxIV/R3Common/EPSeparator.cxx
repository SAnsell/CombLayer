/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/EPSeparator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "EPSeparator.h"

namespace xraySystem
{

EPSeparator::EPSeparator(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  epPairSet(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"Photon");
  nameSideIndex(3,"Electron");
}


EPSeparator::~EPSeparator()  
  /*!
    Destructor
  */
{}

void
EPSeparator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EPSeparator","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");

  photonXStep=Control.EvalVar<double>(keyName+"PhotonXStep");
  electronXStep=Control.EvalVar<double>(keyName+"ElectronXStep");
  electronXYAngle=Control.EvalVar<double>(keyName+"ElectronXYAngle");
  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");

  photonRadius=Control.EvalVar<double>(keyName+"PhotonRadius");

  wallXStep=Control.EvalVar<double>(keyName+"WallXStep");
  wallWidth=Control.EvalVar<double>(keyName+"WallWidth");
  wallHeight=Control.EvalVar<double>(keyName+"WallHeight");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}

void
EPSeparator::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("EPSeparator","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  if (!epPairSet)
    {
      const Geometry::Quaternion electronQ=
	Geometry::Quaternion::calcQRotDeg(electronXYAngle,Z);
      elecXAxis=electronQ.makeRotate(X);
      elecYAxis=electronQ.makeRotate(Y);
      elecOrg=Origin+X*electronXStep;
    }
  else
    elecXAxis=elecYAxis*Z;
  return;
}

void
EPSeparator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EPSeparator","createSurface");

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
    

  const Geometry::Vec3D WOrigin(Origin+X*wallXStep);
  ModelSupport::buildPlane(SMap,buildIndex+3,WOrigin-X*(wallWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,WOrigin+X*(wallWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,WOrigin-Z*(wallHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,WOrigin+Z*(wallHeight/2.0),Z);
    

  // flange cylinder/plangs
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*flangeLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length-flangeLength),Y);
  
  // electron inner :
  // divider
  ModelSupport::buildPlane(SMap,buildIndex+103,elecOrg,elecXAxis);
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,elecOrg,elecYAxis,electronRadius);
  
  // photon inner :
  ModelSupport::buildCylinder(SMap,buildIndex+207,photOrg,Y,photonRadius);
  ModelSupport::buildPlane(SMap,buildIndex+203,photOrg,X);
  ModelSupport::buildPlane(SMap,buildIndex+205,photOrg-Z*photonRadius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,photOrg+Z*photonRadius,Z);
  
  
  return;
}

void
EPSeparator::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EPSeparator","createObjects");

  const std::string frontSurf(ExternalCut::getRuleStr("front"));
  
  std::string Out;

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2 (-107 : -207 : (-103 205 -206 203) )");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2 3 -4 5 -6 (107  207 (-205 : 206 : 103 : -203)");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 -11  (-3:4:-5:6) ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 (-3:4:-5:6) 12 -2 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -7 (-3:4:-5:6) 11 -12 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -7 ");
  addOuterSurf(Out+frontSurf);

  return;
}

void 
EPSeparator::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("EPSeparator","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);

  // photon/electron
  setConnect(1,Origin+Y*length,Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+2));

  // Photon centre line [exit]
  const Geometry::Vec3D photOrg(Origin+X*photonXStep);
      
  setConnect(2,photOrg+Y*length,Y);  
  setLinkSurf(2,SMap.realSurf(buildIndex+2));
  
  // electron surface is intersect from 102 normal into surface 2
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+2));
  FixedComp::setLineConnect(3,elecOrg,elecYAxis);
  
  return;
}

void
EPSeparator::setEPOriginPair(const attachSystem::FixedComp& FC,
			     const long int photonIndex,
			     const long int electronIndex)
  /*!
    Set the electron/Photon origins exactly
    \param FC :: FixedPoint
    \param photonIndex :: link point for photon
    \param electornIndex :: link point for electron
   */
{
  ELog::RegMethod RegA("EPSparator","setEPOriginPair");

  photOrg=FC.getLinkPt(photonIndex);
  elecOrg=FC.getLinkPt(electronIndex);
  
  elecYAxis=FC.getLinkAxis(electronIndex);

  epPairSet=1;
  
  return;
}
  

void
EPSeparator::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("EPSeparator","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
