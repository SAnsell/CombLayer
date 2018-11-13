/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   species/TankMonoVessel.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "portItem.h"

#include "TankMonoVessel.h"

namespace xraySystem
{

TankMonoVessel::TankMonoVessel(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  centreOrigin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}

TankMonoVessel::~TankMonoVessel() 
  /*!
    Destructor
  */
{}

void
TankMonoVessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TankMonoVessel","populate");
  
  FixedOffset::populate(Control);

  
  // Void + Fe special:
  voidRadius=Control.EvalVar<double>(keyName+"VoidWidth");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  baseGap=Control.EvalVar<double>(keyName+"BaseGap");
  topGap=Control.EvalVar<double>(keyName+"TopGap");

  portAXStep=Control.EvalDefVar<double>(keyName+"PortAXStep",0.0);
  portAZStep=Control.EvalDefVar<double>(keyName+"PortAZStep",0.0);
  portAWallThick=Control.EvalPair<double>(keyName+"PortAWallThick",
					  keyName+"PortWallThick");
  portATubeLength=Control.EvalPair<double>(keyName+"PortATubeLength",
					   keyName+"PortTubeLength");
  portATubeRadius=Control.EvalPair<double>(keyName+"PortATubeRadius",
					   keyName+"PortTubeRadius");

  portBXStep=Control.EvalDefVar<double>(keyName+"PortBXStep",0.0);
  portBZStep=Control.EvalDefVar<double>(keyName+"PortBZStep",0.0);

  portBWallThick=Control.EvalPair<double>(keyName+"PortBWallThick",
					  keyName+"PortWallThick");
  portBTubeLength=Control.EvalPair<double>(keyName+"PortBTubeLength",
					   keyName+"PortTubeLength");
  portBTubeRadius=Control.EvalPair<double>(keyName+"PortBTubeRadius",
					   keyName+"PortTubeRadius");

  
  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");
  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");

  if (flangeARadius<(portAWallThick+portATubeRadius+Geometry::zeroTol))
    throw ColErr::SizeError<double>
      (flangeARadius,portAWallThick+portATubeRadius,
       "Flange to small for "+keyName+" port A");

  if (flangeBRadius<(portBWallThick+portBTubeRadius+Geometry::zeroTol))
    throw ColErr::SizeError<double>
      (flangeBRadius,portBWallThick+portBTubeRadius,
       "Flange to small for "+keyName+" port B");

  
  const size_t NPorts=Control.EvalVar<size_t>(keyName+"NPorts");
  const std::string portBase=keyName+"Port";
  double L,R,W,FR,FT,PT;
  int PMat;
  int OFlag;
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName=portBase+std::to_string(i);
      constructSystem::portItem windowPort(portName);
      const Geometry::Vec3D Centre=
	Control.EvalVar<Geometry::Vec3D>(portName+"Centre");
      const Geometry::Vec3D Axis=
	Control.EvalPair<Geometry::Vec3D>(portName,portBase,"Axis");
      
      L=Control.EvalPair<double>(portName,portBase,"Length");
      R=Control.EvalPair<double>(portName,portBase,"Radius");
      W=Control.EvalPair<double>(portName,portBase,"Wall");
      FR=Control.EvalPair<double>(portName,portBase,"FlangeRadius");
      FT=Control.EvalPair<double>(portName,portBase,"FlangeLength");
      PT=Control.EvalDefPair<double>(portName,portBase,"PlateThick",0.0);
      PMat=ModelSupport::EvalDefMat<int>
	(Control,portName+"PlateMat",portBase+"PlateMat",wallMat);

      OFlag=Control.EvalDefVar<int>(portName+"OuterVoid",0);

      if (OFlag) windowPort.setWrapVolume();
      windowPort.setMain(L,R,W);
      windowPort.setFlange(FR,FT);
      windowPort.setCoverPlate(PT,PMat);
      windowPort.setMaterial(voidMat,wallMat);

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(windowPort);
    }					    

  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
TankMonoVessel::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  Origin+=Y*(portATubeLength+wallThick+voidRadius);
  return;
}

void
TankMonoVessel::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!isActive("Front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
	      Origin-Y*(portATubeLength+wallThick+voidRadius),Y);
      setCutSurf("Front",SMap.realSurf(buildIndex+101));
    }
  if (!isActive("Back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
	    Origin+Y*(portBTubeLength+wallThick+voidRadius),Y);
      setCutSurf("Back",-SMap.realSurf(buildIndex+102));
    }

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  
  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin-Z*voidDepth,Z,voidRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin-Z*voidDepth,Z,voidRadius+wallThick);

  // Top plate
  const double xDrop=
    (voidRadius*voidRadius-topGap*topGap)/(2.0*topGap);
  const double topRadius=
    (voidRadius*voidRadius+topGap*topGap)/(2.0*topGap);
  const Geometry::Vec3D topCent=Origin-Z*(xDrop-voidHeight);
  ModelSupport::buildSphere(SMap,buildIndex+108,topCent,topRadius);
  ModelSupport::buildSphere(SMap,buildIndex+118,topCent,topRadius+wallThick);
  
  return;
}

void
TankMonoVessel::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("TankMonoVessel","createObjects");

  std::string Out;

  const std::string FPortStr(ExternalCut::getRuleStr("Front"));
  const std::string BPortStr(ExternalCut::getRuleStr("Back"));
  
  // Main Void 
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -7 ");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  addOuterSurf(Out);

  return;
}

void
TankMonoVessel::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createLinks");

  const Geometry::Vec3D ACentre(Origin+X*portAXStep+Z*portAZStep);
  const Geometry::Vec3D BCentre(Origin+X*portBXStep+Z*portBZStep);

  ExternalCut::createLink("Front",*this,0,ACentre,Y);
  ExternalCut::createLink("Back",*this,1,BCentre,Y); 
  
  return;
}

void
TankMonoVessel::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("TankMonoVessel","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
