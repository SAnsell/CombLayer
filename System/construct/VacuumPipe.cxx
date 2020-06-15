/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/VacuumPipe.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "surfDivide.h"

#include "VacuumPipe.h"

namespace constructSystem
{

VacuumPipe::VacuumPipe(const std::string& Key) :
  attachSystem::FixedOffset(Key,11),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  frontJoin(0),backJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

VacuumPipe::VacuumPipe(const VacuumPipe& A) :
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  frontJoin(A.frontJoin),
  FPt(A.FPt),FAxis(A.FAxis),backJoin(A.backJoin),
  BPt(A.BPt),BAxis(A.BAxis),radius(A.radius),height(A.height),
  width(A.width),length(A.length),feThick(A.feThick),
  claddingThick(A.claddingThick),
  flangeARadius(A.flangeARadius),flangeAHeight(A.flangeAHeight),
  flangeAWidth(A.flangeAWidth),flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),flangeBHeight(A.flangeBHeight),
  flangeBWidth(A.flangeBWidth),flangeBLength(A.flangeBLength),
  activeWindow(A.activeWindow),windowFront(A.windowFront),
  windowBack(A.windowBack),voidMat(A.voidMat),
  feMat(A.feMat),claddingMat(A.claddingMat),flangeMat(A.flangeMat),
  nDivision(A.nDivision)
  /*!
    Copy constructor
    \param A :: VacuumPipe to copy
  */
{}

VacuumPipe&
VacuumPipe::operator=(const VacuumPipe& A)
  /*!
    Assignment operator
    \param A :: VacuumPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      frontJoin=A.frontJoin;
      FPt=A.FPt;
      FAxis=A.FAxis;
      backJoin=A.backJoin;
      BPt=A.BPt;
      BAxis=A.BAxis;
      radius=A.radius;
      height=A.height;
      width=A.width;
      length=A.length;
      feThick=A.feThick;
      claddingThick=A.claddingThick;
      flangeARadius=A.flangeARadius;
      flangeAHeight=A.flangeAHeight;
      flangeAWidth=A.flangeAWidth;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBHeight=A.flangeBHeight;
      flangeBWidth=A.flangeBWidth;
      flangeBLength=A.flangeBLength;
      activeWindow=A.activeWindow;
      windowFront=A.windowFront;
      windowBack=A.windowBack;
      voidMat=A.voidMat;
      feMat=A.feMat;
      claddingMat=A.claddingMat;
      flangeMat=A.flangeMat;
      nDivision=A.nDivision;
    }
  return *this;
}

VacuumPipe::~VacuumPipe()
  /*!
    Destructor
  */
{}

