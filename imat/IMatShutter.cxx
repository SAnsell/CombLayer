/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imat/IMatShutter.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

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
#include "surfDIter.h"
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
#include "shutterBlock.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "GeneralShutter.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "IMatShutter.h"

namespace shutterSystem
{

IMatShutter::IMatShutter(const size_t ID,const std::string& K,
			 const std::string& ZK) :
  GeneralShutter(ID,K),imatKey(ZK),
  insIndex(ModelSupport::objectRegister::Instance().cell(ZK)),
  cellIndex(insIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Index number of shutter
    \param K :: Key name
    \param ZK :: IMat Key name
  */ 
{}

IMatShutter::IMatShutter(const IMatShutter& A) : 
  GeneralShutter(A),
  imatKey(A.imatKey),insIndex(A.insIndex),
  cellIndex(A.cellIndex),xStep(A.xStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),width(A.width),
  height(A.height),innerThick(A.innerThick),
  innerMat(A.innerMat),supportThick(A.supportThick),
  supportMat(A.supportMat),gapThick(A.gapThick),
  maskHeight(A.maskHeight),maskWidth(A.maskWidth),
  maskThick(A.maskThick),maskMat(A.maskMat)
  /*!
    Copy constructor
    \param A :: IMatShutter to copy
  */
{}

IMatShutter&
IMatShutter::operator=(const IMatShutter& A)
  /*!
    Assignment operator
    \param A :: IMatShutter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralShutter::operator=(A);
      imatKey=A.imatKey;
      insIndex=A.insIndex;
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      height=A.height;
      innerThick=A.innerThick;
      innerMat=A.innerMat;
      supportThick=A.supportThick;
      supportMat=A.supportMat;
      gapThick=A.gapThick;
      maskHeight=A.maskHeight;
      maskWidth=A.maskWidth;
      maskThick=A.maskThick;
      maskMat=A.maskMat;
    }
  return *this;
}

IMatShutter::~IMatShutter() 
  /*!
    Destructor
  */
{}

void
IMatShutter::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(imatKey+"XStep"); 
  zStep=Control.EvalVar<double>(imatKey+"ZStep"); 
  xyAngle=Control.EvalVar<double>(imatKey+"XYAngle"); 
  zAngle=Control.EvalVar<double>(imatKey+"ZAngle"); 

  width=Control.EvalVar<double>(imatKey+"Width"); 
  height=Control.EvalVar<double>(imatKey+"Height"); 

  innerThick=Control.EvalVar<double>(imatKey+"InnerThick");
  supportThick=Control.EvalVar<double>(imatKey+"SupportThick"); 
  gapThick=Control.EvalVar<double>(imatKey+"GapThick"); 


  maskWidth=Control.EvalVar<double>(imatKey+"MaskWidth"); 
  maskHeight=Control.EvalVar<double>(imatKey+"MaskHeight"); 
  maskThick=Control.EvalVar<double>(imatKey+"MaskThick"); 

  innerMat=ModelSupport::EvalMat<int>(Control,imatKey+"InnerMat"); 
  supportMat=ModelSupport::EvalMat<int>(Control,imatKey+"SupportMat"); 
  maskMat=ModelSupport::EvalMat<int>(Control,imatKey+"MaskMat"); 
  
  return;
}

void
IMatShutter::createUnitVector()
  /*!
    Create the secondary unit vectors
  */
{
  ELog::RegMethod RegA("IMatShutter","createUnitVector");


  frontPt+=X*xStep+Z*zStep;
  bEnter+=X*xStep+Z*zStep;

  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);

  Qz.rotate(bY);
  Qz.rotate(bZ);
  Qxy.rotate(bY);
  Qxy.rotate(bX);
  Qxy.rotate(bZ);

  return;
}

