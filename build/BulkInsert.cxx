/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/BulkInsert.cxx
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "TwinComp.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"

namespace shutterSystem
{

BulkInsert::BulkInsert(const size_t ID,const std::string& Key) : 
  TwinComp(Key,6),attachSystem::ContainedGroup("inner","outer"),
  shutterNumber(ID),
  surfIndex(ModelSupport::objectRegister::Instance().cell
	    (Key,static_cast<int>(ID))),
  cellIndex(surfIndex+1), 
  populated(0),divideSurf(0),DPlane(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

BulkInsert::BulkInsert(const BulkInsert& A) : 
  attachSystem::TwinComp(A),attachSystem::ContainedGroup(A),
  shutterNumber(A.shutterNumber),surfIndex(A.surfIndex),
  cellIndex(A.cellIndex),populated(A.populated),
  divideSurf(A.divideSurf),DPlane(A.DPlane),
  xyAngle(A.xyAngle),
  innerRadius(A.innerRadius),midRadius(A.midRadius),
  outerRadius(A.outerRadius),zOffset(A.zOffset),
  innerHeight(A.innerHeight),innerWidth(A.innerWidth),
  outerHeight(A.outerHeight),outerWidth(A.outerWidth),
  innerCell(A.innerCell),
  outerCell(A.outerCell),innerVoid(A.innerVoid),
  outerVoid(A.outerVoid),innerInclude(A.innerInclude),
  outerInclude(A.outerInclude)
  /*!
    Copy constructor
    \param A :: BulkInsert to copy
  */
{}

BulkInsert&
BulkInsert::operator=(const BulkInsert& A)
  /*!
    Assignment operator
    \param A :: BulkInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::TwinComp::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      divideSurf=A.divideSurf;
      xyAngle=A.xyAngle;
      innerRadius=A.innerRadius;
      midRadius=A.midRadius;
      outerRadius=A.outerRadius;
      zOffset=A.zOffset;
      innerHeight=A.innerHeight;
      innerWidth=A.innerWidth;
      outerHeight=A.outerHeight;
      outerWidth=A.outerWidth;
      innerCell=A.innerCell;
      outerCell=A.outerCell;
      innerVoid=A.innerVoid;
      outerVoid=A.outerVoid;
      innerInclude=A.innerInclude;
      outerInclude=A.outerInclude;
    }
  return *this;
}

BulkInsert::~BulkInsert() 
  /*!
    Destructor
  */
{}
		

void
BulkInsert::setGlobalVariables(const double SRad,
			       const double IRad,
			       const double ORad)
  /*!
    Sets external variables:
    \param SRad :: Shutter radius
   */
{
  innerRadius=SRad;
  midRadius=IRad;
  outerRadius=ORad;
  populated |= 2;
  return;
}

		
void
BulkInsert::populate(const Simulation& System,
		     const shutterSystem::GeneralShutter& GS)
  /*!
    Populate all the variables
    \param System :: Simulation to use
    \param GS :: GeneralShutter that aligns to this hole
  */
{
  ELog::RegMethod RegA("BulkInsert","populate");
  const FuncDataBase& Control=System.getDataBase();

  // Global from shutter size:

  if (!(populated & 2))
    {
      innerRadius=Control.EvalVar<double>("bulkShutterRadius");
      midRadius=Control.EvalVar<double>("bulkInnerRadius");  
      outerRadius=Control.EvalVar<double>("bulkOuterRadius");  
      populated |= 2;
    }
  xyAngle=GS.getAngle();
  
  zOffset=SimProcess::getIndexVar<double>
    (Control,keyName,"ZOffset",shutterNumber+1);
  innerWidth=SimProcess::getIndexVar<double>
    (Control,keyName,"IWidth",shutterNumber+1);
  innerHeight=SimProcess::getIndexVar<double>
    (Control,keyName,"IHeight",shutterNumber+1);
  outerWidth=SimProcess::getIndexVar<double>
    (Control,keyName,"OWidth",shutterNumber+1);
  outerHeight=SimProcess::getIndexVar<double>
    (Control,keyName,"OHeight",shutterNumber+1);
  
  populated|=1;
  return;
}

void
BulkInsert::createUnitVector(const shutterSystem::GeneralShutter& GS)
  /*!
    Create unit vectors for shutter along shutter direction
    - Z is gravity 
    - Y is beam XY-axis
    - X is perpendicular to XY-Axis
    \param GS :: General shutter
  */
{
  ELog::RegMethod RegA("BulkInsert","createUnitVector");

  attachSystem::TwinComp::createUnitVector(GS);

  divideSurf=GS.getDivideSurf();
  DPlane=(divideSurf) ? SMap.realPtr<Geometry::Plane>(divideSurf) : 0;
  Origin=GS.getTargetPoint();

  return;
}

void
BulkInsert::setExternal(const int rInner,const int rMid,
			const int rOuter)
/*!
    Set the external surfaces
    \param rInner :: inner cylinder surface
    \param rMid :: Mid cylinder surface
    \param rOuter :: outer cylinder surface
  */
{
  ELog::RegMethod RegA("BulkInsert","setExternal");

  SMap.addMatch(surfIndex+7,rInner);
  SMap.addMatch(surfIndex+17,rMid);
  SMap.addMatch(surfIndex+27,rOuter);
  
  //  setExitSurf(SMap.realSurf(surfIndex+27));

  return;
}

void
BulkInsert::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("BulkInsert","createSurfaces");
  // Inner Section
  ModelSupport::buildPlane(SMap,surfIndex+3,
			   Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,
			   Origin+X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+5,
			   Origin+Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,
			   Origin-Z*(innerHeight/2.0),Z);

  // Outer Section
  ModelSupport::buildPlane(SMap,surfIndex+13,
			   Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,
			   Origin+X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+15,
			   Origin+Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,
			   Origin-Z*(outerHeight/2.0),Z);
  return;
}

std::string
BulkInsert::divideStr() const
  /*!
    Construct the divide string
    \return the +/- string form
   */
{
  ELog::RegMethod RegA("BulkInsert","divideStr");
  std::ostringstream cx;
  if (xyAngle<0)
    cx<<divideSurf;
  else
    cx<<-divideSurf;
  return cx.str();
}
  
void
BulkInsert::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BulkInsert","constructObjects");

