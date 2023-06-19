/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Octupole.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

#include "Octupole.h"

namespace xraySystem
{

Octupole::Octupole(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Octupole::Octupole(std::string  Base,
		   const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(std::move(Base))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


Octupole::~Octupole() 
  /*!
    Destructor
  */
{}

void
Octupole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("Octupole","populate");

  FixedRotate::populate(Control);
  
  length=Control.EvalTail<double>(keyName,baseName,"Length");

  frameRadius=Control.EvalTail<double>(keyName,baseName,"FrameRadius");
  frameThick=Control.EvalTail<double>(keyName,baseName,"FrameThick");

  poleYAngle=Control.EvalTail<double>(keyName,baseName,"PoleYAngle");
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
Octupole::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Octupole","createSurface");

  // mid line
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);


  int CN(buildIndex+1000);
  // Note there are 16 surfaces on the inner part of the pole peice
  double angle(M_PI*poleYAngle/180.0);
  for(size_t i=0;i<16;i++)
    {
      const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
      // const Geometry::Vec3D QX=X*cos(M_PI/2.0+angle)+Z*sin(M_PI/2.0+angle);

      // Frame Items:
      ModelSupport::buildPlane(SMap,CN+1,Origin+QR*frameRadius,QR);
      ModelSupport::buildPlane
	(SMap,CN+51,Origin+QR*(frameRadius+frameThick),QR);
      angle+=M_PI/8.0;
      CN++;
    }

  // TRIANGLE DIVIDERS:
  CN=buildIndex+500;
  angle=M_PI*(poleYAngle-22.5)/180.0;
  for(size_t i=0;i<8;i++)
    {
      const Geometry::Vec3D QX=X*cos(M_PI/2.0+angle)+Z*sin(M_PI/2.0+angle);
      ModelSupport::buildPlane(SMap,CN+1,Origin,QX);
      angle+=M_PI/4.0;
      CN++;
    }  

  // MAIN POLE PIECES:
  angle=M_PI*poleYAngle/180.0;
  CN=buildIndex+2000;
  for(size_t i=0;i<8;i++)
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
      
      angle+=M_PI/4.0;
      CN+=10;
    }
  if (!isActive("Inner"))
    ModelSupport::buildCylinder(SMap,buildIndex+5007,Origin,Y,poleGap/10.0);

  return;
}

void
Octupole::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Octupole","createObjects");

  HeadRule HR;
  
  // Outer steel
  HR=ModelSupport::getHeadRule(SMap,buildIndex+1000,buildIndex,
     "1M -2M -51 -52 -53 -54 -55 -56 -57 -58 -59 -60 -61 -62 -63 -64 -65 -66");
  addOuterSurf(HR);
  
  HR*=ModelSupport::getHeadRule(SMap,buildIndex+1000,
			"(1:2:3:4:5:6:7:8:9:10:11:12:13:14:15:16)");

  makeCell("Frame",System,cellIndex++,frameMat,0.0,HR);

  const HeadRule fbHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2");
  if (!isActive("Inner"))
    {
      setCutSurf("Inner",SMap.realSurf(buildIndex+5007));
      HR=HeadRule(SMap,buildIndex,-5007);
      makeCell("Inner",System,cellIndex++,0,0.0,HR*fbHR);
    }

  const HeadRule ICellHR=getRule("Inner");
  /// create triangles

  std::vector<HeadRule> PoleExclude; 
  int BN(buildIndex); // base
  int PN(buildIndex);
  for(size_t i=0;i<8;i++)
    {
      const HeadRule outerCutHR(SMap,BN,-1001);

      // Note a : is needed if poleRadius small (-2107 : 2101)");
      HR=ModelSupport::getHeadRule(SMap,PN,"2103 -2104 -2107");
      makeCell("Pole",System,cellIndex++,poleMat,0.0,HR*outerCutHR*fbHR);
      PoleExclude.push_back(HR);


      HR=ModelSupport::getHeadRule
	(SMap,PN,"2003 -2004 2001 (-2103:2104)");
      makeCell("Coil",System,cellIndex++,coilMat,0.0,HR*outerCutHR*fbHR);
      HR=ModelSupport::getHeadRule(SMap,PN," 2003 -2004 2001");
      PoleExclude.back().addUnion(HR);
      BN+=2;
      PN+=10;
    }

  HR=ModelSupport::getHeadRule
	  (SMap,buildIndex,"501 -502 -1016 -1001 -1002");
  makeCell("Triangle",System,cellIndex++,0,0.0,
	   HR*fbHR*PoleExclude[0].complement()*ICellHR);
  int CN(buildIndex+1);
  int TN(buildIndex+1);
  for(size_t i=1;i<7;i++)
    {
      // three index points
      HR=ModelSupport::getHeadRule
	  (SMap,TN,CN," 501 -502 -1001M -1002M -1003M ");
	
      makeCell("Triangle",System,cellIndex++,0,0.0,
	       HR*fbHR*PoleExclude[i].complement()*ICellHR);
      CN+=2;
      TN++;
    }
  HR=ModelSupport::getHeadRule
	  (SMap,buildIndex,"508 -501 -1014 -1015 -1016 ");
  makeCell("Triangle",System,cellIndex++,0,0.0,HR*fbHR*
	   PoleExclude[7].complement()*ICellHR);	

  
  return;
}

void 
Octupole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Octupole","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);     
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);     

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
Octupole::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Octupole","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
