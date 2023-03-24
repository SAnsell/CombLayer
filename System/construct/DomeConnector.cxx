/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/DomeConnector.cxx
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
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "Quaternion.h"
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
#include "Importance.h"
#include "Object.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "portItem.h"
#include "portSet.h"

#include "DomeConnector.h"

namespace constructSystem
{

DomeConnector::DomeConnector(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  PSet(*this)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


DomeConnector::~DomeConnector()
  /*!
    Destructor
  */
{}

void
DomeConnector::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("DomeConnector","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  curveRadius=Control.EvalVar<double>(keyName+"CurveRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  curveStep=Control.EvalVar<double>(keyName+"CurveStep");
  joinStep=Control.EvalVar<double>(keyName+"JoinStep");
  flatLen=Control.EvalVar<double>(keyName+"FlatLen");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLen=Control.EvalVar<double>(keyName+"FlangeLen");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);

  return;
}

void
DomeConnector::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("DomeConnector","createSurfaces");

  const double surfTolStep(1e-3); // step to allow no near plane/sphere touch
  // sphere on top
  // x step down from cord, y step up from cord.
  const double& y=curveStep;
  const double L=curveRadius;
  const double Radius= (L*L+y*y)/(2.0*y);
  const double x= (L*L-y*y)/(2.0*y);
  const Geometry::Vec3D rCentre=Origin+Y*x; 
  
  ModelSupport::buildSphere(SMap,buildIndex+8,rCentre,Radius);
  ModelSupport::buildSphere(SMap,buildIndex+18,rCentre,Radius+plateThick);


  // Cones sides
  double zStep=(L*joinStep)/(innerRadius-L);
  double tAlpha=L/zStep;
  double alpha=(180.0/M_PI)*std::atan(tAlpha);
  ModelSupport::buildCone(SMap,buildIndex+208,Origin-Y*zStep,Y,alpha);

  // Top sphere extends more than plateThick
  //  L+=plateThick;

  const double rPrime=Radius+plateThick;
  const double sphereWidth=sqrt(rPrime*rPrime-x*x);
  zStep=(sphereWidth*joinStep)/((innerRadius+plateThick)-sphereWidth);
  tAlpha=sphereWidth/zStep;
  alpha=(180.0/M_PI)*std::atan(tAlpha);
  ModelSupport::buildCone(SMap,buildIndex+218,Origin-Y*zStep,Y,alpha);

  // BASE PLANES
  // non-touching plane above top dome
  ModelSupport::buildPlane
    (SMap,buildIndex+1,Origin-Y*(joinStep+plateThick+surfTolStep),Y);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin+Y*joinStep,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(joinStep+flatLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+301,Origin+Y*(joinStep+flatLen-flangeLen),Y);
  
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,innerRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,Y,innerRadius+plateThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,Y,flangeRadius);

  return;
}

void
DomeConnector::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("DomeConnector","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 -101");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-18 8 -101");
  makeCell("Dome",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 -208");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 -218 208");
  makeCell("Void",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -2 -307");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  // flange
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -2 -327 317");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 201 307 -317");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -301 -327 (218:201) (317:-201)");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -101 18 -327");
  makeCell("TopOuterVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -327");
  addOuterSurf(HR);

  return;
}


void
DomeConnector::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("DomeConnector","createLinks");

  //  ExternalCut::createLink("plate",*this,0,Origin,-Y);  //front and back
  //  ExternalCut::createLink("plate",*this,1,Origin,Y);  //front and back
  FixedComp::setConnect(1,Origin-Y*(joinStep+flatLen),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  return;
}