void
VacuumPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumPipe","populate");

  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  height=Control.EvalDefVar<double>(keyName+"Height",-1.0);
  width=Control.EvalDefVar<double>(keyName+"Width",-1.0);

  if (radius<0.0 && (width<0.0 || height<0.0))
    throw ColErr::EmptyContainer
      ("Pipe:["+keyName+"] has neither Radius or Height/Width");

  length=Control.EvalVar<double>(keyName+"Length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");
  claddingThick=Control.EvalDefVar<double>(keyName+"CladdingThick",0.0);

  const double fR=Control.EvalDefVar<double>(keyName+"FlangeRadius",-1.0);
  const double fH=Control.EvalDefVar<double>(keyName+"FlangeHeight",-1.0);
  const double fW=Control.EvalDefVar<double>(keyName+"FlangeWidth",-1.0);

  flangeARadius=Control.EvalDefVar<double>(keyName+"FlangeFrontRadius",fR);
  flangeAHeight=Control.EvalDefVar<double>(keyName+"FlangeFrontHeight",fH);
  flangeAWidth=Control.EvalDefVar<double>(keyName+"FlangeFrontWidth",fW);

  flangeBRadius=Control.EvalDefVar<double>(keyName+"FlangeBackRadius",fR);
  flangeBHeight=Control.EvalDefVar<double>(keyName+"FlangeBackHeight",fH);
  flangeBWidth=Control.EvalDefVar<double>(keyName+"FlangeBackWidth",fW);

  if (flangeARadius<0.0 && (flangeAWidth<0.0 || flangeAHeight<0.0))
    throw ColErr::EmptyContainer
      ("Pipe:["+keyName+"][front] missing flange: Radius/Height/Width");
  if (flangeBRadius<0.0 && (flangeBWidth<0.0 || flangeBHeight<0.0))
    throw ColErr::EmptyContainer
      ("Pipe:["+keyName+"][back] missing flange: Radius/Height/Width");

  const double fL=Control.EvalDefVar<double>(keyName+"FlangeLength",-1.0);
  flangeALength=Control.EvalDefVar<double>(keyName+"FlangeFrontLength",fL);
  flangeBLength=Control.EvalDefVar<double>(keyName+"FlangeBackLength",fL);

  // note 1 ==> front : 2 => back  3 both
  activeWindow=Control.EvalDefVar<int>(keyName+"WindowActive",0);

  windowFront.thick=Control.EvalDefPair<double>
    (keyName+"WindowFrontThick",keyName+"WindowThick",0.0);
  windowFront.radius=Control.EvalDefPair<double>
    (keyName+"WindowFrontRadius",keyName+"WindowRadius",-1.0);
  windowFront.height=Control.EvalDefPair<double>
    (keyName+"WindowFrontHeight",keyName+"WindowHeight",-1.0);
  windowFront.width=Control.EvalDefPair<double>
    (keyName+"WindowFrontWidth",keyName+"WindowWidth",-1.0);
  windowFront.mat=ModelSupport::EvalDefMat<int>
    (Control,keyName+"WindowFrontMat",keyName+"WindowMat",0);


  windowBack.thick=Control.EvalDefPair<double>
    (keyName+"WindowBackThick",keyName+"WindowThick",0.0);
  windowBack.radius=Control.EvalDefPair<double>
    (keyName+"WindowBackRadius",keyName+"WindowRadius",-1.0);
  windowBack.height=Control.EvalDefPair<double>
    (keyName+"WindowBackHeight",keyName+"WindowHeight",-1.0);
  windowBack.width=Control.EvalDefPair<double>
    (keyName+"WindowBackWidth",keyName+"WindowWidth",-1.0);
  windowBack.mat=ModelSupport::EvalDefMat<int>
    (Control,keyName+"WindowBackMat",keyName+"WindowMat",0);

  if ((activeWindow & 1) && windowFront.thick<Geometry::zeroTol)
    activeWindow ^= 1;
  if ((activeWindow & 2) && windowBack.thick<Geometry::zeroTol)
    activeWindow ^= 2;


  if (activeWindow & 1)
    {
      if (windowFront.radius<Geometry::zeroTol &&
	  (windowFront.width<Geometry::zeroTol || windowFront.height<Geometry::zeroTol))
	throw ColErr::EmptyContainer("Pipe:["+keyName+"] has neither "
				     "windowFront:Radius or Height/Width");

      if (windowFront.radius>Geometry::zeroTol &&
	  windowFront.radius+Geometry::zeroTol>flangeARadius)
	throw ColErr::SizeError<double>
	  (windowFront.radius,flangeARadius,"Pipe:["+keyName+"] windowFront.Radius/flangeARadius");
    }
  if (activeWindow & 2)
    {
      if (windowBack.radius<Geometry::zeroTol &&
	  (windowBack.width<Geometry::zeroTol || windowBack.height<Geometry::zeroTol))
	throw ColErr::EmptyContainer("Pipe:["+keyName+"] has neither "
				     "windowBack:Radius or Height/Width");

      if (windowBack.radius>Geometry::zeroTol &&
	  windowBack.radius+Geometry::zeroTol>flangeBRadius)
	throw ColErr::SizeError<double>
	  (windowBack.radius,flangeBRadius,"Pipe:["+keyName+"] windowBack.Radius/flangeBRadius");
    }


  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  claddingMat=ModelSupport::EvalDefMat<int>(Control,keyName+"CladdingMat",0);
  flangeMat=ModelSupport::EvalDefMat<int>(Control,keyName+"FlangeMat",feMat);

  nDivision=Control.EvalDefVar<size_t>(keyName+"NDivision",0);
  return;
}

void
VacuumPipe::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VacuumPipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  applyActiveFrontBack();

  return;
}


