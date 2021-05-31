/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/CurveMagnet.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTIsaCULAR PURPOSE.  See the
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

#include "CurveMagnet.h"

namespace tdcSystem
{

CurveMagnet::CurveMagnet(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("Main"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
    ContainedGroup::addCC("Front");
    ContainedGroup::addCC("Mid");
    ContainedGroup::addCC("Back");
}


CurveMagnet::~CurveMagnet()
  /*!
    Destructor
  */
{}

void
CurveMagnet::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CurveMagnet","populate");

  FixedRotate::populate(Control);

  poleGap=Control.EvalVar<double>(keyName+"PoleGap");
  poleHeight=Control.EvalVar<double>(keyName+"PoleHeight");
  poleAngle=Control.EvalVar<double>(keyName+"PoleAngle");

  coilGap=Control.EvalVar<double>(keyName+"CoilGap");
  coilArcLength=Control.EvalVar<double>(keyName+"CoilArcLength");
  coilArcRadius=Control.EvalVar<double>(keyName+"CoilArcRadius");
  coilDepth=Control.EvalVar<double>(keyName+"CoilDepth");
  coilWidth=Control.EvalVar<double>(keyName+"CoilWidth");

  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");

  return;
}

void
CurveMagnet::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CurveMagnet","createSurfaces");


  const double zLen=coilArcLength*coilArcLength/(2.0*coilArcRadius);
  const double yLen=sqrt(coilArcLength*coilArcLength-zLen*zLen);
  // Two steps : angle to Orgin->bOrg and angle bOrg->bExit
  const double thetaA(2.0*180.0*asin(0.5*coilArcLength/coilArcRadius)/M_PI);

  const Geometry::Quaternion QV =
    Geometry::Quaternion::calcQRotDeg(-thetaA,X);

  // mid point of coil end round:
  const Geometry::Vec3D bY=QV.makeRotate(Y);
  const Geometry::Vec3D bZ=QV.makeRotate(Z);
  const Geometry::Vec3D centOrg=Origin-Z*coilArcRadius;
  const Geometry::Vec3D bOrg=Origin+Y*yLen-Z*zLen;

  // COIL:

  // mid dividers:
  ModelSupport::buildPlane(SMap,buildIndex+151,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+152,bOrg,bY);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(coilGap/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+113,Origin-X*(coilGap/2.0+coilWidth),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(coilGap/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+114,Origin+X*(coilGap/2.0+coilWidth),X);

  ModelSupport::buildCylinder
   (SMap,buildIndex+105,centOrg,X,coilArcRadius-coilDepth/2.0);

  ModelSupport::buildCylinder
    (SMap,buildIndex+106,centOrg,X,coilArcRadius+coilDepth/2.0);

  ELog::EM<<"Coil ARC == "<<coilArcRadius<<" "<<coilDepth/2.0<<" == "<<
    centOrg<<ELog::endDiag;

  // end caps
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,X,coilDepth/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+108,bOrg,X,coilDepth/2.0);

  // inner gap
  ModelSupport::buildCylinder
   (SMap,buildIndex+115,centOrg,X,coilArcRadius-poleHeight/2.0);

  ModelSupport::buildCylinder
    (SMap,buildIndex+116,centOrg,X,coilArcRadius+poleHeight/2.0);

  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,X,poleHeight/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+118,bOrg,X,poleHeight/2.0);

  // POLE arc projection:
  // both cones are at poleAngle and radius: coilArcRaduis
  // Note the main curve is top to bottom so cones have sign reversal
  // in the cells
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(poleGap/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(poleGap/2.0),X);

  const Geometry::Vec3D poleLTopCent=Origin+
    X*(-coilGap/2.0+coilArcRadius/tan(M_PI*poleAngle/180.0))+
       Z*(-coilArcRadius+poleHeight/2.0);

  const Geometry::Vec3D poleLBaseCent=Origin-
    X*(coilGap/2.0+coilArcRadius/tan(M_PI*poleAngle/180.0))-
       Z*(coilArcRadius+poleHeight/2.0);

  ModelSupport::buildCone(SMap,buildIndex+207,poleLBaseCent,X,poleAngle);
  ModelSupport::buildCone(SMap,buildIndex+208,poleLTopCent,X,poleAngle);

  const Geometry::Vec3D poleRTopCent=Origin+
    X*(coilGap/2.0-coilArcRadius/tan(M_PI*poleAngle/180.0))+
       Z*(-coilArcRadius+poleHeight/2.0);

  const Geometry::Vec3D poleRBaseCent=Origin+
    X*(coilGap/2.0+coilArcRadius/tan(M_PI*poleAngle/180.0))-
       Z*(coilArcRadius+poleHeight/2.0);

  ModelSupport::buildCone(SMap,buildIndex+217,poleRBaseCent,X,poleAngle);
  ModelSupport::buildCone(SMap,buildIndex+218,poleRTopCent,X,poleAngle);

  return;
}

void
CurveMagnet::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CurveMagnet","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 151 -152 105 -106 203 -204" );
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -107 -151 203 -204" );
  makeCell("VoidFront",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -108 152 203 -204" );
  makeCell("VoidBack",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -117 -151 113 -103 ");
  makeCell("PoleFront",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"115 -116 151 -152 113 -103 ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -118 152 113 -103 ");
  makeCell("PoleBack",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -117 -151 104 -114 ");
  makeCell("PoleFront",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 115 -116 151 -152 104 -114 ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -118 152 104 -114 ");
  makeCell("PoleBack",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
     "-107 117 -151 113 -103 " );
  makeCell("CoilFront",System,cellIndex++,coilMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
     "113 -103 105 -106 151 -152 (-115:116)" );
  makeCell("Coil",System,cellIndex++,coilMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
     "-108 118 152 113 -103 " );
  makeCell("CoilBack",System,cellIndex++,coilMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-107 117 -151 104 -114" );
  makeCell("CoilFront",System,cellIndex++,coilMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"104 -114 105 -106 151 -152 (-115:116)" );
  makeCell("Coil",System,cellIndex++,coilMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-108 118 152 104 -114" );
  makeCell("CoilBack",System,cellIndex++,coilMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"103 -203 115 -116 151 -152 207 -208" );
  makeCell("LeftPole",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"103 -203 208 -106 151 -152");
  makeCell("LeftPoleVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"103 -203 -207 105 151 -152");
  makeCell("LeftPoleVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"-107 -151 -203 103");
  makeCell("LeftExtraVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"-108 152 -203 103 105 -106");
  makeCell("LeftExtraVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"217 -218 -104 204 115 -116 151 -152");
  makeCell("RightPole",System,cellIndex++,poleMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-217 -104 204 105 151 -152");
  makeCell("RightPoleVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"218 -104 204 -106 151 -152");
  makeCell("RightPoleVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"-107 -151 204 -104");
  makeCell("RightExtraVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"-108 152 204 -104 105 -106");
  makeCell("RightExtraVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"113 -114 105 -106 (-107:151) (-108:-152)");
  addOuterSurf("Main",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," (113 -114 -107 -151) : (113 -114 105 -106 151) ");
  addOuterSurf("Front",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 113 -114 105 -106 ");
  addOuterSurf("Mid",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," (113 -114 -108 152) :  (113 -114 105 -106 -152) ");
  addOuterSurf("Back",Out);

  return;
}


void
CurveMagnet::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("CurveMagnet","createLinks");


  return;
}

void
CurveMagnet::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CurveMagnet","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
