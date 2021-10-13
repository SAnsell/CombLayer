/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/BulkInsert.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "shutterBlock.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"

namespace shutterSystem
{

BulkInsert::BulkInsert(const size_t ID,const std::string& Key) : 
  attachSystem::FixedGroup(Key+std::to_string(ID+1),"Main",6,"Beam",2),
  attachSystem::ContainedGroup("inner","outer"),
  attachSystem::ExternalCut(),attachSystem::CellMap(),
  baseName(Key),
  shutterNumber(ID)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

BulkInsert::BulkInsert(const BulkInsert& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedGroup(A),
  attachSystem::ExternalCut(A),attachSystem::CellMap(A),
  shutterNumber(A.shutterNumber),
  zOffset(A.zOffset),
  innerHeight(A.innerHeight),innerWidth(A.innerWidth),
  outerHeight(A.outerHeight),outerWidth(A.outerWidth),
  innerCell(A.innerCell),
  outerCell(A.outerCell),
  innerInclude(A.innerInclude),
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
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      zOffset=A.zOffset;
      innerHeight=A.innerHeight;
      innerWidth=A.innerWidth;
      outerHeight=A.outerHeight;
      outerWidth=A.outerWidth;
      innerCell=A.innerCell;
      outerCell=A.outerCell;
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
BulkInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BulkInsert","populate");

  const std::string numName=keyName+std::to_string(shutterNumber+1);
  impZero=Control.EvalDefPair<int>(numName+"ImpZero",keyName+"ImpZero",0);  
  // Global from shutter size:


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
    innerMat=ModelSupport::EvalDefMat(Control,baseName+"InnerMat",0);

  if (Control.hasVariable(keyName+"OuterMat"))    
    outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  else
    outerMat=ModelSupport::EvalDefMat(Control,baseName+"OuterMat",0);
  
  return;
}

void
BulkInsert::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
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

  FixedGroup::createUnitVector(FC,2);

  const GeneralShutter& GS=dynamic_cast<const GeneralShutter&>(FC);
  mainFC.createUnitVector(GS.getKey("Main"),2);
  beamFC.createUnitVector(GS.getKey("Beam"),2);
  Origin=GS.getTargetPoint();
  mainFC.setCentre(Origin);
  setDefault("Main","Beam");

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
  
void
BulkInsert::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BulkInsert","createObjects");

  const HeadRule RInnerHR=ExternalCut::getRule("RInner");
  const HeadRule RInnerComp=RInnerHR.complement();
  const HeadRule RMidHR=ExternalCut::getRule("RMid");
  const HeadRule RMidComp=RMidHR.complement();
  const HeadRule ROuterHR=ExternalCut::getRule("ROuter");

  
  const HeadRule& dSurf=ExternalCut::getRule("Divider");

  // inner
  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-5 6 3 -4");
  makeCell("Inner",System,cellIndex++,innerMat,0.0,
	   HR*dSurf*RInnerComp*RMidHR);

  // Outer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-15 16 13 -14");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,
	   HR*dSurf*RMidComp*ROuterHR);

  // ---------------
  // Add excludes
  // ---------------
  MonteCarlo::Object* shutterObj=System.findObject(innerCell);
  if (!shutterObj)
    throw ColErr::InContainerError<int>(innerCell,"shutterObj");

  // ELog::EM<<"WARN == ZERO IMP SETTING CARE UPGRATE METHOD"<<ELog::endWarn;  
  // if (impZero) shutterObj->setImp(0);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -5 6 ");

  HR*=dSurf;   // intersection
  HR.makeComplement();
  shutterObj->addIntersection(HR);

  
  shutterObj=System.findObject(outerCell);  
  if (!shutterObj)
    throw ColErr::InContainerError<int>(outerCell,"shutterObj");

  // ELog::EM<<"WARN == ZERO IMP SETTING CARE UPGRATE METHOD"<<ELog::endWarn;
  // if (impZero) shutterObj->setImp(0);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 -15 16 ");
  HR*=dSurf;
  HR.makeComplement();
  shutterObj->addIntersection(HR);

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
  // not constant because of populate surfaces:
  HeadRule HR=ExternalCut::getRule("ROuter");
  const Geometry::Vec3D bExit=
    BeamLine.getPoint(HR,beamFC.getCentre());
  
  beamFC.setLinkSurf(0,ExternalCut::getRule("RInner"));
  mainFC.addLinkSurf(0,ExternalCut::getRule("Divider"));

  mainFC.setLinkSurf(1,ExternalCut::getRule("ROuter").complement());
  mainFC.addLinkSurf(1,ExternalCut::getRule("Divider"));

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

  beamFC.setLinkSurf(0,ExternalCut::getRule("RInner"));
  beamFC.addLinkSurf(0,ExternalCut::getRule("Divider").complement());
  
  beamFC.setLinkSurf(1,ExternalCut::getRule("ROuter").complement());
  beamFC.addLinkSurf(1,ExternalCut::getRule("Divider"));

  
  return;
}

void
BulkInsert::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: GeneralShutter to use
    \param sideIndex :: link point
   */
{
  ELog::RegMethod RegA("BulkInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  
  createLinks();
  return;
}


}  // NAMESPACE shutterSystem


