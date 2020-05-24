/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/EBeamStop.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "surfDIter.h"
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
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "EBeamStop.h"

namespace tdcSystem
{

EBeamStop::EBeamStop(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


EBeamStop::~EBeamStop()
  /*!
    Destructor
  */
{}

void
EBeamStop::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EBeamStop","populate");

  FixedOffset::populate(Control);

  closedFlag=static_cast<bool>(Control.EvalVar<size_t>(keyName+"Closed"));
  
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  baseFlangeExtra=Control.EvalVar<double>(keyName+"BaseFlangeExtra");
  baseFlangeThick=Control.EvalVar<double>(keyName+"BaseFlangeThick");

  portLength=Control.EvalVar<double>(keyName+"PortLength");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portWallThick=Control.EvalVar<double>(keyName+"PortWallThick");
  portFlangeRadius=Control.EvalVar<double>(keyName+"PortFlangeRadius");
  portFlangeLength=Control.EvalVar<double>(keyName+"PortFlangeLength");

  stopRadius=Control.EvalVar<double>(keyName+"StopRadius");
  stopLength=Control.EvalVar<double>(keyName+"StopLength");
  stopZLift=Control.EvalVar<double>(keyName+"StopZLift");

  const size_t NS(Control.EvalVar<size_t>(keyName+"StopNLen"));
  stopLen.resize(NS);
  stopMat.resize(NS+1);
  for(size_t i=0;i<NS;i++)
    {
      const std::string nStr=std::to_string(i);
      stopLen[i]=Control.EvalVar<double>(keyName+"StopLen"+nStr);
      stopMat[i]=ModelSupport::EvalMat<int>(Control,keyName+"StopMat"+nStr);
    }
  stopMat[NS]=
    ModelSupport::EvalMat<int>(Control,keyName+"StopMat"+std::to_string(NS));

  supportConeLen=Control.EvalVar<double>(keyName+"SupportConeLen");
  supportConeRadius=Control.EvalVar<double>(keyName+"SupportConeRadius");
  supportHoleRadius=Control.EvalVar<double>(keyName+"SupportHoleRadius");
  supportRadius=Control.EvalVar<double>(keyName+"SupportRadius");
  
  stopPortYStep=Control.EvalVar<double>(keyName+"StopPortYStep");
  stopPortRadius=Control.EvalVar<double>(keyName+"StopPortRadius");
  stopPortLength=Control.EvalVar<double>(keyName+"StopPortLength");
  stopPortWallThick=Control.EvalVar<double>(keyName+"StopPortWallThick");
  stopPortFlangeRadius=Control.EvalVar<double>(keyName+"StopPortFlangeRadius");
  stopPortFlangeLength=Control.EvalVar<double>(keyName+"StopPortFlangeLength");
  stopPortPlateThick=Control.EvalVar<double>(keyName+"StopPortPlateThick");

  ionPortYStep=Control.EvalVar<double>(keyName+"IonPortYStep");
  ionPortRadius=Control.EvalVar<double>(keyName+"IonPortRadius");
  ionPortLength=Control.EvalVar<double>(keyName+"IonPortLength");
  ionPortWallThick=Control.EvalVar<double>(keyName+"IonPortWallThick");
  ionPortFlangeRadius=Control.EvalVar<double>(keyName+"IonPortFlangeRadius");
  ionPortFlangeLength=Control.EvalVar<double>(keyName+"IonPortFlangeLength");
  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}


void
EBeamStop::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EBeamStop","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }


  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*height,Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+111,Origin-Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+112,Origin+Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+113,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+114,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(baseThick+depth),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(wallThick+height),Z);

  // Flange at base:
  ModelSupport::buildPlane
    (SMap,buildIndex+121,Origin-Y*(baseFlangeExtra+wallThick+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+122,Origin+Y*(baseFlangeExtra+wallThick+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+123,Origin-X*(baseFlangeExtra+wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+124,Origin+X*(baseFlangeExtra+wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+125,Origin-Z*(depth-baseFlangeThick),Z);

  // ports end at Surf : front/back
  // Ports (front/back align)
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,portRadius+portWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,Origin,Y,portFlangeRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+201,Origin-Y*(portLength+length/2.0-portFlangeLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(portLength+length/2.0-portFlangeLength),Y);
  

  // Ion Pump port:
  const Geometry::Vec3D ionOrg(Origin+Y*ionPortYStep-Z*depth);
  ModelSupport::buildCylinder(SMap,buildIndex+307,ionOrg,Z,ionPortRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,ionOrg,Z,ionPortRadius+ionPortWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+327,ionOrg,Z,ionPortFlangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+305,ionOrg-Z*ionPortLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+315,ionOrg-Z*(ionPortLength-ionPortFlangeLength),Z);

  // Stop Support port:
  const Geometry::Vec3D stopOrg(Origin+Y*stopPortYStep-Z*depth);
  ModelSupport::buildCylinder(SMap,buildIndex+407,stopOrg,Z,stopPortRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+417,stopOrg,Z,stopPortRadius+stopPortWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+427,stopOrg,Z,stopPortFlangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+405,stopOrg-Z*stopPortLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+415,stopOrg-Z*(stopPortLength-stopPortFlangeLength),Z);

  // Main stop cylinder
  const Geometry::Vec3D tOrg=(closedFlag) ? Origin : Origin+Z*stopZLift;

  ModelSupport::buildCylinder(SMap,buildIndex+507,tOrg,Y,stopRadius);
  ModelSupport::buildPlane(SMap,buildIndex+501,tOrg-Y*(stopLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+502,tOrg+Y*(stopLength/2.0),Y);

  const size_t NS(stopLen.size());
  int BI(buildIndex+500);
  for(size_t i=0;i<NS;i++)
    {
      ModelSupport::buildPlane(SMap,BI+11,tOrg+Y*stopLen[i],Y);
      BI+=10;
    }

  // Support Tube:



  return;
}

void
EBeamStop::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EBeamStop","createObjects");

  std::string Out;

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 113 -114 105 -116 (-103:104:106) ");
  makeCell("SideWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 111 -101 113 -114 115 -116 207 ");
  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -207 ");
  makeCell("PortAVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," -111 -217 207 ");
  makeCell("PortAWall",System,cellIndex++,wallMat,0.0,Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," -201 -227 217 ");
  makeCell("PortAFlange",System,cellIndex++,flangeMat,0.0,Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -227 217 -111 ");
  makeCell("PortAOuter",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -112 102 113 -114 115 -116 207 ");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -207 ");
  makeCell("PortBVoid",System,cellIndex++,voidMat,0.0,Out+backStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," 112 -217 207 ");
  makeCell("PortBWall",System,cellIndex++,wallMat,0.0,Out+backStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," 202 -227 217 ");
  makeCell("PortBFlange",System,cellIndex++,flangeMat,0.0,Out+backStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," -202 -227 217 112 ");
  makeCell("PortBOuter",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 121 -122 123 -124 115 -105 307 407 ");
  makeCell("Base",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 121 -122 123 -124 105 -125 (-111:112:-113:114)");
  makeCell("BaseFlange",System,cellIndex++,wallMat,0.0,Out);

  // ion pump
  Out=ModelSupport::getComposite(SMap,buildIndex," -105 -307  305");
  makeCell("IonVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -115 307 -317  305");
  makeCell("IonWall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 305 -315 -327 317 ");
  makeCell("IonFlange",System,cellIndex++,flangeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 315 -115 317 -327  ");
  makeCell("IonOuter",System,cellIndex++,outerMat,0.0,Out);

  // stopper
  Out=ModelSupport::getComposite(SMap,buildIndex," -105 -407  405");
  makeCell("StopVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -115 407 -417  405");
  makeCell("StopWall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 405 -415 -427 417 ");
  makeCell("StopFlange",System,cellIndex++,flangeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 415 -115 417 -427  ");
  makeCell("StopOuter",System,cellIndex++,outerMat,0.0,Out);

  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 111 -112 123 -124 125 -116 (-111:112:-113:114)");
  makeCell("SideOuter",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 121 -111 123 -124 125 -116 227 ");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 112 -122 123 -124 125 -116 227 ");
  makeCell("BackOuter",System,cellIndex++,0,0.0,Out);
  
  if (stopPortLength>=ionPortLength)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex," -115 405 121 -122 123 124 427 (327:305) ");
      makeCell("BaseOuter",System,cellIndex++,0,0.0,Out);
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 121 -122 123 -124 405 -116 ");
    }
  else
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex," -115 305 121 -122 123 124 327 (427:405) ");
      makeCell("BaseOuter",System,cellIndex++,0,0.0,Out);
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 121 -122 123 -124 305 -116 ");
    }
  addOuterSurf(Out);

  return;
}

void
EBeamStop::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("EBeamStop","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}

void
EBeamStop::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("EBeamStop","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