void
VacuumPipe::applyActiveFrontBack()
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("VacuumPipe","applyActiveFrontBack");


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
VacuumPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumPipe","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // Front Inner void
  getShiftedFront(SMap,buildIndex+101,Y,flangeALength);
  if (activeWindow & 1)
    {
      getShiftedFront(SMap,buildIndex+1001,Y,
		      (flangeALength-windowFront.thick)/2.0);
      getShiftedFront(SMap,buildIndex+1002,Y,
		      (flangeALength+windowFront.thick)/2.0);
    }
  // add data to surface
  if (activeWindow & 1)
    {
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1001));
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1002));
    }

  // Back Inner void
  FrontBackCut::getShiftedBack(SMap,buildIndex+102,Y,-flangeBLength);
  if (activeWindow & 2)
    {
      getShiftedBack(SMap,buildIndex+1101,Y,
		     -(flangeBLength-windowBack.thick)/2.0);
      getShiftedBack(SMap,buildIndex+1102,Y,
		     -(flangeBLength+windowBack.thick)/2.0);
    }


  // add data to surface
  if (activeWindow & 1)
    {
      addSurf("BackWindow",buildIndex+1101);
      addSurf("BackWindow",buildIndex+1102);
    }

  // MAIN SURFACES:
  if (radius>Geometry::zeroTol)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
      ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);
      ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,
				  Y,radius+feThick+claddingThick);
      addSurf("OuterRadius",SMap.realSurf(buildIndex+27));
    }
  else
    {
      double H(height/2.0);
      double W(width/2.0);
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*H,Z);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*H,Z);

      H+=feThick;
      W+=feThick;
      ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*H,Z);
      ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*H,Z);

      H+=claddingThick;
      W+=claddingThick;
      ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*H,Z);
      ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*H,Z);

    }

  // FLANGE SURFACES FRONT:
  if (flangeARadius>0.0)
    ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(flangeAWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(flangeAWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(flangeAHeight/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(flangeAHeight/2.0),Z);
    }

  // FLANGE SURFACES BACK:
  if (flangeBRadius>0.0)
    ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(flangeBWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(flangeBWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(flangeBHeight/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(flangeBHeight/2.0),Z);
    }

  // FRONT WINDOW SURFACES:
  if (activeWindow & 1)
    {
      if (windowFront.radius>0.0)
	ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,
                                    windowFront.radius);
      else
	{
	  ModelSupport::buildPlane(SMap,buildIndex+1003,
                                   Origin-X*(windowFront.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1004,
                                   Origin+X*(windowFront.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1005,
                                   Origin-Z*(windowFront.height/2.0),Z);
	  ModelSupport::buildPlane(SMap,buildIndex+1006,
                                   Origin+Z*(windowFront.height/2.0),Z);
	}
    }

  // FRONT WINDOW SURFACES:
  if (activeWindow & 2)
    {
      if (windowBack.radius>Geometry::zeroTol)
	ModelSupport::buildCylinder(SMap,buildIndex+1107,Origin,Y,
                                    windowBack.radius);
      else
	{
	  ModelSupport::buildPlane(SMap,buildIndex+1103,
                                   Origin-X*(windowBack.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1104,
                                   Origin+X*(windowBack.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1105,
                                   Origin-Z*(windowBack.height/2.0),Z);
	  ModelSupport::buildPlane(SMap,buildIndex+1106,
                                   Origin+Z*(windowBack.height/2.0),Z);
	}
    }

  return;
}

void
VacuumPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VacuumPipe","createObjects");

  std::string Out;

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  std::string windowFrontExclude;
  std::string windowBackExclude;
  if (activeWindow & 1)      // FRONT
    {
      Out=ModelSupport::getSetComposite
	(SMap,buildIndex,"-1007 1003 -1004 1005 -1006 1001 -1002 ");
      makeCell("Window",System,cellIndex++,windowFront.mat,0.0,
				       Out+frontBridgeRule());
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowFrontExclude=WHR.display();
    }
  if (activeWindow & 2)
    {
      Out=ModelSupport::getSetComposite
	(SMap,buildIndex,"-1107 1103 -1104 1105 -1106 1102 -1101 ");
      makeCell("Window",System,cellIndex++,windowBack.mat,0.0,
	       Out+backBridgeRule());
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowBackExclude=WHR.display();
    }

  // Void
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -7 3 -4 5 -6");
  HeadRule InnerVoid(Out);
  InnerVoid.makeComplement();
  makeCell("Void",System,cellIndex++,voidMat,0.0,
	   Out+frontStr+backStr+windowFrontExclude+windowBackExclude);

  Out=ModelSupport::getSetComposite(SMap,buildIndex," -17 13 -14 15 -16");
  HeadRule WallLayer(Out);

  Out=ModelSupport::getSetComposite(SMap,buildIndex," -27 23 -24 25 -26");
  HeadRule CladdingLayer(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 ");
  Out+=WallLayer.display()+InnerVoid.display();
  makeCell("Steel",System,cellIndex++,feMat,0.0,Out);
  addCell("MainSteel",cellIndex-1);
  // cladding
  if (claddingThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 ");
      Out+=WallLayer.complement().display()+CladdingLayer.display();
      makeCell("Cladding",System,cellIndex++,claddingMat,0.0,Out);
    }

  // FLANGE: 107 OR 103-106 valid
  Out=ModelSupport::getSetComposite
  (SMap,buildIndex," -101 -107 103 -104 105 -106 ");
  Out+=InnerVoid.display();
  makeCell("Steel",System,cellIndex++,flangeMat,0.0,
	   Out+frontStr+windowFrontExclude);

  // FLANGE: 207 OR 203-206 valid
  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"102 -207 203 -204 205 -106 ");

  Out+=InnerVoid.display()+backStr+windowBackExclude;
  makeCell("Steel",System,cellIndex++,flangeMat,0.0,Out);

  // outer boundary [flange front]
  Out=ModelSupport::getSetComposite
		    (SMap,buildIndex," -101 -107 103 -104 105 -106 ");
  addOuterSurf(Out+frontStr);

  // outer boundary [flange back]
  Out=ModelSupport::getSetComposite
		    (SMap,buildIndex," 102 -207 203 -204 205 -206 ");
  addOuterUnionSurf(Out+backStr);
  // outer boundary mid tube
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 101 -102 ");
  Out+=CladdingLayer.display();
  addOuterUnionSurf(Out);

  return;
}


void
VacuumPipe::createDivision(Simulation& System)
  /*!
    Divide the vacuum pipe into sections if needed
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("VacuumPipe","createDivision");
  if (nDivision>1)
    {
      ModelSupport::surfDivide DA;
      DA.setBasicSplit(nDivision,feMat);

      DA.init();
      DA.setCellN(getCell("MainSteel"));
      DA.setOutNum(cellIndex,buildIndex+1000);
      DA.makePair<Geometry::Plane>(SMap.realSurf(buildIndex+101),
				   SMap.realSurf(buildIndex+102));

      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
      removeCell("MainSteel");
      addCells("MainSteel",DA.getCells());
    }

  return;
}

void
VacuumPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VacuumPipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  // Round pipe
  if (radius>0.0)
    {
      FixedComp::setConnect(2,Origin-X*radius,-X);
      FixedComp::setConnect(3,Origin+X*radius,X);
      FixedComp::setConnect(4,Origin-Z*radius,-Z);
      FixedComp::setConnect(5,Origin+Z*radius,Z);
      FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
      FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
      FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
      FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));

      FixedComp::setConnect(7,Origin-Z*(radius+feThick),-Z);
      FixedComp::setConnect(8,Origin+Z*(radius+feThick),Z);
      FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+17));
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));

      FixedComp::nameSideIndex(7,"outerPipe");
      FixedComp::nameSideIndex(7,"pipeOuterBase");
      FixedComp::nameSideIndex(8,"pipeOuterTop");
    }
  else // rectangular pipe
    {
      FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
      FixedComp::setConnect(3,Origin+X*(width/2.0),X);
      FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
      FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
      FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
      FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
      FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
      FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

      FixedComp::setConnect(7,Origin-Z*(height/2.0+feThick),-Z);
      FixedComp::setConnect(8,Origin+Z*(height/2.0+feThick),Z);
      FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+15));
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+16));
    }

  // MID Point: [NO SURF]
  const Geometry::Vec3D midPt=
    (getLinkPt(1)+getLinkPt(2))/2.0;
  FixedComp::setConnect(6,midPt,Y);

  if (flangeARadius>0.0)
    {
      FixedComp::setConnect(9,Origin-Z*flangeARadius,-Z);
      FixedComp::setConnect(10,Origin+Z*flangeARadius,Z);

      FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+107));
      FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+107));
    }
  else
    {
      FixedComp::setConnect(9,Origin-Z*(flangeAHeight/2.0),-Z);
      FixedComp::setConnect(10,Origin+Z*(flangeAHeight/2.0),Z);
      FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+105));
      FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+106));
    }


  return;
}

void
VacuumPipe::setJoinFront(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumPipe","setJoinFront");


  FrontBackCut::setFront(FC,sideIndex);
  frontJoin=1;
  FPt=FC.getLinkPt(sideIndex);
  FAxis=FC.getLinkAxis(sideIndex);
  return;
}

void
VacuumPipe::setJoinBack(const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Set Back surface
    \param FC :: FixedComponent
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumPipe","setJoinBack");

  FrontBackCut::setBack(FC,sideIndex);
  backJoin=1;
  BPt=FC.getLinkPt(sideIndex);
  BAxis=FC.getLinkAxis(sideIndex);

  return;
}

void
VacuumPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
    
  createDivision(System);
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