void
DomeConnector::createPorts(Simulation& System)
  /*!
    Construct port
    \parma System :: Simulation to use
   */
{
  ELog::RegMethod RegA("DomeConnector","createPorts");

  const HeadRule frontHR=HeadRule(SMap,buildIndex,-101);
  
  MonteCarlo::Object* insertObj= 
    this->getCellObject(System,"Dome");
  const HeadRule innerHR=HeadRule(SMap,buildIndex,8)*frontHR;
  const HeadRule outerHR=HeadRule(SMap,buildIndex,18)*frontHR;


  for(const int CN : insertCells)
    PSet.addInsertPortCells(CN);
  PSet.addInsertPortCells(getCell("TopOuterVoid"));


  PSet.createPorts(System,insertObj,innerHR,outerHR);
  return;
}

void
DomeConnector::insertInCell(MonteCarlo::Object& outerObj) const
  /*!
    Insert both the flange adn the ports in a cell 
    \param outerObj :: Cell to insert
   */
{
  ELog::RegMethod RegA("DomeConnector","insertInCell");

  ContainedComp::insertInCell(outerObj);
  PSet.insertAllInCell(outerObj);
  
  return;
}

void
DomeConnector::insertInCell(Simulation& System,
			 const int CN) const
  /*!
    Insert both the flange adn the ports in a cell 
    \param System :: Simulation
    \param CN :: Cell to insert
   */
{
  ELog::RegMethod RegA("DomeConnector","insertInCell");
  
  
  MonteCarlo::Object* outerObj=System.findObjectThrow(CN,"CN");
  outerObj->addIntersection(outerSurf.complement());

  ContainedComp::insertInCell(*outerObj);
  PSet.insertAllInCell(System,CN);
  
  return;
}

const portItem&
DomeConnector::getPort(const size_t index) const
  /*!
    Accessor to the portItem
    \param index :: Index of port
    \return portItem
  */
{
  ELog::RegMethod RegA("DomeConnector","getPort");
  
  return PSet.getPort(index);
}

void
DomeConnector::correctPortIntersect(const size_t portIndex)
  /*!
    Calculate the true point of the end of port X
    \param portIndex :: Index point
   */
{
  ELog::RegMethod RegA("DomeConnector","calcPortIntersect");

  // port Info
  const portItem& pI=PSet.getPort(portIndex);
  auto [ pOrg,pAxis,pLen ]=PSet.getPortInfo(portIndex);
  Geometry::Vec3D nOrg(pOrg+pAxis*pLen);

  pAxis.reBase(pI.getX(),-pI.getY(),pI.getZ());

  //  Origin-=nOrg;
  //  this->reOrientate(0,-pAxis);
  
  //  ELog::EM<<"New Axis == "<<Y<<" :: "<<Z<<ELog::endDiag;
  pAxis*=-1.0;
  Geometry::Vec3D rotAxis(0,1,0);
  Geometry::Vec3D postZAxis(0,0,1);
  rotAxis=rotAxis.getInBasis(X,Y,Z);
  postZAxis=postZAxis.getInBasis(X,Y,Z);

  ELog::EM<<"R axi == "<<rotAxis<<" "<<pAxis<<ELog::endDiag;
  // retrack y to the port axis
  const Geometry::Quaternion QV=
    Geometry::Quaternion::calcQVRot(Geometry::Vec3D(0,1,0),pAxis,rotAxis);

  // Now move QV into the main basis set origin,X,Y,Z:
  const Geometry::Vec3D& QVvec=QV.getVec();
  const Geometry::Vec3D QAxis=QVvec.getInBasis(X,Y,Z);

  // Move X,Y,Z to the main rotation direction:
  ELog::EM<<"QV[0] = "<<QV[0]<<ELog::endDiag;
  const Geometry::Quaternion QVmain(QV[0],-QAxis);
  QVmain.rotateBasis(X,Y,Z);

  const Geometry::Vec3D portOriginB=
    pOrg.getInBasis(X,Y,Z)+Y*pLen;

  Origin+=portOriginB;


  return;
}
  
void
DomeConnector::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("DomeConnector","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  PSet.constructPortAxis(System.getDataBase());
  correctPortIntersect(0);
  
  createSurfaces();
  createObjects(System);
  createLinks();
  createPorts(System);
  insertObjects(System);  

  
  return;
}

}  // NAMESPACE constructSystem
