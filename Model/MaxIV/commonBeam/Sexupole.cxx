/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Sexupole.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "Sexupole.h"

namespace xraySystem
{

Sexupole::Sexupole(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  NPole(6),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Sexupole::Sexupole(const std::string& Base,
		   const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  NPole(6),
  baseName(Base)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


Sexupole::~Sexupole() 
  /*!
    Destructor
  */
{}

void
Sexupole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("Sexupole","populate");

  FixedRotate::populate(Control);
  
  length=Control.EvalTail<double>(keyName,baseName,"Length");

  frameRadius=Control.EvalTail<double>(keyName,baseName,"FrameRadius");
  frameThick=Control.EvalTail<double>(keyName,baseName,"FrameThick");

  poleGap=Control.EvalTail<double>(keyName,baseName,"PoleGap");
  poleRadius=Control.EvalTail<double>(keyName,baseName,"PoleRadius");
  poleWidth=Control.EvalTail<double>(keyName,baseName,"PoleWidth");

  coilRadius=Control.EvalTail<double>(keyName,baseName,"CoilRadius");
  coilWidth=Control.EvalTail<double>(keyName,baseName,"CoilWidth");
  
  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat",
				       baseName+"PoleMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat",
				       baseName+"CoilMat");
  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat",
				       baseName+"FrameMat");

  return;
}


void
Sexupole::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Sexupole","createSurface");

  // mid line
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  int CN(buildIndex+1000);
  // Note there are 16 surfaces on the inner part of the pole peice
  double angle(M_PI*poleYAngle/180.0);
  for(size_t i=0;i < 2*NPole; i++)
    {
      const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
      // const Geometry::Vec3D QX=X*cos(M_PI/2.0+angle)+Z*sin(M_PI/2.0+angle);

      // Frame Items:
      ModelSupport::buildPlane(SMap,CN+1,Origin+QR*frameRadius,QR);
      ModelSupport::buildPlane
	(SMap,CN+51,Origin+QR*(frameRadius+frameThick),QR);
      angle+=M_PI/static_cast<double>(NPole);
      CN++;
    }

  // TRIANGLE DIVIDERS:
  CN=buildIndex+500;
  angle=M_PI*(poleYAngle-180.0/static_cast<double>(NPole))/180.0;
  for(size_t i=0;i<NPole;i++)
    {
      const Geometry::Vec3D QX=X*cos(M_PI/2.0+angle)+Z*sin(M_PI/2.0+angle);
      ModelSupport::buildPlane(SMap,CN+1,Origin,QX);
      angle+=2.0*M_PI/static_cast<double>(NPole);
      CN++;
    }  

  // MAIN POLE PIECES:
  angle=M_PI*poleYAngle/180.0;
  CN=buildIndex+2000;
  for(size_t i=0;i<NPole;i++)
    {
      const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
      const Geometry::Vec3D QX=X*cos(M_PI/2.0+angle)+Z*sin(M_PI/2.0+angle);
      // Coil Items:
      ModelSupport::buildPlane(SMap,CN+3,Origin-QX*(coilWidth/2.0),QX);
      ModelSupport::buildPlane(SMap,CN+4,Origin+QX*(coilWidth/2.0),QX);
      ModelSupport::buildPlane(SMap,CN+1,Origin+QR*coilRadius,QR);

      // Pole Items:
      ModelSupport::buildPlane(SMap,CN+103,Origin-QX*(poleWidth/2.0),QX);
      ModelSupport::buildPlane(SMap,CN+104,Origin+QX*(poleWidth/2.0),QX);


      const Geometry::Vec3D CPt(Origin+QR*(poleGap+poleRadius));
      ModelSupport::buildCylinder(SMap,CN+107,CPt,Y,poleRadius);

      ModelSupport::buildPlane(SMap,CN+101,CPt,QR);
      
      angle+=2.0*M_PI/static_cast<double>(NPole);
      CN+=10;
    }  
  return;
}

void
Sexupole::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Sexupole","createObjects");

  std::string Out;
  
  std::string unitStr=" 1M -2M ";
  unitStr+=ModelSupport::getSeqIntersection
    (-51,  -(50+2*static_cast<int>(NPole)),1);
  Out=ModelSupport::getComposite(SMap,buildIndex+1000,buildIndex,unitStr);

  addOuterSurf(Out);

  unitStr=ModelSupport::getSeqUnion(1,static_cast<int>(2*NPole),1);
  Out+=ModelSupport::getComposite(SMap,buildIndex+1000,unitStr);
  makeCell("Frame",System,cellIndex++,frameMat,0.0,Out);

  const std::string ICell=isActive("Inner") ? getRuleStr("Inner") : "";
  /// create triangles
  const std::string FB=ModelSupport::getComposite(SMap,buildIndex,"1 -2");


  std::vector<HeadRule> CoilExclude;
  std::vector<HeadRule> PoleExclude; 
  int BN(buildIndex); // base
  int PN(buildIndex);
  for(size_t i=0; i<NPole;i++)
    {
      const std::string outerCut=
	ModelSupport::getComposite(SMap,BN," -1001 ");

      // Note a : is needed if poleRadius small (-2107 : 2101)");
      Out=ModelSupport::getComposite(SMap,PN,"2103 -2104 -2107 ");
      makeCell("Pole",System,cellIndex++,poleMat,0.0,Out+outerCut+FB);
      PoleExclude.push_back(HeadRule(Out));


      Out=ModelSupport::getComposite
	(SMap,PN," 2003 -2004 2001 (-2103:2104) ");
      makeCell("Coil",System,cellIndex++,coilMat,0.0,Out+outerCut+FB);
      Out=ModelSupport::getComposite(SMap,PN," 2003 -2004 2001 ");
      CoilExclude.push_back(HeadRule(Out));
      BN+=2;
      PN+=10;
    }


  std::string OutA,OutB;
  PN=buildIndex;
  int aOffset(-1);
  int bOffset(0);
  for(size_t i=0;i<NPole;i++)
    {
      const HeadRule triCut=ModelSupport::getHeadRule(SMap,PN,"2001");

      OutA=ModelSupport::getRangeComposite
	(SMap,501,506,bOffset,buildIndex,"501R -502R ");
      OutB=ModelSupport::getRangeComposite
	(SMap,1001,1012,aOffset,buildIndex,"-1001R -1002R -1003R ");

      makeCell("Triangle",System,cellIndex++,0,0.0,OutA+OutB+FB+
	       triCut.display()+
	   CoilExclude[i].complement().display());
      makeCell("InnerTri",System,cellIndex++,0,0.0,OutA+FB+
      	       triCut.complement().display()+
	       PoleExclude[i].complement().display()+ICell);

      aOffset+=2;
      bOffset+=1;
      PN+=10;
    }
  return;
}

void 
Sexupole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Sexupole","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);     
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);     

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
Sexupole::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Sexupole","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
