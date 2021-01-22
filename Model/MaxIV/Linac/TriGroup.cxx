/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/TriGroup.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"

#include "TriGroup.h"

namespace tdcSystem
{

TriGroup::TriGroup(const std::string& Key) :
  attachSystem::FixedRotate(Key,9),
  attachSystem::ContainedGroup("Main","Top","Mid","Bend",
			       "BendStr"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ContainedGroup::addCC("FFlange");
  ContainedGroup::addCC("TFlange");
  ContainedGroup::addCC("MFlange");
  ContainedGroup::addCC("BFlange");

}


TriGroup::~TriGroup()
  /*!
    Destructor
  */
{}

void
TriGroup::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TriGroup","populate");

  FixedRotate::populate(Control);

  mainWidth=Control.EvalVar<double>(keyName+"MainWidth");
  mainHeight=Control.EvalVar<double>(keyName+"MainHeight");

  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  mainSideAngle=Control.EvalVar<double>(keyName+"MainSideAngle");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  topRadius=Control.EvalVar<double>(keyName+"TopRadius");
  topLength=Control.EvalVar<double>(keyName+"TopLength");
  topWallThick=Control.EvalVar<double>(keyName+"TopWallThick");
  topFlangeRadius=Control.EvalVar<double>(keyName+"TopFlangeRadius");
  topFlangeLength=Control.EvalVar<double>(keyName+"TopFlangeLength");

  midZAngle=Control.EvalVar<double>(keyName+"MidZAngle");
  midOpeningAngle=Control.EvalVar<double>(keyName+"MidOpeningAngle");
  midLength=Control.EvalVar<double>(keyName+"MidLength");
  midHeight=Control.EvalVar<double>(keyName+"MidHeight");
  midWidth=Control.EvalVar<double>(keyName+"MidWidth");
  midThick=Control.EvalVar<double>(keyName+"MidThick");
  midFlangeRadius=Control.EvalVar<double>(keyName+"MidFlangeRadius");
  midFlangeLength=Control.EvalVar<double>(keyName+"MidFlangeLength");