  std::string Out;
  // Create divide string
  
  const std::string dSurf=divideStr();
  // inner
  Out=ModelSupport::getComposite(SMap,surfIndex,"-5 6 3 -4 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  innerVoid=cellIndex-1;

  // Outer
  Out=ModelSupport::getComposite(SMap,surfIndex,"-15 16 13 -14 17 -27 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  outerVoid=cellIndex-1;

  // ---------------
  // Add excludes
  // ---------------
  MonteCarlo::Qhull* shutterObj=System.findQhull(innerCell);
  if (!shutterObj)
    throw ColErr::InContainerError<int>(innerCell,RegA.getBase());
  innerInclude=ModelSupport::getComposite(SMap,surfIndex,"3 -4 -5 6 ")+dSurf;

  attachSystem::InsertComp IA;
  IA.setInterSurf(innerInclude);
  shutterObj->addSurfString(IA.getExclude());

  shutterObj=System.findQhull(outerCell);
  if (!shutterObj)
    throw ColErr::InContainerError<int>(outerCell,RegA.getBase());
  outerInclude=
    ModelSupport::getComposite(SMap,surfIndex,"13 -14 -15 16 ")+dSurf;
  IA.setInterSurf(outerInclude);
  shutterObj->addSurfString(IA.getExclude());

  return;
}

void
BulkInsert::createLinks()
  /*!
    Create link components
  */
{
  ELog::RegMethod RegA("BulkInsert","createLinks");

  
  // Inner link
  
  const int angleFlag((xyAngle>0) ? -1 : 1);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+7));
  FixedComp::addLinkSurf(0,-angleFlag*divideSurf);

  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+27));
  FixedComp::addLinkSurf(1,angleFlag*divideSurf);

  FixedComp::setLinkSurf(2,SMap.realSurf(surfIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(surfIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+6));

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,bExit,Y);
  FixedComp::setConnect(2,Origin-X*(outerWidth/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(outerWidth/2.0),X);
  FixedComp::setConnect(4,Origin-Y*(outerHeight/2.0),-Z);
  FixedComp::setConnect(5,Origin+Y*(outerHeight/2.0),Z);
  
  // Exit processed by calculating centre line to exit curve

  MonteCarlo::LineIntersectVisit BeamLine(bEnter,bY);
  bExit=BeamLine.getPoint(SMap.realPtr<Geometry::Cylinder>(surfIndex+27),
			  bEnter);
  return;
}

void
BulkInsert::createAll(Simulation& System,
		      const shutterSystem::GeneralShutter& GS)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param GS :: GeneralShutter to use
   */
{
  ELog::RegMethod RegA("BulkInsert","createAll");

  populate(System,GS);
  createUnitVector(GS);
  createSurfaces();
  createObjects(System);
  createLinks();
  return;
}


}  // NAMESPACE shutterSystem