void
IMatShutter::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("IMatShutter","createSurfaces");

  // Void Walls:                                                                                                  
  ModelSupport::buildPlane(SMap,insIndex+3,
			   frontPt-bX*(width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+4,
			   frontPt+bX*(width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+5,
			   frontPt-bZ*(height/2.0),bZ);
  ModelSupport::buildPlane(SMap,insIndex+6,
			   frontPt+bZ*(height/2.0),bZ);
  // Glass walls
  ModelSupport::buildPlane(SMap,insIndex+13,
			   frontPt-bX*(innerThick+width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+14,
			   frontPt+bX*(innerThick+width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+15,
			   frontPt-bZ*(innerThick+height/2.0),bZ);
  ModelSupport::buildPlane(SMap,insIndex+16,
			   frontPt+bZ*(innerThick+height/2.0),bZ);

  //  support walls
  ModelSupport::buildPlane(SMap,insIndex+23,
			   frontPt-bX*(supportThick+innerThick+width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+24,
			   frontPt+bX*(supportThick+innerThick+width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+25,
			   frontPt-bZ*(supportThick+innerThick+height/2.0),bZ);
  ModelSupport::buildPlane(SMap,insIndex+26,
			   frontPt+bZ*(supportThick+innerThick+height/2.0),bZ);

  //  support walls
  ModelSupport::buildPlane(SMap,insIndex+33,
       frontPt-bX*(gapThick+supportThick+innerThick+width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+34,
       frontPt+bX*(gapThick+supportThick+innerThick+width/2.0),bX);
  ModelSupport::buildPlane(SMap,insIndex+35,
       frontPt-bZ*(gapThick+supportThick+innerThick+height/2.0),bZ);
  ModelSupport::buildPlane(SMap,insIndex+36,
       frontPt+bZ*(gapThick+supportThick+innerThick+height/2.0),bZ);

  // Boron mask:
  
  ModelSupport::buildPlane(SMap,insIndex+51,
			   frontPt+bY*maskThick,bY);

  ModelSupport::buildPlane(SMap,insIndex+53,
			   frontPt-bX*maskWidth/2.0,bX);
  ModelSupport::buildPlane(SMap,insIndex+54,
			   frontPt+bX*maskWidth/2.0,bX);
  ModelSupport::buildPlane(SMap,insIndex+55,
			   frontPt-bZ*maskHeight/2.0,bZ);
  ModelSupport::buildPlane(SMap,insIndex+56,
			   frontPt+bZ*maskHeight/2.0,bZ);
  
    
  return;
}
  
void
IMatShutter::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("IMatShutter","constructObjects");

  
  // Create divide string

  const std::string dSurf=divideStr();  
  std::string Out;
  // Get ride old inner void cell(s)
  System.removeCell(innerVoidCell);              // Inner void
  System.removeCell(innerVoidCell+1);              // Inner void

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " 51M -100 13M -14M  15M  -16M"
				 "  ( -3M : 4M : -5M : 6M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " 51M -100 23M -24M  25M  -26M"
				 " ( -13M : 14M : -15M : 16M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " 51M -100 33M -34M  35M  -36M"
				 " ( -23M : 24M : -25M : 26M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " -125 126 13 -14 51M -100 "
				 " ( -33M : 34M : -35M : 36M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));

  // Back section
  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " 100 -17 13M -14M  15M  -16M"
				 "  ( -3M : 4M : -5M : 6M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " 100 -17 23M -24M  25M  -26M"
				 " ( -13M : 14M : -15M : 16M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "  100 -17 33M -34M  35M  -36M"
				 " ( -23M : 24M : -25M : 26M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " -225 226 113 -114 100 -17 "
				 " ( -33M : 34M : -35M : 36M )");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));

  // Inner voids
  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 " 51M -17 3M -4M 5M -6M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Boron mask:

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "7 -125 126 13 -14 -51M  "
				 " (-53M : 54M : -55M : 56M) ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,maskMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,insIndex,
				 "7 -51M 53M -54M 55M -56M ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
				 

  return;
}

void
IMatShutter::createLinks()
  /*!
    Create the links
  */
{
  ELog::RegMethod RegA("IMatShutter","createLinks");

  
  FixedComp::addLinkSurf(0,SMap.realSurf(insIndex+51));
  std::string Out=ModelSupport::getComposite(SMap,surfIndex," -17 100 ");
  FixedComp::addLinkSurf(1,Out);
  FixedComp::addLinkSurf(2,SMap.realSurf(insIndex+3));
  FixedComp::addLinkSurf(3,SMap.realSurf(insIndex+4));
  FixedComp::addLinkSurf(4,SMap.realSurf(insIndex+5));
  FixedComp::addLinkSurf(5,SMap.realSurf(insIndex+6));

  setBeamExit(surfIndex+17,bEnter,bY);
  return;
}

double
IMatShutter::processShutterDrop() const
  /*!
    Calculate the drop on the shutter, given that the 
    fall has to be such that the shutter takes neutrons with
    the correct angle for the shutter.
    \return drop value
  */
{
  ELog::RegMethod RegA("IMatShutter","processShutterDrop");
  // Calculate the distance between the moderator/shutter enterance.
  // currently it is to the target centre

  const double drop=innerRadius*tan(zAngle);
  return drop-zStep;
} 


void
IMatShutter::createAll(Simulation& System,const double,
		       const attachSystem::FixedComp* FCPtr)
  /*!
    Create the shutter
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("IMatShutter","createAll");

  GeneralShutter::populate(System);
  populate(System);  
  GeneralShutter::createAll(System,processShutterDrop(),FCPtr);

  createUnitVector();
  createSurfaces();
  createObjects(System);  
  createLinks();

  return;
}
  
}  // NAMESPACE shutterSystem
