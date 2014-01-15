/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/ZoomChopper.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "surfFunctors.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "shutterBlock.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "BulkShield.h"
#include "bendSection.h"
#include "ZoomBend.h"
#include "ZoomChopper.h"

namespace zoomSystem
{

ZoomChopper::ZoomChopper(const std::string& Key) : 
  attachSystem::TwinComp(Key,6),attachSystem::ContainedComp(),
  chpIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(chpIndex+1),populated(0),
  nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomChopper::ZoomChopper(const ZoomChopper& A) : 
  attachSystem::TwinComp(A),attachSystem::ContainedComp(A),
  chpIndex(A.chpIndex), cellIndex(A.cellIndex),
  populated(A.populated),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  length(A.length),depth(A.depth),height(A.height),
  leftWidth(A.leftWidth),rightWidth(A.rightWidth),
  leftAngle(A.leftAngle),rightAngle(A.rightAngle),
  voidLeftWidth(A.voidLeftWidth),
  voidRightWidth(A.voidRightWidth),voidHeight(A.voidHeight),
  voidDepth(A.voidDepth),voidCut(A.voidCut),
  voidChanLeft(A.voidChanLeft),voidChanRight(A.voidChanRight),
  voidChanUp(A.voidChanUp),voidChanDown(A.voidChanDown),
  nLayers(A.nLayers),wallMat(A.wallMat),
  roofBStep(A.roofBStep),roofHeight(A.roofHeight),
  roofExtra(A.roofExtra),roofMat(A.roofMat),
  roofExtraMat(A.roofExtraMat),monoWallSurf(A.monoWallSurf),
  voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: ZoomChopper to copy
  */
{}

ZoomChopper&
ZoomChopper::operator=(const ZoomChopper& A)
  /*!
    Assignment operator
    \param A :: ZoomChopper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::TwinComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      length=A.length;
      depth=A.depth;
      height=A.height;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
      leftAngle=A.leftAngle;
      rightAngle=A.rightAngle;
      voidLeftWidth=A.voidLeftWidth;
      voidRightWidth=A.voidRightWidth;
      voidHeight=A.voidHeight;
      voidDepth=A.voidDepth;
      voidCut=A.voidCut;
      voidChanLeft=A.voidChanLeft;
      voidChanRight=A.voidChanRight;
      voidChanUp=A.voidChanUp;
      voidChanDown=A.voidChanDown;
      nLayers=A.nLayers;
      wallMat=A.wallMat;
      roofBStep=A.roofBStep;
      roofHeight=A.roofHeight;
      roofExtra=A.roofExtra;
      roofMat=A.roofMat;
      roofExtraMat=A.roofExtraMat;
      monoWallSurf=A.monoWallSurf;
      voidCell=A.voidCell;
    }
  return *this;
}

ZoomChopper::~ZoomChopper() 
  /*!
    Destructor
  */
{}


void
ZoomChopper::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomChopper","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  length=Control.EvalVar<double>(keyName+"Length");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");

  leftWidth=Control.EvalVar<double>(keyName+"LeftWidth");
  rightWidth=Control.EvalVar<double>(keyName+"RightWidth");
  leftAngle=Control.EvalVar<double>(keyName+"LeftAngle");
  rightAngle=Control.EvalVar<double>(keyName+"RightAngle");

  voidLeftWidth=Control.EvalVar<double>(keyName+"VoidLeftWidth");
  voidRightWidth=Control.EvalVar<double>(keyName+"VoidRightWidth");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidCut=Control.EvalVar<double>(keyName+"VoidCut");
  voidChanLeft=Control.EvalVar<double>(keyName+"VoidChanLeft");
  voidChanRight=Control.EvalVar<double>(keyName+"VoidChanRight");
  voidChanUp=Control.EvalVar<double>(keyName+"VoidChanUp");
  voidChanDown=Control.EvalVar<double>(keyName+"VoidChanDown");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nLayers=Control.EvalVar<int>(keyName+"NLayers");

  // SHIELD IS BASIC:
  // ModelSupport::populateDivide(Control,nLayers,keyName+"Mat_",
  // 			       wallMat,guideMat);
  // ModelSupport::populateDivide(Control,nLayers,
  // 			       keyName+"ShieldFrac_",guideFrac);

  populated=1;
  return;
}

void
ZoomChopper::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  FixedComp::setConnect(1,Origin+bY*length,bY);
  FixedComp::setLinkSurf(1,SMap.realSurf(chpIndex+2));
  // Link point is the join between BeamOrigin+
  // beam exit
  setBeamExit(chpIndex+2,bEnter,bY);
  return;
}

