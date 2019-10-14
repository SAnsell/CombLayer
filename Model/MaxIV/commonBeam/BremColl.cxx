/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BremColl.cxx
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
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"

#include "BremColl.h"

namespace xraySystem
{

BremColl::BremColl(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedGroup("Main","Extension"),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

BremColl::BremColl(const BremColl& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),attachSystem::ExternalCut(A),
  height(A.height),width(A.width),length(A.length),
  wallThick(A.wallThick),innerRadius(A.innerRadius),
  flangeARadius(A.flangeARadius),flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),flangeBLength(A.flangeBLength),
  holeXStep(A.holeXStep),holeZStep(A.holeZStep),
  holeAWidth(A.holeAWidth),holeAHeight(A.holeAHeight),
  holeBWidth(A.holeBWidth),holeBHeight(A.holeBHeight),
  voidMat(A.voidMat),innerMat(A.innerMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: BremColl to copy
  */
{}

BremColl&
BremColl::operator=(const BremColl& A)
  /*!
    Assignment operator
    \param A :: BremColl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      height=A.height;
      width=A.width;
      length=A.length;
      wallThick=A.wallThick;
      innerRadius=A.innerRadius;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      holeXStep=A.holeXStep;
      holeZStep=A.holeZStep;
      holeAWidth=A.holeAWidth;
      holeAHeight=A.holeAHeight;
      holeBWidth=A.holeBWidth;
      holeBHeight=A.holeBHeight;
      voidMat=A.voidMat;
      innerMat=A.innerMat;
      wallMat=A.wallMat;
    }
  return *this;
}
  
  
BremColl::~BremColl() 
  /*!
    Destructor
  */
{}

void
BremColl::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("BremColl","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");

  innerRadius=Control.EvalDefVar<double>
    (keyName+"InnerRadius",std::max(height,width)/2.0);


  holeXStep=Control.EvalVar<double>(keyName+"HoleXStep");
  holeZStep=Control.EvalVar<double>(keyName+"HoleZStep");
  holeAHeight=Control.EvalVar<double>(keyName+"HoleAHeight");
  holeAWidth=Control.EvalVar<double>(keyName+"HoleAWidth");
  holeMidDist=Control.EvalVar<double>(keyName+"HoleMidDist");
  holeMidHeight=Control.EvalVar<double>(keyName+"HoleMidHeight");
  holeMidWidth=Control.EvalVar<double>(keyName+"HoleMidWidth");
  holeBHeight=Control.EvalVar<double>(keyName+"HoleBHeight");
  holeBWidth=Control.EvalVar<double>(keyName+"HoleBWidth");
  extLength=Control.EvalVar<double>(keyName+"ExtLength");
  extRadius=Control.EvalVar<double>(keyName+"ExtRadius");

  pipeDepth=Control.EvalVar<double>(keyName+"PipeDepth");
  pipeXSec=Control.EvalVar<double>(keyName+"PipeXSec");
  pipeYStep=Control.EvalVar<double>(keyName+"PipeYStep");
  pipeZStep=Control.EvalVar<double>(keyName+"PipeZStep");
  pipeWidth=Control.EvalVar<double>(keyName+"PipeWidth");
  pipeMidGap=Control.EvalVar<double>(keyName+"PipeMidGap");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");

  return;
}

void
BremColl::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("BremColl","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  Origin+=Y*(flangeALength+length/2.0);
  applyOffset();

  return;
}


void
BremColl::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("BremColl","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(flangeALength+length/2.0),Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(flangeBLength+length/2.0),Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // hole [front]:
  const Geometry::Vec3D holeFront=
    Origin+X*holeXStep+Z*holeZStep-Y*(length/2.0);

  const Geometry::Vec3D holeMid=
    Origin+X*holeXStep+Z*holeZStep+Y*(holeMidDist-length/2.0);

  const Geometry::Vec3D holeBack=
    Origin+X*holeXStep+Z*holeZStep+Y*(length/2.0);

  // mid divider:
  ModelSupport::buildPlane(SMap,buildIndex+1001,holeMid,Y);
  ModelSupport::buildPlane(SMap,buildIndex+1003,
			   holeFront-X*(holeAWidth/2.0),
			   holeMid-X*(holeMidWidth/2.0),
			   holeMid-X*(holeMidWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,
			   holeFront+X*(holeAWidth/2.0),
			   holeMid+X*(holeMidWidth/2.0),
			   holeMid+X*(holeMidWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+1005,
			   holeFront-Z*(holeAHeight/2.0),
			   holeMid-Z*(holeMidHeight/2.0),
			   holeMid-Z*(holeMidHeight/2.0)+X,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,
			   holeFront+Z*(holeAHeight/2.0),
			   holeMid+Z*(holeMidHeight/2.0),
			   holeMid+Z*(holeMidHeight/2.0)+X,
			   Z);

  ModelSupport::buildPlane(SMap,buildIndex+2003,
			   holeMid-X*(holeMidWidth/2.0),
			   holeBack-X*(holeBWidth/2.0),
			   holeBack-X*(holeBWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+2004,
			   holeMid+X*(holeMidWidth/2.0),
			   holeBack+X*(holeBWidth/2.0),
			   holeBack+X*(holeBWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+2005,
			   holeMid-Z*(holeMidHeight/2.0),
			   holeBack-Z*(holeBHeight/2.0),
			   holeBack-Z*(holeBHeight/2.0)+X,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+2006,
			   holeMid+Z*(holeMidHeight/2.0),
			   holeBack+Z*(holeBHeight/2.0),
			   holeBack+Z*(holeBHeight/2.0)+X,
			   Z);
  // Extension space
  ModelSupport::buildPlane(SMap,buildIndex+3002,holeBack+Y*(extLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3003,holeBack-X*(holeBWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+3004,holeBack+X*(holeBWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+3005,holeBack-Z*(holeBHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+3006,holeBack+Z*(holeBHeight/2.0),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+3007,holeBack,Y,extRadius);

  // PipeWork [rect cross sect]
  const Geometry::Vec3D PCent(Origin+Y*(pipeYStep-length/2.0)-Z*pipeZStep);
  const Geometry::Vec3D QCent(Origin+Y*(pipeYStep-length/2.0)+Z*pipeZStep);
  
  ModelSupport::buildPlane(SMap,buildIndex+4001,PCent-Y*(pipeDepth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+4002,PCent+Y*(pipeDepth/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+4003,PCent-X*(pipeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4004,PCent+X*(pipeWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4005,PCent-Z*(pipeXSec/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+4006,PCent+Z*(pipeXSec/2.0),Z);

  // left / righ side
  ModelSupport::buildPlane(SMap,buildIndex+4103,
			   PCent-X*(pipeWidth/2.0-pipeXSec),X);
  ModelSupport::buildPlane(SMap,buildIndex+4104,
			   PCent+X*(pipeWidth/2.0-pipeXSec),X);

  // Pipe Top
  ModelSupport::buildPlane(SMap,buildIndex+4505,QCent-Z*(pipeXSec/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+4506,QCent+Z*(pipeXSec/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+4803,QCent-X*(pipeMidGap/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4804,QCent+X*(pipeMidGap/2.0),X);

  
  // void space:
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			      Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			      Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			      Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			      Origin+Z*(wallThick+height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(length/2.),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeBRadius);

  return;
}

void
BremColl::createObjects(Simulation& System)
  /*!
    Builds the brem-collimator
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BremColl","createObjects");

  const std::string frontSurf(getRuleStr("front"));
  const std::string backSurf(getRuleStr("back"));

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "101 -1001 1003 -1004 1005 -1006 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1001 -102 2003 -2004 2005 -2006 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Pre Water:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -4001 3 -4 5 -6 (-1003: 1004 : -1005: 1006) ");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,Out);

  // Post Water
  Out=ModelSupport::getComposite
      (SMap,buildIndex," 4002 -1001  3 -4 5 -6 (-1003: 1004 : -1005: 1006) ");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,Out);


  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -102 3 -4 5 -6 (-2003: 2004 : -2005: 2006)");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"101 -102 13 -14 15 -16 (-3:4:-5:6)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  // flanges
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -101 ");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," -7  102 3007");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,Out+backSurf);

  // water cooling

  // Part with water
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002 4103 -4104 4006 -4505 "
     "(-1003: 1004 : -1005: 1006)");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002 4003 -4004 4005 -4006 ");
  makeCell("BaseWater",System,cellIndex++,waterMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002 4003 -4103 4006 -4505 ");
  makeCell("LeftWater",System,cellIndex++,waterMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002  4104 -4004 4006 -4505 ");
  makeCell("RightWater",System,cellIndex++,waterMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002 4003 -4803 4505 -4506 ");
  makeCell("TopWaterA",System,cellIndex++,waterMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002  4804 -4004 4505 -4506 ");
  makeCell("TopWaterB",System,cellIndex++,waterMat,0.0,Out);

  // Outer layer on water
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002 3 -4 5 -6 (-4003:4004:-4005:4506) ");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,Out);

  // Mid Gap
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 4001 -4002 4803 -4804 4505  -4506 ");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,Out);
  
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 -101 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," -27 7 102 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+backSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 13 -14 15 -16");
  addOuterSurf("Main",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 -101 ");
  addOuterUnionSurf("Main",Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," -27 102 ");
  addOuterUnionSurf("Main",Out+backSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -3002 -3007  ");
  addOuterSurf("Extension",Out);

  return;
}

void
BremColl::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("BremColl","createLinks");

  // port centre
  
  ExternalCut::createLink("front",*this,0,Origin,-Y); 
  ExternalCut::createLink("back",*this,1,Origin,Y);  
  
  return;
}

void
BremColl::createExtension(Simulation& System,
			  const int insertCell)
  /*!
    Nasty method to build the extension of the bremcollimator
    that feeds into the next object. If it gets any more
    complex then build the brem collimator OUTSIDE the build zone
    \param System :: Simulation to use
    \param insertCell :: Simulation to use
  */
{
  ELog::RegMethod RegA("BremColl","createEntension");
  
  std::string Out;
  // Extension
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -3007 102 -3002 (-3003 : 3004 : -3005 : 3006)");
  makeCell("Extension",System,cellIndex++,innerMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -3007 102 -3002 3003 -3004 3005 -3006 ");
  makeCell("ExtVoid",System,cellIndex++,voidMat,0.0,Out);

  this->insertInCell("Extension",System,insertCell);
  return;
}
  
  
  
void
BremColl::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("BremColl","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  
  insertObjects(System);   


  return;
}
  
}  // NAMESPACE xraySystem
