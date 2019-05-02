/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/PreBendPipe.cxx
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

#include "PreBendPipe.h"

namespace xraySystem
{

PreBendPipe::PreBendPipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(1,"photonExit");
  FixedComp::nameSideIndex(2,"electronExit");
}


PreBendPipe::~PreBendPipe() 
  /*!
    Destructor
  */
{}

void
PreBendPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("PreBendPipe","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  straightLength=Control.EvalVar<double>(keyName+"StraightLength");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");
  electronAngle=Control.EvalVar<double>(keyName+"ElectronAngle");
  
  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}

void
PreBendPipe::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("PreBendPipe","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
PreBendPipe::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PreBendPipe","createSurface");

  // Do outer surfaces (vacuum ports)

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  const Geometry::Vec3D StrOrg(Origin+Y*straightLength);
  
  ModelSupport::buildPlane(SMap,buildIndex+101,StrOrg,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  
  // mid layer divider
  ModelSupport::buildPlane(SMap,buildIndex+10,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*radius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*radius,Z);


  // wall
    
  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(radius+wallThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(radius+wallThick),Z);

  // build extention plane system
  
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(-electronAngle,Z);

  const Geometry::Vec3D XElec=QR.makeRotate(X);
  const Geometry::Vec3D YElec=QR.makeRotate(Y);
  
  const Geometry::Vec3D cylCentre=StrOrg+X*(radius+electronRadius);

  // divider plane
  ModelSupport::buildPlane
    (SMap,buildIndex+110,cylCentre-X*(electronRadius/2.0),X);
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,cylCentre,Z,electronRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+117,cylCentre+X*wallThick,Z,electronRadius);

  // END plane
  const double xDisp=(1.0-cos(M_PI*electronAngle/180.0))*electronRadius;
  const double yDisp=sin(M_PI*electronAngle/180.0)*electronRadius;
  cylEnd=StrOrg+X*xDisp+Y*yDisp;
  elecAxis=YElec;
  
  ModelSupport::buildPlane(SMap,buildIndex+102,cylEnd,YElec);
  SurfMap::setSurf("electronCut",SMap.realSurf(buildIndex+102));
  
  // Exit cylinder [ levels]
  ModelSupport::buildPlane(SMap,buildIndex+210,cylEnd,XElec);
  ModelSupport::buildCylinder
    (SMap,buildIndex+207,cylEnd,YElec,radius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,cylEnd,YElec,radius+wallThick);
  
  
  // flange cylinder/planes
  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,flangeARadius);
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin+Y*flangeALength,Y);

  ModelSupport::buildCylinder
    (SMap,buildIndex+2007,Origin,Y,flangeBRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+2001,Origin+Y*(length-flangeBLength),Y);


  return;
}

void
PreBendPipe::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreBendPipe","createObjects");

  const std::string frontSurf(ExternalCut::getRuleStr("front"));
  
  std::string Out;

  // cylinder half
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -101 -7  ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  // photon full
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -2 -7 -10 ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out);

  // electron  curve
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 101 -102 107 105 -106 -110 ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out);

  // electron straight
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 102 -2 (-207:-210) 105 -106  -110 ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out);


  // WALLS:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -101 7 -17 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out+frontSurf);

  // photon full
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-10 101 -2 -17 7 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out);

  // electron  curve
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 101 -102 117 115 -116 (-107:-105:106) -110");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out);

    // electron straight
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 102 -2 (-217:-210) 115 -116 ((210 207):-105:106) ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out);
    
  // front flange
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1001 -1007 17 ");
  makeCell("frontFlange",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1001 1007 -2007");
  makeCell("frontFlangeVoid",System,cellIndex++,0,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -101 17 -2007");
  makeCell("frontOuterVoid",System,cellIndex++,0,0.0,Out);

  // back flange
  Out=ModelSupport::getComposite  //(217 210)
    (SMap,buildIndex,"  2001 -2 -2007 (( 17 -10 ) : (217 210) : -115: 116) ");
  makeCell("backFlange",System,cellIndex++,flangeMat,0.0,Out);


  //  mid outer void
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 -2007 (( 17 -10 ) : -117:-115:116) -110");
  makeCell("outerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 102 -2001 -2007 (( 17 -10 ) : (210 217) :-115:116) ");
  makeCell("outerVoid",System,cellIndex++,0,0.0,Out);



  Out=ModelSupport::getComposite(SMap,buildIndex,"-2 -2007 ");
  addOuterSurf(Out+frontSurf);

  return;
}

void 
PreBendPipe::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("PreBendPipe","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);

  // photon/electron
  setConnect(1,Origin+Y*length,Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+2));

  // electron surface is intersect from 102 normal into surface 2
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+2));
  FixedComp::setLineConnect(2,cylEnd,elecAxis);


  
  return;
}

void
PreBendPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("PreBendPipe","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
