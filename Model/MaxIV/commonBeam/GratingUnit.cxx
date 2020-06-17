/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/GratingUnit.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "polySupport.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "GrateHolder.h"
#include "GratingUnit.h"


namespace xraySystem
{

GratingUnit::GratingUnit(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  grateArray({
        std::make_shared<xraySystem::GrateHolder>(keyName+"Grate0"),
	std::make_shared<xraySystem::GrateHolder>(keyName+"Grate1"),
	std::make_shared<xraySystem::GrateHolder>(keyName+"Grate2")})
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


GratingUnit::~GratingUnit()
  /*!
    Destructor
   */
{}

void
GratingUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("GratingUnit","populate");

  FixedRotate::populate(Control);

  HArm=Control.EvalDefVar<double>(keyName+"HArm",40.0);
  PArm=Control.EvalDefVar<double>(keyName+"PArm",22.25);

  zLift=Control.EvalVar<double>(keyName+"ZLift");

  mirrorTheta=Control.EvalVar<double>(keyName+"MirrorTheta");
  mWidth=Control.EvalVar<double>(keyName+"MirrorWidth");
  mThick=Control.EvalVar<double>(keyName+"MirrorThick");
  mLength=Control.EvalVar<double>(keyName+"MirrorLength");

  grateIndex=Control.EvalDefVar<int>(keyName+"GrateIndex",0);
  grateTheta=Control.EvalVar<double>(keyName+"GrateTheta");
  mainGap=Control.EvalVar<double>(keyName+"MainGap");
  mainBarCut=Control.EvalVar<double>(keyName+"MainBarCut");
  mainBarXLen=Control.EvalVar<double>(keyName+"MainBarXLen");
  mainBarDepth=Control.EvalVar<double>(keyName+"MainBarDepth");
  mainBarYWidth=Control.EvalVar<double>(keyName+"MainBarYWidth");

  slidePlateZGap=Control.EvalVar<double>(keyName+"SlidePlateZGap");
  slidePlateThick=Control.EvalVar<double>(keyName+"SlidePlateThick");
  slidePlateWidth=Control.EvalVar<double>(keyName+"SlidePlateWidth");
  slidePlateLength=Control.EvalVar<double>(keyName+"SlidePlateLength");

  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  slideMat=ModelSupport::EvalMat<int>(Control,keyName+"SlideMat");


  return;
}

void
GratingUnit::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("GratingUnit","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  FixedComp::applyShift(0.0,HArm/2.0,zLift);
  return;
}

void
GratingUnit::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("GratingUnit","createSurfaces");

  setCutSurf("innerFront",*grateArray[0],-1);
  setCutSurf("innerBack",*grateArray[0],-2);
  setCutSurf("innerLeft",*grateArray[0],-3);
  setCutSurf("innerRight",*grateArray[2],-4);
  setCutSurf("innerBase",*grateArray[0],-5);
  setCutSurf("innerTop",*grateArray[0],-6);

  // Rotate grating structure based on angle
  const Geometry::Quaternion QGX
    (Geometry::Quaternion::calcQRotDeg(grateTheta,X));

  Geometry::Vec3D GX(X);
  Geometry::Vec3D GY(Y);
  Geometry::Vec3D GZ(Z);
  QGX.rotate(GY);
  QGX.rotate(GZ);


  ExternalCut::makeShiftedSurf
    (SMap,"innerFront",buildIndex+101,GY,-slidePlateLength);
  ExternalCut::makeShiftedSurf
    (SMap,"innerBack",buildIndex+201,GY,slidePlateLength);

  ExternalCut::makeShiftedSurf
    (SMap,"innerLeft",buildIndex+3,GX,-slidePlateWidth);
  ExternalCut::makeShiftedSurf
    (SMap,"innerRight",buildIndex+4,GX,slidePlateWidth);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin+GZ*slidePlateZGap,GZ);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+GZ*(slidePlateZGap+slidePlateThick),GZ);

  // Main support bars
  ExternalCut::makeShiftedSurf
    (SMap,"innerFront",buildIndex+1001,GY,-mainBarYWidth);
  ExternalCut::makeShiftedSurf
    (SMap,"innerBack",buildIndex+1002,GY,mainBarYWidth);

  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-GX*(mainBarXLen/2.0),GX);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+GX*(mainBarXLen/2.0),GX);

  ModelSupport::buildPlane(SMap,buildIndex+1013,Origin-GX*(mainBarCut/2.0),GX);
  ModelSupport::buildPlane(SMap,buildIndex+1014,Origin+GX*(mainBarCut/2.0),GX);

  ModelSupport::buildPlane(SMap,buildIndex+1005,
			   Origin+GZ*(slidePlateZGap-mainBarDepth),GZ);


  // Compute the mirror centre [reflection double angle]

  const double yZero(8.0);
  const double G=sqrt(PArm*PArm+HArm*HArm);
  const double alpha=std::atan(PArm/HArm);

  const double mAngle=std::max(std::abs(mirrorTheta),2.0)*M_PI/180.0;
  const double deltaX=G*sin(alpha-mAngle);

  const Geometry::Vec3D MCentre=Origin-Z*zLift-Y*(deltaX+yZero);
  const Geometry::Quaternion QMX
    (Geometry::Quaternion::calcQRotDeg(mirrorTheta,X));
  Geometry::Vec3D MX(X);
  Geometry::Vec3D MY(Y);
  Geometry::Vec3D MZ(Z);
  QMX.rotate(MY);
  QMX.rotate(MZ);


  ModelSupport::buildPlane(SMap,buildIndex+301,MCentre-MY*(mLength/2.0),MY);
  ModelSupport::buildPlane(SMap,buildIndex+302,MCentre+MY*(mLength/2.0),MY);
  ModelSupport::buildPlane(SMap,buildIndex+303,MCentre-MX*(mWidth/2.0),MX);
  ModelSupport::buildPlane(SMap,buildIndex+304,MCentre+MX*(mWidth/2.0),MX);
  ModelSupport::buildPlane(SMap,buildIndex+305,MCentre-MZ*mThick,MZ);
  ModelSupport::buildPlane(SMap,buildIndex+306,MCentre,MZ);

  return;
}

