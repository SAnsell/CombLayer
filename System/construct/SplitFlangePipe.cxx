/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/SplitFlangePipe.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "SplitFlangePipe.h"

namespace constructSystem
{

SplitFlangePipe::SplitFlangePipe(const std::string& Key,
				 const bool IF) :
  attachSystem::FixedRotate(Key,10),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  innerLayer(IF),frontJoin(0),backJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(2,"innerPipe");
  FixedComp::nameSideIndex(6,"outerPipe");
  FixedComp::nameSideIndex(8,"pipeWall");
}

SplitFlangePipe::SplitFlangePipe(const SplitFlangePipe& A) :
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  innerLayer(A.innerLayer),frontJoin(A.frontJoin),
  FPt(A.FPt),FAxis(A.FAxis),backJoin(A.backJoin),
  BPt(A.BPt),BAxis(A.BAxis),radius(A.radius),length(A.length),
  feThick(A.feThick),bellowThick(A.bellowThick),
  bellowStep(A.bellowStep),flangeARadius(A.flangeARadius),
  flangeALength(A.flangeALength),flangeBRadius(A.flangeBRadius),
  flangeBLength(A.flangeBLength),voidMat(A.voidMat),
  feMat(A.feMat),bellowMat(A.bellowMat)
  /*!
    Copy constructor
    \param A :: SplitFlangePipe to copy
  */
{}

SplitFlangePipe&
SplitFlangePipe::operator=(const SplitFlangePipe& A)
  /*!
    Assignment operator
    \param A :: SplitFlangePipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      frontJoin=A.frontJoin;
      FPt=A.FPt;
      FAxis=A.FAxis;
      backJoin=A.backJoin;
      BPt=A.BPt;
      BAxis=A.BAxis;
      radius=A.radius;
      length=A.length;
      feThick=A.feThick;
      bellowThick=A.bellowThick;
      bellowStep=A.bellowStep;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      feMat=A.feMat;
      bellowMat=A.bellowMat;
    }
  return *this;
}

SplitFlangePipe::~SplitFlangePipe()
  /*!
    Destructor
  */
{}

void
SplitFlangePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("SplitFlangePipe","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");

  if (!innerLayer)
    {
      bellowThick=Control.EvalDefVar<double>(keyName+"BellowThick",0.0);
      bellowStep=Control.EvalDefVar<double>(keyName+"BellowStep",0.0);
    }
  else
    {
      bellowThick=Control.EvalDefVar<double>(keyName+"CladThick",0.0);
      bellowStep=Control.EvalDefVar<double>(keyName+"CladStep",0.0);
    }

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeFrontRadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBackRadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeFrontLength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBackLength",
					 keyName+"FlangeLength");


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  if (!innerLayer)
    bellowMat=ModelSupport::EvalDefMat(Control,keyName+"BellowMat",feMat);
  else
    bellowMat=ModelSupport::EvalDefMat(Control,keyName+"CladMat",feMat);

  return;
}

void
SplitFlangePipe::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("SplitFlangePipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  applyActiveFrontBack();
  return;
}


void
SplitFlangePipe::applyActiveFrontBack()
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("SplitFlangePipe","applyActiveFrontBack");


  const Geometry::Vec3D curFP=(frontJoin) ? FPt : Origin;
  const Geometry::Vec3D curBP=(backJoin) ? BPt : Origin+Y*length;

  Origin=(curFP+curBP)/2.0;
  const Geometry::Vec3D YAxis=(curBP-curFP).unit();
  Geometry::Vec3D RotAxis=(YAxis*Y).unit();   // need unit for numerical acc.
  if (!RotAxis.nullVector())
    {
      const Geometry::Quaternion QR=
	Geometry::Quaternion::calcQVRot(Y,YAxis,RotAxis);
      Y=YAxis;
      QR.rotate(X);
      QR.rotate(Z);
    }
  else if (Y.dotProd(YAxis) < -0.5) // (reversed
    {
      Y=YAxis;
      X*=-1.0;
      Z*=-1.0;
    }
  return;
}


