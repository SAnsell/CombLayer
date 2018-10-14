/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/OffsetFlangePipe.cxx
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
#include "Qhull.h"
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
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "OffsetFlangePipe.h"

namespace constructSystem
{

OffsetFlangePipe::OffsetFlangePipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,11),
  attachSystem::ContainedSpace(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  frontJoin(0),backJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(9,"FlangeACentre");
  nameSideIndex(10,"FlangeBCentre");
}

OffsetFlangePipe::OffsetFlangePipe(const OffsetFlangePipe& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedSpace(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  frontJoin(A.frontJoin),FPt(A.FPt),FAxis(A.FAxis),
  backJoin(A.backJoin),BPt(A.BPt),BAxis(A.BAxis),
  radius(A.radius),length(A.length),feThick(A.feThick),
  flangeAXStep(A.flangeAXStep),flangeAZStep(A.flangeAZStep),
  flangeAXYAngle(A.flangeAXYAngle),flangeAZAngle(A.flangeAZAngle),
  flangeARadius(A.flangeARadius),flangeALength(A.flangeALength),
  flangeBXStep(A.flangeBXStep),flangeBZStep(A.flangeBZStep),
  flangeBXYAngle(A.flangeBXYAngle),flangeBZAngle(A.flangeBZAngle),
  flangeBRadius(A.flangeBRadius),flangeBLength(A.flangeBLength),
  voidMat(A.voidMat),feMat(A.feMat),flangeAYAxis(A.flangeAYAxis),
  flangeBYAxis(A.flangeBYAxis)
  /*!
    Copy constructor
    \param A :: OffsetFlangePipe to copy
  */
{}

OffsetFlangePipe&
OffsetFlangePipe::operator=(const OffsetFlangePipe& A)
  /*!
    Assignment operator
    \param A :: OffsetFlangePipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedSpace::operator=(A);
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
      flangeAXStep=A.flangeAXStep;
      flangeAZStep=A.flangeAZStep;
      flangeAXYAngle=A.flangeAXYAngle;
      flangeAZAngle=A.flangeAZAngle;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBXStep=A.flangeBXStep;
      flangeBZStep=A.flangeBZStep;
      flangeBXYAngle=A.flangeBXYAngle;
      flangeBZAngle=A.flangeBZAngle;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      feMat=A.feMat;
      flangeAYAxis=A.flangeAYAxis;
      flangeBYAxis=A.flangeBYAxis;
    }
  return *this;
}

OffsetFlangePipe::~OffsetFlangePipe() 
  /*!
    Destructor
  */
{}

void
OffsetFlangePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");  
  length=Control.EvalVar<double>(keyName+"Length");
  feThick=Control.EvalVar<double>(keyName+"FeThick");

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeFrontRadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBackRadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeFrontLength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBackLength",
					 keyName+"FlangeLength");

  flangeAXStep=Control.EvalDefPair<double>(keyName+"FlangeFrontXStep",
					   keyName+"FlangeXStep",0.0);
  flangeAZStep=Control.EvalDefPair<double>(keyName+"FlangeFrontZStep",
					   keyName+"FlangeZStep",0.0);
  flangeAXYAngle=Control.EvalDefVar<double>
    (keyName+"FlangeFrontXYAngle",0.0);

  flangeAZAngle=Control.EvalDefVar<double>
    (keyName+"FlangeFrontZAngle",0.0);
    
  flangeBXStep=Control.EvalDefPair<double>(keyName+"FlangeBackXStep",
					   keyName+"FlangeXStep",0.0);
  flangeBZStep=Control.EvalDefPair<double>(keyName+"FlangeBackZStep",
					   keyName+"FlangeZStep",0.0);
  flangeBXYAngle=Control.EvalDefVar<double>
    (keyName+"FlangeBackXYAngle",0.0);

  flangeBZAngle=Control.EvalDefVar<double>
    (keyName+"FlangeBackZAngle",0.0);

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  return;
}

void
OffsetFlangePipe::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  applyActiveFrontBack();
  flangeAYAxis=Y;
  flangeBYAxis=Y;
  return;
}

void
OffsetFlangePipe::applyActiveFrontBack()
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("OffsetFlangePipe","applyActiveFrontBack");

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
OffsetFlangePipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createSurfaces");
  
  // Inner void
  flangeAYAxis=Y;
  flangeBYAxis=Y;
  if (!frontActive())
    {
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(flangeAZAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(flangeAXYAngle,Z);
      Qz.rotate(flangeAYAxis);
      Qxy.rotate(flangeAYAxis);
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),flangeAYAxis); 
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  getShiftedFront(SMap,buildIndex+11,1,flangeAYAxis,flangeALength);

  
  if (!backActive())
    {

      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(flangeBZAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(flangeBXYAngle,Z);
      Qz.rotate(flangeBYAxis);
      Qxy.rotate(flangeBYAxis);
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),flangeBYAxis);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }
  getShiftedBack(SMap,buildIndex+12,-1,flangeBYAxis,flangeBLength);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);

  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,
			      Origin+X*flangeAXStep+Z*flangeAZStep,flangeAYAxis,
			      flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,
			      Origin+X*flangeBXStep+Z*flangeBZStep,flangeBYAxis,
			      flangeBRadius);
  return;
}

void
OffsetFlangePipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createObjects");

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

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -17 7");
  makeCell("MainPipe",System,cellIndex++,feMat,0.0,Out);

  // outer boundary [flange front/back]
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -11 -107 ");
  addOuterSurf(Out+frontStr);
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 12 -207 ");
  addOuterUnionSurf(Out+backStr);
  // outer boundary mid tube
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 11 -12 -17 ");
  addOuterUnionSurf(Out);

  return;
}
  
void
OffsetFlangePipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));
  
  // pipe wall
  FixedComp::setConnect(7,Origin-Z*(radius+feThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(radius+feThick),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));

  // flange mid point
  FixedComp::setLinkSignedCopy(9,*this,1);
  FixedComp::setLinkSignedCopy(10,*this,2);
  FixedComp::setConnect
    (9,FixedComp::getLinkPt(1)+X*flangeAXStep+Z*flangeAZStep,flangeAYAxis);
  FixedComp::setConnect
    (10,FixedComp::getLinkPt(2)+X*flangeBXStep+Z*flangeBZStep,flangeBYAxis);

  return;
}
  
void
OffsetFlangePipe::setFront(const attachSystem::FixedComp& FC,
			   const long int sideIndex,
			   const bool joinFlag)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
    \param joinFlag :: joint front to link object 
   */
{
  ELog::RegMethod RegA("OffsetFlangePipe","setFront");

  
  FrontBackCut::setFront(FC,sideIndex);
  if (joinFlag)
    {
      frontJoin=1;
      FPt=FC.getLinkPt(sideIndex);
      FAxis=FC.getLinkAxis(sideIndex);
    }
    
  return;
}
  
void
OffsetFlangePipe::setBack(const attachSystem::FixedComp& FC,
			  const long int sideIndex,
			  const bool joinFlag)
  /*!
    Set Back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
    \param joinFlag :: joint front to link object 
   */
{
  ELog::RegMethod RegA("OffsetFlangePipe","setBack");
  
  FrontBackCut::setBack(FC,sideIndex);
  if (joinFlag)
    {
      backJoin=1;
      BPt=FC.getLinkPt(sideIndex);
      BAxis=FC.getLinkAxis(sideIndex);
    }
  return;
}
  
void
OffsetFlangePipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("OffsetFlangePipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