  bendZAngle=Control.EvalVar<double>(keyName+"BendZAngle");
  bendZDrop=Control.EvalVar<double>(keyName+"BendZDrop");
  bendArcRadius=Control.EvalVar<double>(keyName+"BendArcRadius");
  bendArcLength=Control.EvalVar<double>(keyName+"BendArcLength");
  bendStrLength=Control.EvalVar<double>(keyName+"BendStrLength");
  bendHeight=Control.EvalVar<double>(keyName+"BendHeight");
  bendWidth=Control.EvalVar<double>(keyName+"BendWidth");
  bendThick=Control.EvalVar<double>(keyName+"BendThick");
  bendFlangeRadius=Control.EvalVar<double>(keyName+"BendFlangeRadius");
  bendFlangeLength=Control.EvalVar<double>(keyName+"BendFlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
TriGroup::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TriGroup","createSurfaces");

  // Inner void
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  // use this so angled fronts correctly make
  ExternalCut::makeShiftedSurf
    (SMap,"front",buildIndex+11,Y,flangeLength);


  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*mainLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(mainHeight/2.0),Z);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+5,
				  Origin-Z*(mainHeight/2.0),
				  Z,X,-mainSideAngle);

  // main pipe walls
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(mainLength+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+mainWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(wallThick+mainHeight/2.0),Z);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+15,
				  Origin-Z*(wallThick+mainHeight/2.0),
				  Z,X,-mainSideAngle);



  // Flange Surfaces :
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeRadius);


  // top pipe:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,topRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,topRadius+topWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+127,Origin,Y,topFlangeRadius);


  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(topLength+mainLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+112,Origin+Y*(topLength+mainLength-topFlangeLength),Y);


  // Mid section:
  const Geometry::Quaternion QV =
    Geometry::Quaternion::calcQRotDeg(-midZAngle,X);
  const Geometry::Vec3D mY=QV.makeRotate(Y);
  const Geometry::Vec3D mZ=QV.makeRotate(Z);
  const Geometry::Vec3D mOrg=
    Origin+mY*(mainLength/cos(M_PI*midZAngle/180.0));
  FixedComp::setConnect(2,mOrg+mY*midLength,mY);

  const Geometry::Quaternion QVlow =
    Geometry::Quaternion::calcQRotDeg(-midZAngle-midOpeningAngle,X);
  const Geometry::Vec3D mZlow=QVlow.makeRotate(Z);
  const Geometry::Quaternion QVup =
    Geometry::Quaternion::calcQRotDeg(-midZAngle+midOpeningAngle,X);
  const Geometry::Vec3D mZup=QVup.makeRotate(Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+202,mOrg+mY*midLength,mY);
  ModelSupport::buildPlane(SMap,buildIndex+203,mOrg-X*(midWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,mOrg+X*(midWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+205,mOrg+mY*(midLength)-mZ*(midHeight/2.0),mZlow);
  ModelSupport::buildPlane(SMap,buildIndex+206,mOrg+mY*(midLength)+mZ*(midHeight/2.0),mZup);

  ModelSupport::buildPlane
    (SMap,buildIndex+213,mOrg-X*(midThick+midWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+214,mOrg+X*(midThick+midWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+215,mOrg+mY*(midLength)-mZ*(midThick+midHeight/2.0),mZlow);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,mOrg+mY*(midLength)+mZ*(midThick+midHeight/2.0),mZup);

  ModelSupport::buildPlane
    (SMap,buildIndex+222,mOrg+mY*(midLength+midThick-midFlangeLength),mY);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,mOrg,mY,midFlangeRadius);

  // Bend:

  // impact point of bend (virtual curve)
  const double zExit=bendArcLength*bendArcLength/(2.0*bendArcRadius);
  const double yExit=sqrt(bendArcLength*bendArcLength-zExit*zExit);

  // Two steps : angle to Orgin->bOrg and angle bOrg->bExit
  const double thetaA(2.0*180.0*asin(0.5*bendArcLength/bendArcRadius)/M_PI);

  const Geometry::Quaternion QWA =
    Geometry::Quaternion::calcQRotDeg(-bendZAngle,X);
  const Geometry::Quaternion QWB =
    Geometry::Quaternion::calcQRotDeg(-(thetaA+bendZAngle),X);

  const Geometry::Vec3D aY=QWA.makeRotate(Y);
  const Geometry::Vec3D aZ=QWA.makeRotate(Z);

  const Geometry::Vec3D bY=QWB.makeRotate(Y);
  const Geometry::Vec3D bZ=QWB.makeRotate(Z);

  // origin / bend exit / str exit
  const Geometry::Vec3D bOrg=Origin+Y*mainLength-Z*bendZDrop;
  const Geometry::Vec3D bExit=bOrg+aY*yExit-aZ*zExit;
  const Geometry::Vec3D cExit=bExit+bY*bendStrLength;

  const Geometry::Vec3D bendCent(bOrg-aZ*bendArcRadius);

  ModelSupport::buildPlane(SMap,buildIndex+302,bExit,bY);
  ModelSupport::buildPlane(SMap,buildIndex+303,bOrg-X*(bendWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,bOrg+X*(bendWidth/2.0),X);
  ModelSupport::buildCylinder
    (SMap,buildIndex+305,bendCent,X,bendArcRadius-bendHeight/2.0);
  ModelSupport::buildCylinder
    (SMap,buildIndex+306,bendCent,X,bendArcRadius+bendHeight/2.0);
  ELog::EM<<"Bend ARC == "<<bendArcRadius<<" "<<bendHeight/2.0<<" == "<<
    bendCent<<ELog::endDiag;
  ModelSupport::buildPlane(SMap,buildIndex+313,
			   bOrg-X*(bendThick+bendWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+314,
			   bOrg+X*(bendThick+bendWidth/2.0),X);
  ModelSupport::buildCylinder
    (SMap,buildIndex+315,bendCent,X,bendArcRadius-(bendThick+bendHeight/2.0));
  ModelSupport::buildCylinder
    (SMap,buildIndex+316,bendCent,X,bendArcRadius+(bendThick+bendHeight/2.0));

  // section after bend:
  ModelSupport::buildPlane(SMap,buildIndex+402,cExit,bY);
  ModelSupport::buildPlane(SMap,buildIndex+405,bExit-bZ*(bendHeight/2.0),bZ);
  ModelSupport::buildPlane(SMap,buildIndex+406,bExit+bZ*(bendHeight/2.0),bZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+415,bExit-bZ*(bendThick+bendHeight/2.0),bZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+416,bExit+bZ*(bendThick+bendHeight/2.0),bZ);

  // Flange

  ModelSupport::buildPlane(SMap,buildIndex+422,cExit-bY*bendFlangeLength,bY);
  ModelSupport::buildCylinder(SMap,buildIndex+427,cExit,bY,bendFlangeRadius);

  FixedComp::setConnect(3,cExit,bY);
  //  ELog::EM<<"Exit Point == "<<cExit<<ELog::endDiag;
  //  ELog::EM<<"Exit angle == "<<180.0*atan(bY[2]/bY[1])/M_PI<<ELog::endDiag;

  // Mid splitting point
  FixedComp::setConnect(4,mOrg+mY*midLength-mZ*midFlangeRadius,(mY+bY).unit());

  return;
}

void
TriGroup::createObjects(Simulation& System)
  /*!
    Adds the vacuum system
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("TriGroup","createObjects");

  std::string Out;

  const std::string frontStr=ExternalCut::getRuleStr("front");

  // Void
  Out=ModelSupport::getComposite(SMap,buildIndex,"3 -4  5 -6 -2 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"13 -14  15 -16 -2 (-3:4:-5:6) ");
  makeCell("Walls",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
	"2 13 -14  15 -16 -12 107 (-203:204:-205:206) (-303:304:-305:306)" );
  makeCell("EndWall",System,cellIndex++,wallMat,0.0,Out);

  // FLANGE Front:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -11 -7 (-13 : 14 : -15 : 16) ");
  makeCell("FrontFlange",System,cellIndex++,flangeMat,0.0,Out+frontStr);

  // TOP Pipe
  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -102 -107 ");
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -102 -117 107");
  makeCell("TopPipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"112 -102 -127 117");
  makeCell("TopFlange",System,cellIndex++,flangeMat,0.0,Out);


  // Mid Pipe
  Out=ModelSupport::getComposite(SMap,buildIndex,"2 203 -204 205 -206 -202 ");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"12 -202 213 -214 215 -216 (-203:204:-205:206)");
  makeCell("MidPipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-202 222 -227 (-213:214:-215:216)");
  makeCell("MidFlange",System,cellIndex++,flangeMat,0.0,Out); // CF viewport

   // Bend Pipe
  Out=ModelSupport::getComposite(SMap,buildIndex,"2 303 -304 305 -306 -302 ");
  makeCell("BendVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"302 303 -304 405 -406 -402 ");
  makeCell("BendVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"12 -302 313 -314 315 -316 (-303:304:-305:306)");
  makeCell("BendPipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"302 -402 313 -314 415 -416 (-303:304:-405:406)");
  makeCell("BendStr",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-402 422 -427 (-313:314:-415:416)");
  makeCell("BendFlange",System,cellIndex++,flangeMat,0.0,Out);


  // outer boundary [flange front/back]
  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -16 -12");
  addOuterSurf("Main",Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -7");
  addOuterSurf("FFlange",Out+frontStr);


  // outer boundary [flange front/back]
  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -112 -117");
  addOuterSurf("Top",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 112 -102 -127");
  addOuterSurf("TFlange",Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"12 213 -214 215 -216 -202");
  addOuterSurf("Mid",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"222 -227 -202");
  addOuterSurf("MFlange",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"12 313 -314 315 -316 -302");
  addOuterSurf("Bend",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"302 313 -314 415 -416 -402");
  addOuterSurf("BendStr",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"422 -427 -402");
  addOuterSurf("BFlange",Out);


  return;
}

void
TriGroup::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("TriGroup","createLinks");

  //stuff for intersection
  ExternalCut::createLink("front",*this,0,Origin,Y);  //front

  FixedComp::setConnect(1,Origin+Y*(topLength+mainLength),Y);
  // connect 3 + 4 set in createSurfaces

  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+102));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+202));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+402));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+202));

  FixedComp::nameSideIndex(1,"straightExit");
  FixedComp::nameSideIndex(2,"viewExit");
  FixedComp::nameSideIndex(3,"magnetExit");

  // top lift point : Out is an complemnt of the volume
  std::string Out;
  FixedComp::setConnect(7,Origin+Z*(wallThick+mainHeight/2.0),Z);
  Out=ModelSupport::getComposite(SMap,buildIndex," (-13 : 14 : -15 : 16) ");
  FixedComp::setLinkSurf(7,Out);

  FixedComp::nameSideIndex(7,"outerPipe");

  FixedComp::setConnect(8,Origin+Y*(mainLength+wallThick),Y);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+12));
  FixedComp::nameSideIndex(8,"EndWall");


  return;
}


void
TriGroup::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("TriGroup","createAll");

  populate(System.getDataBase());
  FixedRotate::createUnitVector(FC,FIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
