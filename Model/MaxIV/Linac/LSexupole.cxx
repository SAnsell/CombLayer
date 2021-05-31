/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/LSextupole.cxx
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

#include "LSexupole.h"

namespace tdcSystem
{

LSexupole::LSexupole(const std::string& Key) :
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

LSexupole::LSexupole(const std::string& Base,
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


LSexupole::~LSexupole()
  /*!
    Destructor
  */
{}

void
LSexupole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("LSexupole","populate");

  FixedRotate::populate(Control);

  length=Control.EvalTail<double>(keyName,baseName,"Length");

  frameRadius=Control.EvalTail<double>(keyName,baseName,"FrameRadius");
  frameOuter=Control.EvalTail<double>(keyName,baseName,"FrameOuter");

  poleYAngle=Control.EvalTail<double>(keyName,baseName,"PoleYAngle");
  poleGap=Control.EvalTail<double>(keyName,baseName,"PoleGap");
  poleRadius=Control.EvalTail<double>(keyName,baseName,"PoleRadius");
  poleWidth=Control.EvalTail<double>(keyName,baseName,"PoleWidth");

  coilRadius=Control.EvalTail<double>(keyName,baseName,"CoilRadius");
  coilWidth=Control.EvalTail<double>(keyName,baseName,"CoilWidth");
  coilEndRadius=Control.EvalTail<double>(keyName,baseName,"CoilEndRadius");
  coilEndExtra=Control.EvalTail<double>(keyName,baseName,"CoilEndExtra");

  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat",
				       baseName+"PoleMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat",
				       baseName+"CoilMat");
  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat",
				       baseName+"FrameMat");

  return;
}


void
LSexupole::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LSexupole","createSurface");

  const size_t NPole(6);
  // mid line
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  // pole extention
  const Geometry::Vec3D ePt=Y*(length/2.0+coilEndExtra);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-(ePt*1.001),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+(ePt*1.001),Y);

  int CN(buildIndex+1000);
  // Note there are 16 surfaces on the inner part of the pole peice
  double angle(M_PI*poleYAngle/180.0);

  for(size_t i=0;i < 2*NPole; i++)
    {
      const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);

      // Frame Items:
      ModelSupport::buildPlane(SMap,CN+1,Origin+QR*frameRadius,QR);
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

      const Geometry::Vec3D ePt=Y*(length/2.0-coilEndRadius+coilEndExtra);
      ModelSupport::buildCylinder(SMap,CN+9,Origin-ePt,QR,coilEndRadius);
      ModelSupport::buildCylinder(SMap,CN+19,Origin+ePt,QR,coilEndRadius);

      // Pole Items:
      ModelSupport::buildPlane(SMap,CN+203,Origin-QX*(poleWidth/2.0),QX);
      ModelSupport::buildPlane(SMap,CN+204,Origin+QX*(poleWidth/2.0),QX);
      const Geometry::Vec3D CPt(Origin+QR*(poleGap+poleRadius));
      ModelSupport::buildCylinder(SMap,CN+207,CPt,Y,poleRadius);
      ModelSupport::buildPlane(SMap,CN+201,CPt,QR);

      angle+=2.0*M_PI/static_cast<double>(NPole);
      CN+=20;
    }
  // Outer frame
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*frameOuter,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*frameOuter,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*frameOuter,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*frameOuter,Z);

  return;
}

