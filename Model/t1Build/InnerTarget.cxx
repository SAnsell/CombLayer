/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/InnerTarget.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "InnerTarget.h"


namespace ts1System
{

InnerTarget::InnerTarget(const std::string& Key) :
  constructSystem::TargetBase(Key,3),
  tarIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tarIndex+1),frontPlate(0),backPlate(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

InnerTarget::InnerTarget(const InnerTarget& A) : 
  constructSystem::TargetBase(A),
  tarIndex(A.tarIndex),cellIndex(A.cellIndex),
  frontPlate(A.frontPlate),backPlate(A.backPlate),
  mainLength(A.mainLength),coreRadius(A.coreRadius),
  cladThick(A.cladThick),waterThick(A.waterThick),
  pressThick(A.pressThick),voidThick(A.voidThick),
  sphYStep(A.sphYStep),sphRadius(A.sphRadius),wMat(A.wMat),
  taMat(A.taMat),waterMat(A.waterMat),
  targetTemp(A.targetTemp),waterTemp(A.waterTemp),
  externTemp(A.externTemp),frontWater(A.frontWater),
  innerClad(A.innerClad),innerWater(A.innerWater),
  nSphere(A.nSphere),sRadius(A.sRadius),sCent(A.sCent),
  mainCell(A.mainCell),nLayers(A.nLayers),
  mainFrac(A.mainFrac)
  /*!
    Copy constructor
    \param A :: InnerTarget to copy
  */
{}

InnerTarget&
InnerTarget::operator=(const InnerTarget& A)
  /*!
    Assignment operator
    \param A :: InnerTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::TargetBase::operator=(A);
      cellIndex=A.cellIndex;
      frontPlate=A.frontPlate;
      backPlate=A.backPlate;
      mainLength=A.mainLength;
      coreRadius=A.coreRadius;
      cladThick=A.cladThick;
      waterThick=A.waterThick;
      pressThick=A.pressThick;
      voidThick=A.voidThick;
      sphYStep=A.sphYStep;
      sphRadius=A.sphRadius;
      wMat=A.wMat;
      taMat=A.taMat;
      waterMat=A.waterMat;
      targetTemp=A.targetTemp;
      waterTemp=A.waterTemp;
      externTemp=A.externTemp;
      frontWater=A.frontWater;
      innerClad=A.innerClad;
      innerWater=A.innerWater;
      nSphere=A.nSphere;
      sRadius=A.sRadius;
      sCent=A.sCent;
      mainCell=A.mainCell;
      nLayers=A.nLayers;
      mainFrac=A.mainFrac;
    }
  return *this;
}


InnerTarget*
InnerTarget::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new InnerTarget(*this);
}

InnerTarget::~InnerTarget() 
  /*!
    Destructor
  */
{}

void
InnerTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database to use
  */
{
  ELog::RegMethod RegA("InnerTarget","populate");

  attachSystem::FixedOffset::populate(Control);
  
  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  coreRadius=Control.EvalVar<double>(keyName+"CoreRadius");
  cladThick=Control.EvalVar<double>(keyName+"CladThick");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  pressThick=Control.EvalVar<double>(keyName+"PressThick");
  voidThick=Control.EvalVar<double>(keyName+"VoidThick");

  sphYStep=Control.EvalVar<double>(keyName+"SphYStep");
  sphRadius=Control.EvalVar<double>(keyName+"SphRadius");
  coneRadius=Control.EvalVar<double>(keyName+"ConeRadius");

  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  targetTemp=Control.EvalVar<double>(keyName+"TargetTemp");
  waterTemp=Control.EvalVar<double>(keyName+"WaterTemp");
  externTemp=Control.EvalVar<double>(keyName+"ExternTemp");
  
  frontWater=Control.EvalVar<double>(keyName+"FrontWater");
  innerClad=Control.EvalVar<double>(keyName+"InnerClad");
  innerWater=Control.EvalVar<double>(keyName+"InnerWater");
  nSphere=Control.EvalVar<size_t>(keyName+"NSphere");
  Geometry::Vec3D Pt;
  double R;
  for(size_t i=0;i<nSphere;i++)
    {
      R=Control.EvalVar<double>(keyName+StrFunc::makeString("SRadius",i+1));
      Pt=Control.EvalVar<Geometry::Vec3D>
	(keyName+StrFunc::makeString("SCent",i+1));
      sRadius.push_back(R);
      sCent.push_back(Pt); // to be origin XYZ corrected later
    }

  ///  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  return;
}

void
InnerTarget::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for origin + xyz
    \param sideIndex :: sideOffset
  */
{
  ELog::RegMethod RegA("InnerTarget","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  for(Geometry::Vec3D& sPt : sCent)
    sPt= Origin+X* sPt.X()+ Y* (sPt.Y()-sphRadius+sphYStep)+Z*sPt.Z();

  return;
}

void
InnerTarget::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("InnerTarget","createSurface");

  SMap.addMatch(tarIndex+1001,frontPlate);
  SMap.addMatch(tarIndex+1002,backPlate);

  // Inner W
  ModelSupport::buildCylinder(SMap,tarIndex+7,Origin,Y,coreRadius);
  ModelSupport::buildPlane(SMap,tarIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,tarIndex+2,Origin+Y*mainLength,Y);
  ModelSupport::buildSphere(SMap,tarIndex+8,Origin+Y*sphYStep,sphRadius);

  double TThick(cladThick);
  // Ta 
  ModelSupport::buildCylinder(SMap,tarIndex+17,Origin,Y,
			      coreRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+12,
			   Origin+Y*(mainLength+TThick),Y);
  ModelSupport::buildSphere(SMap,tarIndex+18,Origin+Y*sphYStep,
			    sphRadius+TThick);

  // Water
  TThick+=waterThick;
  ModelSupport::buildCylinder(SMap,tarIndex+27,Origin,Y,
			      coreRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+22,
			   Origin+Y*(mainLength+TThick),Y);
  ModelSupport::buildSphere(SMap,tarIndex+28,Origin+Y*sphYStep,
			    sphRadius+TThick);


  // Pressure
  TThick+=pressThick;
  ModelSupport::buildCylinder(SMap,tarIndex+37,Origin,Y,
		      coreRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+32,
		   Origin+Y*(mainLength+TThick),Y);
  ModelSupport::buildSphere(SMap,tarIndex+38,Origin+Y*sphYStep,
			    sphRadius+TThick);

  // void
  TThick+=voidThick;
  ModelSupport::buildCylinder(SMap,tarIndex+47,Origin,Y,
		      coreRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+42,
	   Origin+Y*(mainLength+TThick),Y);
  ModelSupport::buildSphere(SMap,tarIndex+48,Origin+Y*sphYStep,
			    sphRadius+TThick);
  

  // Inner surfaces
  ModelSupport::buildCylinder(SMap,tarIndex+107,Origin,Y,frontWater);
  ModelSupport::buildCylinder(SMap,tarIndex+117,Origin,Y,
			      frontWater+innerClad);

  int TI(tarIndex+200);
  for(size_t i=0;i<nSphere;i++)
    {
      ELog::EM<<"Cent == "<<sCent[i]<<ELog::endDebug;
      ModelSupport::buildPlane(SMap,TI+1,sCent[i],Y);   // divider plane
      ModelSupport::buildSphere(SMap,TI+8,sCent[i],sRadius[i]);
      ModelSupport::buildSphere(SMap,TI+18,sCent[i],sRadius[i]+innerClad);
      ModelSupport::buildSphere(SMap,TI+28,sCent[i],
				sRadius[i]+innerClad+innerWater);
      ModelSupport::buildSphere(SMap,TI+38,sCent[i],
				sRadius[i]+2*innerClad+innerWater);
      TI+=100;
    }
  
  if (nSphere)
    {
      // Calculate Cone:
      // Use similar triangles
      Geometry::Vec3D ConCent(sCent.back());
      double backDist=mainLength-(ConCent-Origin).dotProd(Y);
      // a= br/(R-r) 
      const double frontDist=
	backDist*sRadius.back()/(coneRadius-sRadius.back());
      ConCent-=Y*frontDist;
      const double coneAngle((180.0/M_PI)*atan(sRadius.back()/frontDist));
      ModelSupport::buildCone(SMap,TI+9,ConCent,Y,coneAngle);
      double fStep=innerClad/sin(M_PI*coneAngle/180.0);
      ModelSupport::buildCone(SMap,TI+19,ConCent-Y*fStep,Y,coneAngle);
      fStep+=innerWater/sin(M_PI*coneAngle/180.0);
      ModelSupport::buildCone(SMap,TI+29,ConCent-Y*fStep,Y,coneAngle);
      fStep+=innerClad/sin(M_PI*coneAngle/180.0);
      ModelSupport::buildCone(SMap,TI+39,ConCent-Y*fStep,Y,coneAngle);
    }
  else
    {
      ModelSupport::buildPlane(SMap,tarIndex+201,
			       Origin+Y*(frontWater*2),Y);   // divider plane
      ModelSupport::buildPlane(SMap,tarIndex+228,
			       Origin+Y*(frontWater*2),-Y);   // divider plane
    }
  return;
}

void
InnerTarget::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("InnerTarget","createObjects");
  
  // Tungsten inner core

  std::string Out;
  Out=ModelSupport::getComposite(SMap,tarIndex,"-8 : (1 -2 -7)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));

  // Cladding [with front water divider]
  Out=ModelSupport::getComposite(SMap,tarIndex,"-18 -1 8 117");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"1 -17 -12 (7:2) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  // Water
  Out=ModelSupport::getComposite(SMap,tarIndex,"-28 -1 18");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"1 -27 -22 (17:12) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  // TA outer [pressure]
  Out=ModelSupport::getComposite(SMap,tarIndex,"-38 -1 28");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"1 -37 -32 (27:22) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  // void 
  Out=ModelSupport::getComposite(SMap,tarIndex,"-48 -1 38");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"1 -47 -42 (37:32) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // Inner objects:
  Out=ModelSupport::getComposite(SMap,tarIndex,"-18 -107 -201 228");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tarIndex,"-18 -117 107 -201 228");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  // First half sphere
  Out=ModelSupport::getComposite(SMap,tarIndex,"-208 -201");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"-218 208 -201");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"-228 218 -201");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"-238 228 -201 117 ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  // Second  sphere
  int TI(tarIndex+200);
  for(size_t i=1;i<nSphere;i++)
    {
      Out=ModelSupport::getComposite(SMap,TI,"(-8:-108) 1 -101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,TI,"(-18:-118) (8 108) 1 -101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,TI,"(-28:-128) (18 118) 1 -101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,TI,"(-38:-138) (28 128) 1 -101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
      TI+=100;
    }
  // Cone:
  Out=ModelSupport::getComposite(SMap,TI,tarIndex,"1 -109 -2M -7M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,TI,tarIndex,"1 -2M 109 -119 -7M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,TI,tarIndex,"1 -2M 119 -129 -7M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,TI,tarIndex,"1 -2M 129 -139 -7M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,tarIndex," (117 : 201) ");
  TI=tarIndex+200;
  for(size_t i=1;i<nSphere;i++)
    {
      Out+=ModelSupport::getComposite(SMap,TI," 38 ");
      TI+=100;
    }

  // Outer cone
  if (nSphere)
    {
      Out+=ModelSupport::getComposite(SMap,TI," (38:1) ");
      Out+=ModelSupport::getComposite(SMap,TI," (139 : -1) ");
    }
  MonteCarlo::Qhull* IPtr=System.findQhull(tarIndex+1); 
  IPtr->addSurfString(Out);

  
  // Set EXCLUDE:
  Out=ModelSupport::getComposite(SMap,tarIndex,"-48 : (1 -42 -47)");
  addOuterSurf(Out);
  addBoundarySurf(SMap.realSurf(tarIndex+48));    
  return;
}

