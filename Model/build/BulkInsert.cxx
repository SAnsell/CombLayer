/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/BulkInsert.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "shutterBlock.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"

namespace shutterSystem
{

BulkInsert::BulkInsert(const size_t ID,const std::string& Key) : 
  attachSystem::FixedGroup(Key+std::to_string(ID+1),"Main",6,"Beam",2),
  attachSystem::ContainedGroup("inner","outer"),
  baseName(Key),
  shutterNumber(ID),
  populated(0),divideSurf(0),DPlane(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

BulkInsert::BulkInsert(const BulkInsert& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedGroup(A),
  shutterNumber(A.shutterNumber),
  populated(A.populated),
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
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
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
BulkInsert::populate(const FuncDataBase& Control,
		     const shutterSystem::GeneralShutter& GS)
  /*!
    Populate all the variables
    \param System :: Simulation to use
    \param GS :: GeneralShutter that aligns to this hole
  */
{
  ELog::RegMethod RegA("BulkInsert","populate");

  const std::string numName=keyName+std::to_string(shutterNumber+1);
  impZero=Control.EvalDefPair<int>(numName+"ImpZero",keyName+"ImpZero",0);  
  // Global from shutter size:

  if (!(populated & 2))
    {
      innerRadius=Control.EvalVar<double>("bulkShutterRadius");
      midRadius=Control.EvalVar<double>("bulkInnerRadius");  
      outerRadius=Control.EvalVar<double>("bulkOuterRadius");  
      populated |= 2;
    }
  xyAngle=GS.getAngle();

  zOffset=Control.EvalTail<double>(keyName,baseName,"ZOffset");
  innerWidth=Control.EvalTail<double>(keyName,baseName,"IWidth");
  innerHeight=Control.EvalTail<double>(keyName,baseName,"IHeight"
				       );
  outerWidth=Control.EvalTail<double>(keyName,baseName,"OWidth");
  outerHeight=Control.EvalTail<double>(keyName,baseName,"OHeight");
  
  
  const std::string KeyNum=std::to_string(shutterNumber+1);

  
  if (Control.hasVariable(keyName+"InnerMat"))
    innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  else
    innerMat=ModelSupport::EvalDefMat<int>(Control,baseName+"InnerMat",0);

  if (Control.hasVariable(keyName+"OuterMat"))    
    outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  else
    outerMat=ModelSupport::EvalDefMat<int>(Control,baseName+"OuterMat",0);
  
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
    Note use the exit point of the GS to construct the 
    bulk insert start point [I think]
    \param GS :: General shutter :
  */
{
  ELog::RegMethod RegA("BulkInsert","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(GS.getKey("Main"),2);
  beamFC.createUnitVector(GS.getKey("Beam"),2);

  
  divideSurf=GS.getDivideSurf();
  DPlane=(divideSurf) ? SMap.realPtr<Geometry::Plane>(divideSurf) : 0;
  Origin=GS.getTargetPoint();
  mainFC.setCentre(Origin);
  setDefault("Main","Beam");
  
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

  SMap.addMatch(buildIndex+7,rInner);
  SMap.addMatch(buildIndex+17,rMid);
  SMap.addMatch(buildIndex+27,rOuter);
  
  //  setExitSurf(SMap.realSurf(buildIndex+27));

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
  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin+Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin-Z*(innerHeight/2.0),Z);

  // Outer Section
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin+Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
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
  Out=ModelSupport::getComposite(SMap,buildIndex,"-5 6 3 -4 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Object(cellIndex++,innerMat,0.0,Out));
  innerVoid=cellIndex-1;

  // Outer
  Out=ModelSupport::getComposite(SMap,buildIndex,"-15 16 13 -14 17 -27 ")+dSurf;
  System.addCell(MonteCarlo::Object(cellIndex++,outerMat,0.0,Out));
  outerVoid=cellIndex-1;

  // ---------------
  // Add excludes
  // ---------------
  MonteCarlo::Object* shutterObj=System.findObject(innerCell);
  if (!shutterObj)
    throw ColErr::InContainerError<int>(innerCell,"shutterObj");

  if (impZero) shutterObj->setImp(0);
  innerInclude=ModelSupport::getComposite(SMap,buildIndex,"3 -4 -5 6 ")+dSurf;
  HeadRule ExLiner(innerInclude);
  ExLiner.makeComplement();
  shutterObj->addSurfString(ExLiner.display());
  
  shutterObj=System.findObject(outerCell);  
  if (!shutterObj)
    throw ColErr::InContainerError<int>(outerCell,"shutterObj");
  if (impZero) shutterObj->setImp(0);
  outerInclude=
    ModelSupport::getComposite(SMap,buildIndex,"13 -14 -15 16 ")+dSurf;

  ExLiner.procString(outerInclude);
  ExLiner.makeComplement();
  shutterObj->addSurfString(ExLiner.display());

  return;
}

void
BulkInsert::createLinks()
  /*!
    Create link components
  */
{
  ELog::RegMethod RegA("BulkInsert","createLinks");

  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  // Inner link

  

  MonteCarlo::LineIntersectVisit BeamLine(beamFC.getCentre(),beamFC.getY());
  const Geometry::Vec3D bExit=
    BeamLine.getPoint(SMap.realPtr<Geometry::Cylinder>(buildIndex+27),
		      beamFC.getCentre());
  
  const int angleFlag((xyAngle>0) ? -1 : 1);
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+7));
  mainFC.addLinkSurf(0,-angleFlag*divideSurf);

  mainFC.setLinkSurf(1,SMap.realSurf(buildIndex+27));
  mainFC.addLinkSurf(1,angleFlag*divideSurf);

  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,bExit,Y);

  mainFC.setLinkSurf(2,SMap.realSurf(buildIndex+3));
  mainFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));
  mainFC.setLinkSurf(4,SMap.realSurf(buildIndex+5));
  mainFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));

  mainFC.setConnect(2,Origin-X*(outerWidth/2.0),-X);
  mainFC.setConnect(3,Origin+X*(outerWidth/2.0),X);
  mainFC.setConnect(4,Origin-Y*(outerHeight/2.0),-Z);
  mainFC.setConnect(5,Origin+Y*(outerHeight/2.0),Z);

  // Exit processed by calculating centre line to exit curve


  
  beamFC.setConnect(0,beamFC.getCentre(),-beamFC.getY());
  beamFC.setConnect(1,bExit,beamFC.getY());

  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+7));
  beamFC.addLinkSurf(0,-angleFlag*divideSurf);
  
  beamFC.setLinkSurf(1,SMap.realSurf(buildIndex+27));
  beamFC.addLinkSurf(1,angleFlag*divideSurf);

  
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

  populate(System.getDataBase(),GS);
  createUnitVector(GS);
  createSurfaces();
  createObjects(System);
  createLinks();
  return;
}


}  // NAMESPACE shutterSystem