void
ZoomChopper::createUnitVector(const zoomSystem::ZoomBend& ZB)
  /*!
    Create the unit vectors
    \param ZB :: Zoom Bender
  */
{
  ELog::RegMethod RegA("ZoomChopper","createUnitVector");
  
  TwinComp::createUnitVector(ZB);
  Origin+=X*xStep+Y*yStep+Z*zStep;
  return;
}


void
ZoomChopper::createSurfaces(const shutterSystem::GeneralShutter& GS)
  /*!
    Create All the surfaces
    \param GS :: GeneralShutter [for divide]
  */
{
  ELog::RegMethod RegA("ZoomChopper","createSurface");
  

  SMap.addMatch(chpIndex+100,GS.getDivideSurf());
  SMap.addMatch(chpIndex+1,monoWallSurf);

  ModelSupport::buildPlane(SMap,chpIndex+2,Origin+
			   bY*length,bY);     // back plane
  
  Geometry::Vec3D LNorm(-X);
  Geometry::Quaternion::calcQRotDeg(-leftAngle,Z).rotate(LNorm);  
  Geometry::Vec3D RNorm(-X);
  Geometry::Quaternion::calcQRotDeg(rightAngle,Z).rotate(RNorm);  

  ModelSupport::buildPlane(SMap,chpIndex+3,
			   Origin+Y*length+X*leftWidth,LNorm);     
  ModelSupport::buildPlane(SMap,chpIndex+4,
  			   Origin+Y*length-X*rightWidth,RNorm);     

  FixedComp::setConnect(2,Origin+Y*length+X*leftWidth,LNorm);     
  FixedComp::setConnect(3,Origin+Y*length-X*rightWidth,RNorm);
  FixedComp::setLinkSurf(2,SMap.realSurf(chpIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(chpIndex+4));


  ModelSupport::buildPlane(SMap,chpIndex+5,Origin-Z*depth,Z); 
  ModelSupport::buildPlane(SMap,chpIndex+6,Origin+Z*height,Z);

  FixedComp::setConnect(4,Origin-Z*depth,Z);     
  FixedComp::setConnect(5,Origin+Z*height,Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(chpIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(chpIndex+6));

  // Void ::
  ModelSupport::buildPlane(SMap,chpIndex+11,
			   Origin-Y*voidCut,Y);     
  ModelSupport::buildPlane(SMap,chpIndex+13,
			   Origin+X*voidLeftWidth,-X);     
  ModelSupport::buildPlane(SMap,chpIndex+14,
			   Origin-X*voidRightWidth,-X);     
  ModelSupport::buildPlane(SMap,chpIndex+15,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,chpIndex+16,Origin+Z*voidHeight,Z);

  ModelSupport::buildPlane(SMap,chpIndex+23,
			   Origin+X*voidChanLeft,-X);     
  ModelSupport::buildPlane(SMap,chpIndex+24,
			   Origin-X*voidChanRight,-X);       
  ModelSupport::buildPlane(SMap,chpIndex+25,Origin-Z*voidChanDown,Z);
  ModelSupport::buildPlane(SMap,chpIndex+26,Origin+Z*voidChanUp,Z);

  return;
}

void
ZoomChopper::createObjects(Simulation& System,
			   const attachSystem::ContainedGroup& CObj)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param CObj :: Contained object
   */
{
  ELog::RegMethod RegA("ZoomChopper","createObjects");

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,chpIndex,"100 1 -2 3 -4 5 -6");
  addOuterSurf(Out);      

  Out+=ModelSupport::getComposite(SMap,chpIndex," (-11:-13:14:-15:16) ");
  Out+=ModelSupport::getComposite(SMap,chpIndex," (11:-23:24:-25:26) ");
  Out+=CObj.getExclude("B");
  Out+=CObj.getExclude("C");
  Out+=CObj.getExclude("D");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  // voids:
  Out=ModelSupport::getComposite(SMap,chpIndex,"11 -2 13 -14 15 -16");
  Out+=CObj.getExclude("B");
  Out+=CObj.getExclude("C");
  Out+=CObj.getExclude("D");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,chpIndex,"100 1 -11 23 -24 25 -26");
  Out+=CObj.getExclude("B");
  Out+=CObj.getExclude("C");
  Out+=CObj.getExclude("D");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return;
}

void
ZoomChopper::createAll(Simulation& System,
		       const zoomSystem::ZoomBend& ZB,
		       const shutterSystem::GeneralShutter& GShutter)	       
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZB :: Zoom Bender
    \param GShutter :: General Shutter [for direction]
  */
{
  ELog::RegMethod RegA("ZoomChopper","createAll");
  
  populate(System);
  createUnitVector(ZB);
  createSurfaces(GShutter);
  createObjects(System,ZB);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE shutterSystem
