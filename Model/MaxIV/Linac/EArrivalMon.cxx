/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/EArrivalMon.cxx
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

#include "EArrivalMon.h"

namespace tdcSystem
{

EArrivalMon::EArrivalMon(const std::string& Key) :
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


EArrivalMon::~EArrivalMon() 
  /*!
    Destructor
  */
{}

void
EArrivalMon::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EArrivalMon","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  thick=Control.EvalVar<double>(keyName+"Thick");
  faceThick=Control.EvalVar<double>(keyName+"FaceThick");

  frontPipeILen=Control.EvalVar<double>(keyName+"FrontPipeILen");
  frontPipeLen=Control.EvalVar<double>(keyName+"FrontPipeLen");
  frontPipeRadius=Control.EvalVar<double>(keyName+"FrontPipeRadius");
  frontPipeThick=Control.EvalVar<double>(keyName+"FrontPipeThick");

  backPipeILen=Control.EvalVar<double>(keyName+"BackPipeILen");
  backPipeLen=Control.EvalVar<double>(keyName+"BackPipeLen");
  backPipeRadius=Control.EvalVar<double>(keyName+"BackPipeRadius");
  backPipeThick=Control.EvalVar<double>(keyName+"BackPipeThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  windowRotAngle=Control.EvalVar<double>(keyName+"WindowRotAngle");
  windowRadius=Control.EvalVar<double>(keyName+"WindowRadius");
  windowThick=Control.EvalVar<double>(keyName+"WindowThick");
  

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");



  return;
}


void
EArrivalMon::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EArrivalMon","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(frontPipeLen+length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(backPipeLen+length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // main pipe and surround
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+thick);

  //front /back face
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length/2.0),Y);

  // front back wall
  ModelSupport::buildPlane
    (SMap,buildIndex+21,Origin-Y*(faceThick+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+22,Origin+Y*(faceThick+length/2.0),Y);

  // Flange ends
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(frontPipeLen+length/2.0-flangeLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(backPipeLen+length/2.0-flangeLength),Y);
  // inner flange stops
  ModelSupport::buildPlane
    (SMap,buildIndex+151,Origin-Y*(-frontPipeILen+length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+152,Origin+Y*(-backPipeILen+length/2.0),Y);
  
  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,flangeRadius);
  
  // front exit
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,frontPipeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,frontPipeRadius+frontPipeThick);

  // back flange
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,backPipeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,backPipeRadius+backPipeThick);

  const Geometry::Vec3D winAxis(X*cos(M_PI*windowRotAngle/180.0)+
				Z*sin(M_PI*windowRotAngle/180.0));

  
  const Geometry::Vec3D midWindow(Origin+winAxis*(radius+thick/2.0));
  // mid divider
  ModelSupport::buildPlane(SMap,buildIndex+500,Origin,winAxis);
  ModelSupport::buildPlane
    (SMap,buildIndex+501,midWindow-winAxis*(windowThick/2.0),winAxis);
  ModelSupport::buildPlane
    (SMap,buildIndex+502,midWindow+winAxis*(windowThick/2.0),winAxis);
  
  ModelSupport::buildCylinder(SMap,buildIndex+507,Origin,winAxis,windowRadius);
  // cut plates

  return;
}

void
EArrivalMon::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EArrivalMon","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");
  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 11 -12 -7 (117:151) (217:-152) ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 7 -17 (507:-500)");
  makeCell("Wall",System,cellIndex++,mainMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 21 117 -17 ");
  makeCell("FPlate",System,cellIndex++,mainMat,0.0,Out);

  // back plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -22 217 -17 ");
  makeCell("BPlate",System,cellIndex++,mainMat,0.0,Out);
  
  // front Main pipe
  Out=ModelSupport::getComposite(SMap,buildIndex," -151  -107 ");
  makeCell("FPipeVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -151 107 -117");
  makeCell("FPipe",System,cellIndex++,mainMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 117 -307 ");
  makeCell("FFlange",System,cellIndex++,mainMat,0.0,Out+frontStr);


  // back Main pipe
  Out=ModelSupport::getComposite(SMap,buildIndex," 152 -207 ");
  makeCell("BPipeVoid",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 152 207 -217");
  makeCell("BPipe",System,cellIndex++,mainMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 217 -307 ");
  makeCell("BFlange",System,cellIndex++,mainMat,0.0,Out+backStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -21 101 -307 117 ");
  makeCell("AFlangeOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 -102 -307 117 ");
  makeCell("BFlangeOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -21 -17 307  ");
  makeCell("AOut",System,cellIndex++,0,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 -17 307  ");
  makeCell("BOut",System,cellIndex++,0,0.0,Out+backStr);

  // window
  Out=ModelSupport::getComposite(SMap,buildIndex,"501 -502 -507 ");
  makeCell("Window",System,cellIndex++,windowMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 500 -501 7 -507 ");
  makeCell("WindowVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 502 -17 -507 ");
  makeCell("WindowOut",System,cellIndex++,0,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -17  ");
  addOuterSurf(Out+frontStr+backStr);
  
  return;
}

void 
EArrivalMon::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("EArrivalMon","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back
      
  return;
}

void
EArrivalMon::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("EArrivalMon","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length+2*frontPipeLen);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
