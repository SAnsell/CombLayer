/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/targetFront.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
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
#include "ContainedComp.h"
#include "BeamWindow.h"
#include "ProtonVoid.h"
#include "TargetBase.h"
#include "targetFront.h"

namespace snsSystem
{

targetFront::targetFront(const std::string& Key) :
  constructSystem::TargetBase(Key,3),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


targetFront::targetFront(const targetFront& A) : 
  constructSystem::TargetBase(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
  populated(A.populated),xOffset(A.xOffset),
  yOffset(A.yOffset),zOffset(A.zOffset),
  mainLength(A.mainLength),mainHeight(A.mainHeight),
  mainWidth(A.mainWidth),mercuryMat(A.mercuryMat),
  mercuryTemp(A.mercuryTemp),mainCell(A.mainCell)
  /*!
    Copy constructor
    \param A :: targetFront to copy
  */
{}

targetFront&
targetFront::operator=(const targetFront& A)
  /*!
    Assignment operator
    \param A :: targetFront to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::TargetBase::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xOffset=A.xOffset;
      yOffset=A.yOffset;
      zOffset=A.zOffset;
      mainLength=A.mainLength;
      mainHeight=A.mainHeight;
      mainWidth=A.mainWidth;
      mercuryMat=A.mercuryMat;
      mercuryTemp=A.mercuryTemp;
      mainCell=A.mainCell;
    }
  return *this;
}

targetFront*
targetFront::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new targetFront(*this);
}

targetFront::~targetFront() 
  /*!
    Destructor
  */
{}

void
targetFront::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("targetFront","populate");

  const FuncDataBase& Control=System.getDataBase();

  xOffset=Control.EvalVar<double>(keyName+"XOffset");
  yOffset=Control.EvalVar<double>(keyName+"YOffset");
  zOffset=Control.EvalVar<double>(keyName+"ZOffset");

  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  mainJoin=Control.EvalVar<double>(keyName+"MainJoin");
  mainHeight=Control.EvalVar<double>(keyName+"MainHeight");
  mainWidth=Control.EvalVar<double>(keyName+"MainWidth");
  totalLength=Control.EvalVar<double>(keyName+"TotalLength");

  innerWall=Control.EvalVar<double>(keyName+"InnerWallThick");

  heliumLength=Control.EvalVar<double>(keyName+"HeLength");
  heliumJoin=Control.EvalVar<double>(keyName+"HeJoin");
  heliumStep=Control.EvalVar<double>(keyName+"HeliumStep");
  heliumXStep=Control.EvalVar<double>(keyName+"HeliumXStep");
  heliumThick=Control.EvalVar<double>(keyName+"HeliumThick");

  pressLength=Control.EvalVar<double>(keyName+"PressLength");
  pressJoin=Control.EvalVar<double>(keyName+"PressJoin");
  pressStep=Control.EvalVar<double>(keyName+"PressStep");
  pressXStep=Control.EvalVar<double>(keyName+"PressXStep");
  pressThick=Control.EvalVar<double>(keyName+"PressThick");

  waterLength=Control.EvalVar<double>(keyName+"WaterLength");
  waterJoin=Control.EvalVar<double>(keyName+"WaterJoin");
  waterStep=Control.EvalVar<double>(keyName+"WaterStep");
  waterXStep=Control.EvalVar<double>(keyName+"WaterXStep");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  waterJoin=Control.EvalVar<double>(keyName+"WaterJoin");

  outerLength=Control.EvalVar<double>(keyName+"OuterLength");
  outerJoin=Control.EvalVar<double>(keyName+"OuterJoin");
  outerStep=Control.EvalVar<double>(keyName+"OuterStep");
  outerXStep=Control.EvalVar<double>(keyName+"OuterXStep");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  hgCutAngle=Control.EvalVar<double>(keyName+"HgCutAngle");
  innerCutAngle=Control.EvalVar<double>(keyName+"InnerCutAngle");
  heCutAngle=Control.EvalVar<double>(keyName+"HeCutAngle");
  pressCutAngle=Control.EvalVar<double>(keyName+"PressCutAngle");
  waterCutAngle=Control.EvalVar<double>(keyName+"WaterCutAngle");
  outerCutAngle=Control.EvalVar<double>(keyName+"OuterCutAngle");

  mercuryMat=ModelSupport::EvalMat<int>(Control,keyName+"MercuryMat");
  innerWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerWallMat");
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat");
  pressMat=ModelSupport::EvalMat<int>(Control,keyName+"PressMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  mercuryTemp=Control.EvalVar<double>(keyName+"MercuryTemp");

  return;
}

void
targetFront::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for origin + xyz
  */
{
  ELog::RegMethod RegA("targetFront","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xOffset,yOffset,zOffset);
  
  return;
}

void
targetFront::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("targetFront","createSurface");

  // This section is a simple rounded corner cylinder system
  // with the corners exactly matching the length/width
  
  // front face 
  ModelSupport::buildCylinder(SMap,protonIndex+7,Origin,X,mainHeight/2.0);
  ModelSupport::buildPlane(SMap,protonIndex+1,Origin,Y);

  // vertical
  ModelSupport::buildPlane(SMap,protonIndex+5,Origin-Z*(mainHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,protonIndex+6,Origin+Z*(mainHeight/2.0),Z);
  
  // Matching cyclinder for side
  ModelSupport::buildCylinder(SMap,protonIndex+17,
			      Origin-X*(mainWidth/2.0),Y,mainHeight/2.0);
  ModelSupport::buildCylinder(SMap,protonIndex+18,
			      Origin+X*(mainWidth/2.0),Y,mainHeight/2.0);
  // Dividing planes
  ModelSupport::buildPlane(SMap,protonIndex+13,Origin-X*(mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,protonIndex+14,Origin+X*(mainWidth/2.0),X);

  // Front corners +  dividing plane [20 offset] 
  ModelSupport::buildSphere(SMap,protonIndex+27,
			    Origin-X*(mainWidth/2.0),mainHeight/2.0);
  ModelSupport::buildSphere(SMap,protonIndex+28,
			    Origin+X*(mainWidth/2.0),mainHeight/2.0);
  // Back plane
  ModelSupport::buildPlane(SMap,protonIndex+2,Origin+Y*mainLength,Y);
  ModelSupport::buildPlane(SMap,protonIndex+52,Origin+Y*totalLength,Y);


  const double XStepAdd[]={0.0,heliumXStep,pressXStep,waterXStep,
			  outerXStep};
  const double StepAdd[]={0.0,heliumStep,pressStep,waterStep,
			  outerStep};
  const double ThickAdd[]={innerWall,heliumThick,pressThick,waterThick,
			   outerThick};
  const double backLen[]={mainLength,heliumLength,pressLength,
			  waterLength,outerLength};

  double Wall(mainHeight/2.0);
  int offset(protonIndex+100);
  double Step(0.0);
  double XStep(0.0);
  for(size_t i=0;i<5;i++)
    {
      Wall+=ThickAdd[i];
      Step+=StepAdd[i];
      XStep+=XStepAdd[i];
      ModelSupport::buildPlane(SMap,offset+5,Origin-Z*Wall,Z);
      ModelSupport::buildPlane(SMap,offset+6,Origin+Z*Wall,Z);
      
      ModelSupport::buildCylinder(SMap,offset+7,Origin+Y*Step,X,Wall);
      
      ModelSupport::buildSphere(SMap,offset+27,
				Origin-X*(mainWidth/2.0+XStep)+Y*Step,Wall);
      ModelSupport::buildSphere(SMap,offset+28,
			    Origin+X*(mainWidth/2.0+XStep)+Y*Step,Wall);  
      ModelSupport::buildCylinder(SMap,offset+17,
				  Origin-X*(mainWidth/2.0+XStep),Y,Wall);
      ModelSupport::buildCylinder(SMap,offset+18,
				  Origin+X*(mainWidth/2.0+XStep),Y,Wall);
      // main Cuts
      ModelSupport::buildPlane(SMap,offset+2,Origin+Y*backLen[i],Y);

      offset+=100;
    }

  const double cutAngle[]=
    {
      hgCutAngle,innerCutAngle,
      heCutAngle,pressCutAngle,
      waterCutAngle,outerCutAngle};
  const double cutLen[]={mainLength,mainLength,heliumLength,
			 pressLength,pressLength,pressLength};
  const double joinLen[]={mainJoin,mainJoin,heliumJoin,
			 pressJoin,waterJoin,outerJoin};


  Wall=mainHeight/2.0;     // reset wall
  offset=1000+protonIndex;
  XStep=mainWidth/2.0;

  // CUTS at 1000
  for(size_t i=0;i<5;i++)
    {
      Wall+=ThickAdd[i];
      XStep+=XStepAdd[i];

      Geometry::Vec3D ZCutUp(Z);
      Geometry::Vec3D ZCutDown(Z);
      Geometry::Quaternion ZRot=
	Geometry::Quaternion::calcQRotDeg(cutAngle[i],X);
      ZRot.rotate(ZCutUp);
      ZRot.invRotate(ZCutDown);
      // He Cut
      ModelSupport::buildPlane(SMap,offset+5,Origin-Z*Wall+Y*cutLen[i],ZCutDown);
      ModelSupport::buildPlane(SMap,offset+6,Origin+Z*Wall+Y*cutLen[i],ZCutUp);
      ModelSupport::buildPlane(SMap,offset+2,Origin+Y*joinLen[i],Y);
      // Cones:
      // To calculate origin of cone, travel along centre of cylinder (17/18) 
      // by -Y*tan(alpha)*Wall  [+/-X*XStep]
      const Geometry::Vec3D ConeOrigin=
	Origin+Y*(cutLen[i]-Wall/tan(cutAngle[i]*M_PI/180.0));

      ModelSupport::buildCone(SMap,offset+17,Origin-X*Wall,Y,cutAngle[i]);
      ModelSupport::buildCone(SMap,offset+18,Origin+X*Wall,Y,cutAngle[i]);
      offset+=100;
    }

  return;
}



void
targetFront::createObjects(Simulation& System)
  /*!
    Builds the target
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("targetFront","createObjects");

  std::string Out;
  mainCell=cellIndex;
  // Main cylinder:
  
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 " (1 : (-27:13) (-28:-14) -7) (-18:-14) (13:-17) -2 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mercuryMat,mercuryTemp,Out));
  
  // inner wall:
  Out=ModelSupport::getComposite(SMap,protonIndex,"13 -14 1 -2 105 -106 (-5 : 6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-117 17 -13 1 -102 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-118 18 14 1 -102 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-107 7 13 -14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out)); 
  Out=ModelSupport::getComposite(SMap,protonIndex,"-127 27 -13  -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-128 28 14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out));

  // helium wall:
  Out=ModelSupport::getComposite(SMap,protonIndex,"13 -14 1 -2 205 -206 (-105 : 106) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-217 117 -13 1 -202 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-218 118 14 1 -202 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-207 107 13 -14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out)); 
  Out=ModelSupport::getComposite(SMap,protonIndex,"-227 127 -13  -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-228 128 14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out));


  // Pressure wall:
  Out=ModelSupport::getComposite(SMap,protonIndex,"13 -14 1 -2 305 -306 (-205 : 206) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-317 217 -13 1 -302 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-318 218 14 1 -302 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-307 207 13 -14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out)); 
  Out=ModelSupport::getComposite(SMap,protonIndex,"-327 227 -13  -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-328 228 14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out));


  // Cool wall:
  Out=ModelSupport::getComposite(SMap,protonIndex,"13 -14 1 -2 405 -406 (-305 : 306) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-417 317 -13 1 -402 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-418 318 14 1 -402 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-407 307 13 -14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out)); 
  Out=ModelSupport::getComposite(SMap,protonIndex,"-427 327 -13  -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-428 328 14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));


  // Outer wall:
  Out=ModelSupport::getComposite(SMap,protonIndex,"13 -14 1 -2 505 -506 (-405 : 406) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-517 417 -13 1 -502 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-518 418 14 1 -502 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-507 407 13 -14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out)); 
  Out=ModelSupport::getComposite(SMap,protonIndex,"-527 427 -13  -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,protonIndex,"-528 428 14 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));

  
  
  Out=ModelSupport::getComposite(SMap,protonIndex," 2 -1002 1005 -1006 ((13 -14) : -1018) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mercuryMat,0.0,Out));
  addOuterSurf(Out);
  //  Out=ModelSupport::getComposite(SMap,protonIndex,
  //				 " (-27:1:23) (-28:1:-24) (-7:1) (-18:-14) (13:-17) -2 5 -6 ");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,innerWallMat,mercuryTemp,Out));

  Out=ModelSupport::getComposite(SMap,protonIndex,
				 " ( 1 : (-527:13) (-528:-14) -507) (-518:-14) (13:-517) -2 505 -506 ");
  addOuterUnionSurf(Out);
  return;
}

void
targetFront::createLinks()
  /*!
    Create all the links
    \todo swap link 0 to be link 2 
  */
{
  // all point out
  FixedComp::setLinkSurf(0,SMap.realSurf(protonIndex+7));
  FixedComp::addLinkSurf(0,-SMap.realSurf(protonIndex+1));

  FixedComp::setLinkSurf(1,SMap.realSurf(protonIndex+2));

  FixedComp::setConnect(0,Origin-Y*mainHeight/2.0,-Y);
  FixedComp::setConnect(1,Origin+Y*mainLength,Y);

  return;
}


void
targetFront::createBeamWindow(Simulation& System)
  /*!
    Create the beamwindow if present
    \param System :: Simulation to build into
  */
{
  ELog::RegMethod RegA("targetFront","createBeamWindow");
  if (PLine->getVoidCell())
    {
      ModelSupport::objectRegister& OR=
	ModelSupport::objectRegister::Instance();
      
      if (!BWPtr)
	{
	  BWPtr=boost::shared_ptr<ts1System::BeamWindow>
	  (new ts1System::BeamWindow("BWindow"));
	  OR.addObject(BWPtr);
	}      
      return;
      BWPtr->addBoundarySurf(PLine->getCompContainer());
      BWPtr->setInsertCell(PLine->getVoidCell());
      BWPtr->createAll(System,*this,0);  // 0 => front face of target
    }
  return;
}


void
targetFront::addProtonLine(Simulation& System,
			 const attachSystem::FixedComp& refFC,
			 const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector edge
    \param index :: Index of the proton cutting surface [6 typically (-7)]
   */
{
  ELog::RegMethod RegA("SNStarget","addProtonLine");
  return;
}

  
void
targetFront::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("targetFront","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}


  
}  // NAMESPACE TMRsystem