void
GratingUnit::createObjects(Simulation& System)
  /*!
    Create the mono unit
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("GratingUnit","createObjects");

  const HeadRule frontHR=getRule("innerFront");
  const HeadRule frontOutHR=frontHR.complement();
  const HeadRule backHR=getRule("innerBack");
  const HeadRule backOutHR=backHR.complement();
  const HeadRule baseHR=getRule("innerBase");
  const HeadRule baseOutHR=baseHR.complement();
  const HeadRule topHR=getRule("innerTop");

  const HeadRule cylinderHR=getRule("innerCylinder");

  HeadRule innerHR(frontHR);
  innerHR.addIntersection(backHR);

  innerHR.addIntersection(getRule("innerLeft"));
  innerHR.addIntersection(getRule("innerRight"));

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 ");
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out+
	   baseHR.display() + innerHR.display()+ topHR.display());
  innerHR.makeComplement();

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -201 3 -4 5 -6 ");
  makeCell("FBar",System,cellIndex++,mainMat,0.0,Out + innerHR.display());

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -201 3 -4 6 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,
	   Out+innerHR.display()+topHR.display());

  // outer void on edges
  HeadRule cutHR(getRule("innerLeft"));
  cutHR.addIntersection(getRule("innerRight"));
  cutHR.makeComplement();

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -5 ");
  Out+=cutHR.display()+frontHR.display()+backHR.display()+baseHR.display();

  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

  // support bars

  Out=ModelSupport::getComposite(SMap,buildIndex," 1001  1003 -1013 1005 -5");
  makeCell("OuterFBar",System,cellIndex++,mainMat,0.0,Out+frontOutHR.display());
  Out=ModelSupport::getComposite(SMap,buildIndex," 1001  1013 -1014 1005 -5");
  makeCell("OuterFBarVoid",System,cellIndex++,0,0.0,Out+frontOutHR.display());
  Out=ModelSupport::getComposite(SMap,buildIndex," 1001  1014 -1004 1005 -5");
  makeCell("OuterFBar",System,cellIndex++,mainMat,0.0,Out+frontOutHR.display());

  Out=frontHR.display()+backHR.display()+baseOutHR.display()+
    ModelSupport::getComposite(SMap,buildIndex," 1003 -1004 1005 ");
  makeCell("OuterBarVoid1",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1003 -3 -5 ");
  Out += frontHR.display() + backHR.display() + baseHR.display();
  makeCell("OuterBarVoid2",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1004 4 -5 ");
  Out += frontHR.display() + backHR.display() + baseHR.display();
  makeCell("OuterBarVoid3",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1001 -1002 1003 -1004 1005 -5");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1002 1003 -1013 1005 -5");
  makeCell("OuterBBar",System,cellIndex++,mainMat,0.0,Out+
	   cylinderHR.display()+backOutHR.display());
  Out=ModelSupport::getComposite(SMap,buildIndex," -1002 1013 -1014 1005 -5");
  makeCell("OuterBBarVoid",System,cellIndex++,0,0.0,Out+backOutHR.display());
  Out=ModelSupport::getComposite(SMap,buildIndex," -1002 1014 -1004 1005 -5");
  makeCell("OuterBBar",System,cellIndex++,mainMat,0.0,Out+
  	   cylinderHR.display()+backOutHR.display());

  // inner
  Out=ModelSupport::getComposite(SMap,buildIndex,"  101 -201 3 -4 ");
  addOuterUnionSurf(Out+baseHR.display()+topHR.display());


  // Mirror:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 301 -302 303 -304 305 -306 ");
  makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,Out);
  addOuterUnionSurf(Out);
  //  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -201 3 -4 5 -6 ");
  //  makeCell("MidVoid",System,cellIndex++,0,0.0,Out);

  // support:

  // Out=ModelSupport::getComposite(SMap,buildIndex,"101 -202 3 -4 5 -6");
  // addOuterSurf(Out);

  return;
}

void
GratingUnit::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("GratingUnit","createLinks");


  // // top surface going back down beamline to ring
  // FixedComp::setConnect(0,Origin,-Y);
  // FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+106));

  // // top surface going to experimental area
  // FixedComp::setConnect(1,Origin,Y);
  // FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+205));

  return;
}

void
GratingUnit::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("GratingUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  /// insert later
  for(size_t i=0;i<3;i++)
    {
      grateArray[i]->setIndexPosition(static_cast<int>(i)-grateIndex);
      grateArray[i]->setRotation(0.0,grateTheta);
      grateArray[i]->createAll(System,*this,0);
    }


  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  /// insert later
  for(size_t i=0;i<3;i++)
    grateArray[i]->insertInCell(System,getCell("InnerVoid"));


  return;
}

}  // NAMESPACE xraySystem