void
SplitFlangePipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("SplitFlangePipe","createSurfaces");

  // Inner void
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  getShiftedFront(SMap,buildIndex+11,Y,flangeALength);
  getShiftedFront(SMap,buildIndex+21,Y,(flangeALength+bellowStep));
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  FrontBackCut::getShiftedBack(SMap,buildIndex+12,Y,-flangeBLength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+22,Y,
			       -(flangeBLength+bellowStep));

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,radius+feThick+bellowThick);

  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
SplitFlangePipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SplitFlangePipe","createObjects");

  std::string Out;

  const std::string frontStr=frontRule();
  const std::string backStr=backRule();

  // Void
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  // FLANGE Front:
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -107 7 ");
  makeCell("FrontFlange",System,cellIndex++,feMat,0.0,Out+frontStr);

  // FLANGE Back:
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -207 7 ");
  makeCell("BackFlange",System,cellIndex++,feMat,0.0,Out+backStr);

  // Inner clip if present
  if (bellowStep>Geometry::zeroTol)
    {

      if (innerLayer)
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 7");
	  makeCell("MainPipe",System,cellIndex++,feMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 17");
	  makeCell("Cladding",System,cellIndex++,bellowMat,0.0,Out);
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -21 -17 7 ");
	  makeCell("FrontClip",System,cellIndex++,feMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,buildIndex," -12 22 -17 7 ");
	  makeCell("BackClip",System,cellIndex++,feMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 -27 7");
	  makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);
	}

      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -21 -27 17");
      makeCell("FrontSpaceVoid",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex," -12 22 -27 17");
      makeCell("BackSpaceVoid",System,cellIndex++,0,0.0,Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -22 -27 7 ");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);
    }

  // we can simplify outer void if the flanges have the same radii
  if (std::abs(flangeARadius-flangeBRadius)<Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 27 -107 ");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getSetComposite(SMap,buildIndex," -107");
      addOuterUnionSurf(Out+frontStr+backStr);
    }
  else
    {
      // outer boundary [flange front]
      Out=ModelSupport::getSetComposite(SMap,buildIndex," -11 -107 ");
      addOuterSurf(Out+frontStr);
      Out=ModelSupport::getSetComposite(SMap,buildIndex," 12 -207 ");
      addOuterUnionSurf(Out+backStr);


      // outer boundary mid tube
      Out=ModelSupport::getSetComposite(SMap,buildIndex," 11 -12 -27");
      addOuterUnionSurf(Out);
    }

  return;
}



void
SplitFlangePipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("SplitFlangePipe","createLinks");

  // stuff for intersection

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));

  FixedComp::setConnect(6,Origin-Z*(radius+bellowThick),-Z);
  FixedComp::setConnect(7,Origin+Z*(radius+bellowThick),Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+27));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+27));

  // pipe wall
  FixedComp::setConnect(8,Origin-Z*(radius+feThick),-Z);
  FixedComp::setConnect(9,Origin+Z*(radius+feThick),Z);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+17));


  return;
}

void
SplitFlangePipe::setJoinFront(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("SplitFlangePipe","setJoinFront");

  FrontBackCut::setFront(FC,sideIndex);
  frontJoin=1;
  FPt=FC.getLinkPt(sideIndex);
  FAxis=FC.getLinkAxis(sideIndex);

  return;
}

void
SplitFlangePipe::setJoinBack(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Set Back surface
    \param FC :: FixedComponent
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("SplitFlangePipe","setJoinBack");

  FrontBackCut::setBack(FC,sideIndex);
  backJoin=1;
  BPt=FC.getLinkPt(sideIndex);
  BAxis=FC.getLinkAxis(sideIndex);

  return;
}

void
SplitFlangePipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("SplitFlangePipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