void
LSexupole::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LSexupole","createObjects");
  const size_t NPole(6);

  std::string Out,unitStr;

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 3 -4 5 -6" );
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6" );
  unitStr=ModelSupport::getSeqUnion(1,static_cast<int>(2*NPole),1);
  Out+=ModelSupport::getComposite(SMap,buildIndex+1000,unitStr);
  makeCell("Frame",System,cellIndex++,frameMat,0.0,Out);

  const std::string ICell=isActive("Inner") ? getRuleStr("Inner") : "";
  /// plane front / back
  const std::string FB=ModelSupport::getComposite(SMap,buildIndex,"1 -2");
  const std::string frontSurf=ModelSupport::getComposite(SMap,buildIndex," -1");
  const std::string backSurf=ModelSupport::getComposite(SMap,buildIndex," 2 ");
  const std::string frontRegion=ModelSupport::getComposite(SMap,buildIndex,"11 -1");
  const std::string backRegion=ModelSupport::getComposite(SMap,buildIndex," 2 -12");

  std::vector<HeadRule> PoleExclude;
  std::vector<HeadRule> frontExclude;
  std::vector<HeadRule> backExclude;

  int BN(buildIndex); // base
  int PN(buildIndex);
  for(size_t i=0; i<NPole;i++)
    {
      // const std::string outerCut=
      // 	ModelSupport::getComposite(SMap,BN," -1001 ");
      const std::string outerCut=ModelSupport::getComposite
	(SMap,buildIndex,"-1001 -1002 -1003 -1004 -1005 -1006 -1007 "
      "-1008 -1009 -1010 -1011 -1012 ");


      Out=ModelSupport::getComposite
	(SMap,PN,BN,"2203 -2204 (-2207 : 2201) -1001M ");
      makeCell("Pole",System,cellIndex++,poleMat,0.0,Out+FB);
      PoleExclude.push_back(HeadRule(Out));

      Out=ModelSupport::getComposite
	(SMap,PN,BN," 2003 -2004 2001  (-2203:2204) -1001M");
      makeCell("Coil",System,cellIndex++,coilMat,0.0,Out+FB);
      Out=ModelSupport::getComposite(SMap,PN," 2003 -2004 2001 ");
      PoleExclude.back().addUnion(Out);

      // Front extra pieces
      Out=ModelSupport::getComposite
	(SMap,PN,BN," 2003 -2004 2001  -2009 -1001M");
      makeCell("CoilFront",System,cellIndex++,coilMat,0.0,Out+frontSurf);
      frontExclude.push_back(HeadRule(Out));

      // back extra pieces
      Out=ModelSupport::getComposite
	(SMap,PN,BN," 2003 -2004 2001 -2019 -1001M");
      makeCell("CoilBack",System,cellIndex++,coilMat,0.0,Out+backSurf);
      backExclude.push_back(HeadRule(Out));

      BN+=2;
      PN+=20;
    }

  int aOffset(-1);
  int bOffset(0);
  std::string OutA,OutB;
  const std::vector<std::string>
    sides({"-4", "-4 -6", "-6 3", "3","3 5", "5 -4"});

  for(size_t i=0;i<NPole;i++)
    {
      OutA=ModelSupport::getRangeComposite
	(SMap,501,506,bOffset,buildIndex,"501R -502R ");
      OutB=ModelSupport::getRangeComposite
      	(SMap,1001,1012,aOffset,buildIndex,"-1001R -1002R -1003R ");
      makeCell("Triangle",System,cellIndex++,0,0.0,OutA+OutB+FB+
	   PoleExclude[i].complement().display()+ICell);

      OutB=ModelSupport::getComposite(SMap,buildIndex,sides[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -1 ");
      makeCell("FrontVoid",System,cellIndex++,0,0.0,OutA+OutB+Out+
	   frontExclude[i].complement().display()+ICell);

      Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 ");
      makeCell("BackVoid",System,cellIndex++,0,0.0,OutA+OutB+Out+
	       backExclude[i].complement().display()+ICell);

      aOffset+=2;
      bOffset+=1;
    }
  return;
}

void
LSexupole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("LSexupole","createLinks");

  const Geometry::Vec3D ePt=Y*(length/2.0+coilEndExtra);
  FixedComp::setConnect(0,Origin-(ePt*1.001),Y);
  FixedComp::setConnect(1,Origin+(ePt*1.001),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  return;
}

void
LSexupole::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("LSexupole","createAll");


  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
