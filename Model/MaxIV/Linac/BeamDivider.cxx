/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/BeamDivider.cxx
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "BeamDivider.h"

namespace tdcSystem
{

BeamDivider::BeamDivider(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup
  ("Box","Main","Exit","FlangeB","FlangeE","FlangeA"),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


BeamDivider::~BeamDivider() 
  /*!
    Destructor
  */
{}

void
BeamDivider::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("BeamDivider","populate");

  FixedRotate::populate(Control);

  boxLength=Control.EvalVar<double>(keyName+"BoxLength");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  mainWidth=Control.EvalVar<double>(keyName+"MainWidth");
  exitWidth=Control.EvalVar<double>(keyName+"ExitWidth");
  height=Control.EvalVar<double>(keyName+"Height");

  mainXStep=Control.EvalVar<double>(keyName+"MainXStep");
  exitXStep=Control.EvalVar<double>(keyName+"ExitXStep");
  exitAngle=Control.EvalVar<double>(keyName+"ExitAngle");
  
  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  mainRadius=Control.EvalVar<double>(keyName+"MainRadius");
  mainThick=Control.EvalVar<double>(keyName+"MainThick");

  exitLength=Control.EvalVar<double>(keyName+"ExitLength");
  exitRadius=Control.EvalVar<double>(keyName+"ExitRadius");
  exitThick=Control.EvalVar<double>(keyName+"ExitThick");

  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");
  flangeERadius=Control.EvalVar<double>(keyName+"FlangeERadius");
  flangeELength=Control.EvalVar<double>(keyName+"FlangeELength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");


  return;
}


void
BeamDivider::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamDivider","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(boxLength+mainLength),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  
  const double ang(M_PI*exitAngle/180.0);
  const Geometry::Vec3D exitOrg(Origin+X*exitXStep);
  const Geometry::Vec3D RNorm(X*cos(ang)-Y*sin(ang));
  const Geometry::Vec3D RAxis(X*sin(ang)+Y*cos(ang));
  
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*boxLength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+112,Origin+Y*(wallThick+boxLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*mainWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,exitOrg+X*exitWidth,RNorm);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(mainWidth+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,exitOrg+X*exitWidth+RNorm*wallThick,RNorm);  
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(wallThick+height/2.0),Z);

  // end flange
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeARadius);
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin+Y*flangeALength,Y);

  // Main [400]
  const Geometry::Vec3D mainOrg(Origin+X*mainXStep);
  ModelSupport::buildCylinder(SMap,buildIndex+407,mainOrg,Y,mainRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,mainOrg,Y,
			      mainThick+mainRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+427,mainOrg,Y,flangeBRadius);
  ModelSupport::buildPlane(SMap,buildIndex+422,
			   mainOrg+Y*(boxLength+mainLength-flangeBLength),Y);
			   
  // Exit [500]
  ModelSupport::buildPlane(SMap,buildIndex+502,
			   exitOrg+RAxis*(boxLength+exitLength),RAxis);
  ModelSupport::buildCylinder
    (SMap,buildIndex+507,exitOrg,RAxis,exitRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+517,exitOrg,RAxis,exitThick+exitRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+527,exitOrg,RAxis,flangeERadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+522,
     exitOrg+RAxis*(boxLength+exitLength-flangeELength),
     RAxis);
  
  return;
}

void
BeamDivider::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamDivider","createObjects");

  std::string Out;

  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void [box]
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 -102 ");
  makeCell("BoxVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);

  // side wall
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 13 -14 15 -16  (-3 : 4 : -5 : 6) -102 ");
  makeCell("BoxWall",System,cellIndex++,wallMat,0.0,Out+frontStr);
  
  // front flange
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -207  (-13 : 14 : -15 : 16) -201 ");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,Out+frontStr);


  // box plate
  Out=ModelSupport::getComposite
     (SMap,buildIndex," 102 -112 13 -14 15 -16 517 417 ");
   makeCell("BoxPlate",System,cellIndex++,wallMat,0.0,Out);

    // side wall
  // Out=ModelSupport::getComposite
  //   (SMap,buildIndex," 201 -207  (-13 : 14 : -15 : 16) -112 ");
  // makeCell("BoxOuter",System,cellIndex++,0,0.0,Out);

  // Main Tube
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -407 ");
  makeCell("MainVoid",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -417 407");
  makeCell("MainWall",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 422 417 -427");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,Out+backStr);

  // Out=ModelSupport::getComposite(SMap,buildIndex," 112 -422 417 -427");
  // makeCell("MainOuter",System,cellIndex++,0,0.0,Out+backStr);

  // exit pipe
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -507 -502");
  makeCell("ExitVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -517 507 -502");
  makeCell("ExitWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 522 517 -527 -502");
  makeCell("FlangeE",System,cellIndex++,wallMat,0.0,Out);

  // Out=ModelSupport::getComposite(SMap,buildIndex," 112 -522 517 -527 -502");
  // makeCell("ExitOuter",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -112 13 -14 15 -16 ");
  addOuterSurf("Box",Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"112 -417 ");
  addOuterSurf("Main",Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"112 -517 -522");
  addOuterSurf("Exit",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-207 -201");
  addOuterSurf("FlangeA",Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-527 -502 522");
  addOuterSurf("FlangeE",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-427  422");
  addOuterSurf("FlangeB",Out+backStr);
  
  return;
}

void 
BeamDivider::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("BeamDivider","createLinks");

  const Geometry::Vec3D mainOrg(Origin+X*mainXStep);
  const Geometry::Vec3D exitOrg(Origin+X*exitXStep);
  const double ang(M_PI*exitAngle/180.0);
  const Geometry::Vec3D RAxis(X*sin(ang)+Y*cos(ang));
  
  //front and back
  ExternalCut::createLink("front",*this,0,Origin,Y);  
  ExternalCut::createLink("back",*this,1,mainOrg,Y);

  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+502));
  FixedComp::setLineConnect(2,exitOrg,RAxis);

  FixedComp::nameSideIndex(2,"exit");
  return;
}

void
BeamDivider::createAll(Simulation& System,
    	               const attachSystem::FixedComp& FC,
	               const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("BeamDivider","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