void
InnerTarget::createLinks()
  /*!
    Create all the links
    \todo swap link 0 to be link 2 
  */
{
  ELog::RegMethod RegA("InnerTarget","createLinks");

  // all point out
  FixedComp::setLinkSurf(0,SMap.realSurf(tarIndex+48));
  FixedComp::addLinkSurf(0,-SMap.realSurf(tarIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(tarIndex+42));
  FixedComp::setLinkSurf(2,SMap.realSurf(tarIndex+47));

  const double TThick(cladThick+waterThick+pressThick+
		      voidThick);
		      
  FixedComp::setConnect(0,Origin-Y*(sphRadius-sphYStep+TThick),-Y);
  FixedComp::setConnect(1,Origin+Y*(mainLength+TThick),Y);
  FixedComp::setConnect(2,Origin+Z*(coreRadius+TThick),Z);

  return;
}



void
InnerTarget::addProtonLine(Simulation& System,
			 const attachSystem::FixedComp& refFC,
			 const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector edge
    \param index :: Index of the proton cutting surface [6 typically (-7)]
   */
{
  ELog::RegMethod RegA("InnerTarget","addProtonLine");

  // 0 ::  front fact of target
  PLine->createAll(System,*this,0,refFC,index);
  createBeamWindow(System,1);
  System.populateCells();
  System.createObjSurfMap();
  return;
}

  
void
InnerTarget::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("InnerTarget","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);


  return;
}


  
}  // NAMESPACE ts1System
