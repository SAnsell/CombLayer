/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Octupole.cxx
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

Octupole::Octupole(const std::string& Base,
		   const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Base)
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
  
  length=Control.EvalPair<double>(keyName,baseName,"Length");

  frameRadius=Control.EvalPair<double>(keyName,baseName,"FrameRadius");
  frameThick=Control.EvalPair<double>(keyName,baseName,"FrameThick");

  poleGap=Control.EvalPair<double>(keyName,baseName,"PoleGap");
  poleRadius=Control.EvalPair<double>(keyName,baseName,"PoleRadius");
  poleWidth=Control.EvalPair<double>(keyName,baseName,"PoleWidth");

  coilRadius=Control.EvalPair<double>(keyName,baseName,"CoilRadius");
  coilWidth=Control.EvalPair<double>(keyName,baseName,"CoilWidth");
  
  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat",
				       baseName+"PoleMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat",
				       baseName+"CoilMat");
  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat",
				       baseName+"FrameMat");

  return;
}

void
Octupole::createUnitVector(const attachSystem::FixedComp& FC,
    	                     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("Octupole","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
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

  std::string Out;
  
  // Outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex+1000,buildIndex,
     "1M -2M -51 -52 -53 -54 -55 -56 -57 -58 -59 -60 -61 -62 -63 -64 -65 -66 ");
  addOuterSurf(Out);
  
  Out+=ModelSupport::getComposite(SMap,buildIndex+1000,
			" (1:2:3:4:5:6:7:8:9:10:11:12:13:14:15:16) ");

  makeCell("Frame",System,cellIndex++,frameMat,0.0,Out);

  /// create triangles
  const std::string FB=ModelSupport::getComposite(SMap,buildIndex,"1 -2");


  std::vector<HeadRule> PoleExclude; 
  int BN(buildIndex); // base
  int PN(buildIndex);
  for(size_t i=0;i<8;i++)
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
      PoleExclude.back().addUnion(Out);
      BN+=2;
      PN+=10;
    }

  Out=ModelSupport::getComposite
	  (SMap,buildIndex,"501 -502 -1016 -1001 -1002 ");
  makeCell("Triangle",System,cellIndex++,0,0.0,Out+FB+
	   PoleExclude[0].complement().display());
  int CN(buildIndex+1);
  int TN(buildIndex+1);
  for(size_t i=1;i<7;i++)
    {
      // three index points
      Out=ModelSupport::getComposite
	  (SMap,TN,CN," 501 -502 -1001M -1002M -1003M ");
	
      makeCell("Triangle",System,cellIndex++,0,0.0,Out+FB+
	       PoleExclude[i].complement().display());
      CN+=2;
      TN++;
    }
  Out=ModelSupport::getComposite
	  (SMap,buildIndex,"508 -501 -1014 -1015 -1016 ");
  makeCell("Triangle",System,cellIndex++,0,0.0,Out+FB+
	   	       PoleExclude[7].complement().display());	

  
  return;
}

void 
Octupole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Octupole","createLinks");
  
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
