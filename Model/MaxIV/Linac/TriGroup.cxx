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
#include "FixedUnit.h"
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
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Main","Top","Mid","Bend",
			       "FFlange"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ContainedGroup::addCC("TFlange");

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
  midLength=Control.EvalVar<double>(keyName+"MidLength");
  midHeight=Control.EvalVar<double>(keyName+"MidHeight");
  midWidth=Control.EvalVar<double>(keyName+"MidWidth");
  midThick=Control.EvalVar<double>(keyName+"MidThick");
  midFlangeRadius=Control.EvalVar<double>(keyName+"MidFlangeRadius");
  midFlangeLength=Control.EvalVar<double>(keyName+"MidFlangeLength");

  bendArcRadius=Control.EvalVar<double>(keyName+"BendArcRadius");
  bendArcLength=Control.EvalVar<double>(keyName+"BendArcLength");
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

  ModelSupport::buildPlane
    (SMap,buildIndex+202,mOrg+mY*(midLength),mY);
  ModelSupport::buildPlane(SMap,buildIndex+203,mOrg-X*(midWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,mOrg+X*(midWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+205,mOrg-mZ*(midHeight/2.0),mZ);
  ModelSupport::buildPlane(SMap,buildIndex+206,mOrg+mZ*(midHeight/2.0),mZ);

  ModelSupport::buildPlane
    (SMap,buildIndex+212,mOrg+mY*(midLength+midThick),mY);
  ModelSupport::buildPlane
    (SMap,buildIndex+213,mOrg-X*(midThick+midWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+214,mOrg+X*(midThick+midWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+215,mOrg-mZ*(midThick+midHeight/2.0),mZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,mOrg+mZ*(midThick+midHeight/2.0),mZ);

  ModelSupport::buildPlane
    (SMap,buildIndex+222,mOrg+mY*(midLength+midThick-midFlangeLength),mY);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,mOrg,mY,midFlangeRadius);
  
  // Bend:

  // impact point of bend (virtual curve)
  // --> zlen= r +/- sqrt(r^2-L^2) [use neg only]
  // r (radius of bend) / L length of straight (mainLength) / zlen drop in Z

  const double fullLen(arcLength+mainLength);
  
  const double zLen=bendArcRadius-
    sqrt(bendArcRadius*bendArcRadius-mainLength*mainLength);

  const double zExit=bendArcRadius-
    sqrt(bendArcRadius*bendArcRadius-fullLength*fullLength);
  
  
  const Geometry::Vec3D bOrg=Origin+Y*mainLength-Z*zLen;
  const Geometry::Vec3D bExit=Origin+Y*fullLength-Z*zExit;

  const double phi(atan(zExit)
  const Geometry::Vec3D bY=Y*fullLength-Z*zExit;  // ???
  
  ModelSupport::buildPlane(SMap,buildIndex+302,bExit,bY);
  ModelSupport::buildPlane
    (SMap,buildIndex+313,mOrg-X*(midThick+midWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+314,mOrg+X*(midThick+midWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+215,mOrg-mZ*(midThick+midHeight/2.0),mZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,mOrg+mZ*(midThick+midHeight/2.0),mZ);

  
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
    (SMap,buildIndex,"13 -14  15 -16 -12 (2:-3:4:-5:6) 107 ");
  makeCell("Walls",System,cellIndex++,wallMat,0.0,Out+frontStr);

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


  // outer boundary [flange front/back]
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 13 -14 15 -16 -12");
  addOuterSurf("Main",Out+frontStr);

  Out=ModelSupport::getSetComposite(SMap,buildIndex," -11 -7");
  addOuterSurf("FFlange",Out+frontStr);

  // outer boundary [flange front/back]
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"12 -112 -117");
  addOuterSurf("Top",Out);
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 112 -102 -127");
  addOuterSurf("TFlange",Out); 
 
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

  FixedComp::setConnect(2,Origin+Y*(wallThick+mainLength),Y);
  FixedComp::setConnect(3,Origin+Y*(wallThick+mainLength),Y);
  FixedComp::setConnect(4,Origin+Y*(wallThick+mainLength),Y);
 


  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+12));


  // top lift point : Out is an complemnt of the volume
  std::string Out;
  FixedComp::setConnect(7,Origin+Z*(wallThick+mainHeight/2.0),Z);
  Out=ModelSupport::getComposite(SMap,buildIndex," (-13 : 14 : -15 : 16) ");
  FixedComp::setLinkSurf(7,Out);

  FixedComp::nameSideIndex(7,"outerPipe");
  
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