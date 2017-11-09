/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/targetOuter.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BeamWindow.h"
#include "ProtonVoid.h"
#include "TargetBase.h"
#include "targetOuter.h"

namespace snsSystem
{

targetOuter::targetOuter(const std::string& Key) :
  constructSystem::TargetBase(Key,3),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


targetOuter::targetOuter(const targetOuter& A) : 
  constructSystem::TargetBase(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
  mainLength(A.mainLength),mainHeight(A.mainHeight),
  mainWidth(A.mainWidth),mercuryMat(A.mercuryMat),
  mercuryTemp(A.mercuryTemp),mainCell(A.mainCell)
  /*!
    Copy constructor
    \param A :: targetOuter to copy
  */
{}

targetOuter&
targetOuter::operator=(const targetOuter& A)
  /*!
    Assignment operator
    \param A :: targetOuter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::TargetBase::operator=(A);
      cellIndex=A.cellIndex;
      mainLength=A.mainLength;
      mainHeight=A.mainHeight;
      mainWidth=A.mainWidth;
      mercuryMat=A.mercuryMat;
      mercuryTemp=A.mercuryTemp;
      mainCell=A.mainCell;
    }
  return *this;
}

targetOuter*
targetOuter::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new targetOuter(*this);
}

targetOuter::~targetOuter() 
  /*!
    Destructor
  */
{}

void
  targetOuter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database to use
  */
{
  ELog::RegMethod RegA("targetOuter","populate");


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
  outerLift=Control.EvalVar<double>(keyName+"OuterLift");
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
targetOuter::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for origin + xyz
  */
{
  ELog::RegMethod RegA("targetOuter","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyOffset();
  
  return;
}

void
targetOuter::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("targetOuter","createSurface");

  // This section is a simple rounded corner cylinder system
  // with the corners exactly matching the length/width
  
  // front face 
  ModelSupport::buildPlane(SMap,protonIndex+1,Origin,Y);

  // Dividing planes
  ModelSupport::buildPlane(SMap,protonIndex+13,Origin-X*(mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,protonIndex+14,Origin+X*(mainWidth/2.0),X);

  // Back plane
  ModelSupport::buildPlane(SMap,protonIndex+52,Origin+Y*totalLength,Y);


  const double XStepAdd[]={0.0,0.0,heliumXStep,pressXStep,waterXStep,
			  outerXStep};
  const double StepAdd[]={0.0,0.0,heliumStep,pressStep,waterStep,
			  outerStep};
  const double ThickAdd[]={0.0,innerWall,heliumThick,pressThick,waterThick,
			   outerThick};
  const double backLen[]={mainLength,mainLength,heliumLength,pressLength,
			  waterLength,outerLength};

  double Wall(mainHeight/2.0);
  int offset(protonIndex);
  double Step(0.0);
  double XStep(0.0);
  for(size_t i=0;i<6;i++)
    {
      Wall+=ThickAdd[i];
      Step+=StepAdd[i];
      XStep+=XStepAdd[i];
      //vertical
      ModelSupport::buildPlane(SMap,offset+5,Origin-Z*Wall,Z);
      ModelSupport::buildPlane(SMap,offset+6,Origin+Z*Wall,Z);
      
      ModelSupport::buildCylinder(SMap,offset+7,Origin+Y*Step,X,Wall);

      // side cylinders
      // Matching cyclinder for side
      ModelSupport::buildCylinder(SMap,offset+17,
				  Origin-X*(mainWidth/2.0+XStep),Y,Wall);
      ModelSupport::buildCylinder(SMap,offset+18,
				  Origin+X*(mainWidth/2.0+XStep),Y,Wall);

      // corners
      // Front corners +  dividing plane [20 offset] 
      ModelSupport::buildSphere(SMap,offset+27,
				Origin-X*(mainWidth/2.0+XStep)+Y*Step,Wall);
      ModelSupport::buildSphere(SMap,offset+28,
			    Origin+X*(mainWidth/2.0+XStep)+Y*Step,Wall);  
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
  const double Lift[]={0.0,0.0,0.0,0.0,0.0,outerLift};


  Wall=mainHeight/2.0;     // reset wall
  offset=1000+protonIndex;
  XStep=mainWidth/2.0;
  // CUTS at 1000
  for(size_t i=0;i<6;i++)
    {
      Wall+=ThickAdd[i]+Lift[i];
      XStep+=XStepAdd[i];

      Geometry::Vec3D ZCutUp(Z);
      Geometry::Vec3D ZCutDown(Z);
      Geometry::Quaternion ZRot=
	Geometry::Quaternion::calcQRotDeg(cutAngle[i],X);
      ZRot.rotate(ZCutUp);
      ZRot.invRotate(ZCutDown);
      // He Cut
      ModelSupport::buildPlane(SMap,offset+5,
			       Origin-Z*Wall+Y*cutLen[i],ZCutDown);
      ModelSupport::buildPlane(SMap,offset+6,
			       Origin+Z*Wall+Y*cutLen[i],ZCutUp);
      ModelSupport::buildPlane(SMap,offset+2,Origin+Y*joinLen[i],Y);
      // Cones:
      // To calculate origin of cone, travel along centre of cylinder (17/18) 
      // by -Y*tan(alpha)*Wall  [+/-X*XStep]
      const double TA=tan(cutAngle[i]*M_PI/180.0);
      const Geometry::Vec3D ConeOrigin=
	Origin+Y*(cutLen[i]-Wall/TA);

      ModelSupport::buildCone(SMap,offset+17,ConeOrigin-X*XStep,Y,cutAngle[i]);
      ModelSupport::buildCone(SMap,offset+18,ConeOrigin+X*XStep,Y,cutAngle[i]);

      const double WallExt=Wall+(joinLen[i]-cutLen[i])*TA;
      // LONG RUN AT 2000:
      ModelSupport::buildPlane(SMap,offset+1005,Origin-Z*WallExt,Z);
      ModelSupport::buildPlane(SMap,offset+1006,Origin+Z*WallExt,Z);

      // Cylinders must have the cone radius at exit and have correct centre of cone
      ModelSupport::buildCylinder(SMap,offset+1017,
				  ConeOrigin-X*XStep,Y,WallExt);
      ModelSupport::buildCylinder(SMap,offset+1018,
				  ConeOrigin+X*XStep,Y,WallExt);
      // DIVIDERS:
      ModelSupport::buildPlane(SMap,offset+1013,ConeOrigin-X*XStep,X);
      ModelSupport::buildPlane(SMap,offset+1014,ConeOrigin+X*XStep,X);

      offset+=100;
    }
  // Dividing planes


  return;
}

void
targetOuter::createObjects(Simulation& System)
  /*!
    Builds the target
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("targetOuter","createObjects");

  std::string Out;
  mainCell=cellIndex;
  // Main cylinder:
  
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 " (1 : (-27:13) (-28:-14) -7) (-18:-14) (13:-17) -2 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mercuryMat,mercuryTemp,Out));

  const double temp[]={mercuryTemp,mercuryTemp,0,0,0};
  const int mat[]={innerWallMat,heMat,pressMat,waterMat,outerMat};
  int offset(protonIndex);
  for(size_t i=0;i<5;i++)
    {
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"13 -14 1 -102M 105M -106M (-5M : 6M) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"-117M 17M -13 1 -102M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"-118M 18M 14 1 -102M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"-107M 7M 13 -14 -1 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out)); 
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"-127M 27M -13  -1 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"-128M 28M 14 -1 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      offset+=100;
    }

  // Cone sections
  Out=ModelSupport::getComposite(SMap,protonIndex," 2 -1002 1005 -1006 ((13 -14) : -1018 : -1017) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mercuryMat,0.0,Out));
  // joining straight
  Out=ModelSupport::getComposite(SMap,protonIndex," 1002 -52 2005 -2006 ((2013 -2014) : -2018 : -2017) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mercuryMat,0.0,Out));

  offset=protonIndex;
  for(size_t i=0;i<5;i++)
    {
      if (i)
	{
	  Out=ModelSupport::getComposite(SMap,protonIndex,offset,"13 -14 102M -1102M 1105M -1106M (-1005M : 1006M) (-5M:6M)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,protonIndex,offset,"13 -14 102M -1102M 1105M -1106M (-1005M : 1006M) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	}
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"-13 102M -1102M -1117M 1017M 17M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"14 102M -1102M -1118M 1018M 18M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      
      // Straight section
      Out=ModelSupport::getComposite(SMap,protonIndex,offset,"1102M -52 2105M -2106M -2014 2013 (-2005M:2006M) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      if (i)
	{
	  Out=ModelSupport::getComposite(SMap,protonIndex,offset,"1102M -52 2014 -2118M (2018M:1018M) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	  Out=ModelSupport::getComposite(SMap,protonIndex,offset,"1102M -52 -2013 -2117M (2017M:1017M) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,protonIndex,offset,"1102M -52 2014 -2118M 2018M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	  Out=ModelSupport::getComposite(SMap,protonIndex,offset,"1102M -52 -2013 -2117M 2017M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
	}
      offset+=100;
    }

  // SPECIAL SECTION FOR INNER PRESSURE
  Out=ModelSupport::getComposite(SMap,protonIndex,"1302 (1206:-1205) 2205 -2206 2013 -2014 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0,Out));

  // SPECIAL CELL FOR HE
  Out=ModelSupport::getComposite(SMap,protonIndex,"1202 (1106:-1105) 2105 -2106 2013 -2014 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mercuryTemp,Out));
  
  // Out=ModelSupport::getComposite(SMap,protonIndex," 2 -1302 1305 -1306 ((13 -14) : -1318 : -1317) ");
  // addOuterUnionSurf(Out);
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 " ( 1 : (-527:13) (-528:-14) -507) (-518:-14) (13:-517) -502 505 -506 ");
  addOuterUnionSurf(Out);
  Out=ModelSupport::getComposite(SMap,protonIndex," 502 -1302 1505 -1506 ((13 -14) : -1518 : -1517) ");
  addOuterUnionSurf(Out);
  Out=ModelSupport::getComposite(SMap,protonIndex,"1302 2505 -2506 -52 ((2013 -2014) : -2518 : -2517)");
  addOuterUnionSurf(Out);
  return;
}

void
targetOuter::createLinks()
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
targetOuter::addProtonLine(Simulation& System,
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

  PLine->createAll(System,*this,2,refFC,index);
  createBeamWindow(System,1);

  return;
}

  
void
targetOuter::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("targetOuter","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}


  
}  // NAMESPACE snsSystem
