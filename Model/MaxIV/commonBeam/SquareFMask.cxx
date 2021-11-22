/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/SquareFMask.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SquareFMask.h"

namespace xraySystem
{

SquareFMask::SquareFMask(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()  
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  

void
SquareFMask::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SquareFMask","populate");

  FixedRotate::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  minLength=Control.EvalVar<double>(keyName+"MinLength");
  innerAWidth=Control.EvalVar<double>(keyName+"innerAWidth");
  innerMinWidth=Control.EvalVar<double>(keyName+"innerMinWidth");
  innerBWidth=Control.EvalVar<double>(keyName+"innerBWidth");
  innerAHeight=Control.EvalVar<double>(keyName+"innerAHeight");
  innerMinHeight=Control.EvalVar<double>(keyName+"innerMinHeight");
  innerBHeight=Control.EvalVar<double>(keyName+"innerBHeight");

  flangeAInRadius=Control.EvalVar<double>(keyName+"FlangeFrontInRadius");
  flangeAOutRadius=Control.EvalVar<double>(keyName+"FlangeFrontOutRadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeFrontLength");

  flangeBInRadius=Control.EvalVar<double>(keyName+"FlangeBackInRadius");
  flangeBOutRadius=Control.EvalVar<double>(keyName+"FlangeBackOutRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBackLength");

  pipeRadius=Control.EvalDefVar<double>(keyName+"PipeRadius",0.0);
  if (pipeRadius>Geometry::zeroTol)
    {
      pipeXWidth=Control.EvalVar<double>(keyName+"PipeXWidth");
      pipeZDepth=Control.EvalVar<double>(keyName+"PipeZDepth");
      for(size_t i=0;i<4;i++)
	pipeYStep[i]=Control.EvalVar<double>
	  (keyName+"PipeYStep"+std::to_string(i));

      waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
    }
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  
  return;
}

void
SquareFMask::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("SquareFMask","createSurface");

  Geometry::Vec3D APt;  // front
  Geometry::Vec3D MPt;  // mid
  Geometry::Vec3D BPt;  // back
  

  // four possibilites : front/ back/ both/ none:
  if (ExternalCut::isActive("front") &&
      ExternalCut::isActive("back"))
    {
      APt=ExternalCut::interPoint("front",Origin,Y);
      BPt=ExternalCut::interPoint("back",Origin,Y);
      length=BPt.Distance(APt);
    } 
  else if (ExternalCut::isActive("back"))  // front not active
    {
      BPt=ExternalCut::interPoint("back",Origin,Y);
      APt=BPt-Y*length;
      ModelSupport::buildPlane(SMap,buildIndex+1,APt,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  else if (ExternalCut::isActive("front"))  // back not active
    {
      APt=ExternalCut::interPoint("front",Origin,Y);
      BPt=APt+Y*length;
      ModelSupport::buildPlane(SMap,buildIndex+2,BPt,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  else
    {
      APt=Origin-Y*(length/2.0);
      BPt=Origin+Y*(length/2.0);
      ModelSupport::buildPlane(SMap,buildIndex+1,APt,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
      ModelSupport::buildPlane(SMap,buildIndex+2,BPt,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  MPt=APt+Y*minLength;
      

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,APt+Y*flangeALength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,BPt-Y*flangeBLength,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeAInRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,flangeAOutRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,flangeBInRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+18,Origin,Y,flangeBOutRadius);

  // Inner Structure
  //ModelSupport::buildPlane(SMap,buildIndex+101,MPt,Y);
  
  const double AH2(innerAHeight/2.0);
  const double MH2(innerMinHeight/2.0);
  const double BH2(innerBHeight/2.0);
  const double AW2(innerAWidth/2.0);
  const double MW2(innerMinWidth/2.0);
  const double BW2(innerBWidth/2.0);
  
  ModelSupport::buildPlane(SMap,buildIndex+103,
			   APt-X*AW2-Z*AH2,
			   APt-X*AW2+Z*AH2,
			   MPt-X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   APt+X*AW2-Z*AH2,
			   APt+X*AW2+Z*AH2,
			   MPt+X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   APt-X*AW2-Z*AH2,
			   APt+X*AW2-Z*AH2,
			   MPt+X*MW2-Z*MH2,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
			   APt-X*AW2+Z*AH2,
			   APt+X*AW2+Z*AH2,
			   MPt+X*MW2+Z*MH2,Z);

  ModelSupport::buildPlane(SMap,buildIndex+203,
			   BPt-X*BW2-Z*BH2,
			   BPt-X*BW2+Z*BH2,
			   MPt-X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   BPt+X*BW2-Z*BH2,
			   BPt+X*BW2+Z*BH2,
			   MPt+X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+205,
			   BPt-X*BW2-Z*BH2,
			   BPt+X*BW2-Z*BH2,
			   MPt+X*MW2-Z*MH2,Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,
			   BPt-X*BW2+Z*BH2,
			   BPt+X*BW2+Z*BH2,
			   MPt+X*MW2+Z*MH2,Z);



  if (pipeRadius>Geometry::zeroTol)
    {
      // start from front of collimator
      const Geometry::Vec3D pipeOrgZ(APt-Z*pipeZDepth);

      int BI(buildIndex+1000);
      for(size_t i=0;i<4;i++)
	{
	  const Geometry::Vec3D PCent(pipeOrgZ+Y*pipeYStep[i]);
	  ModelSupport::buildCylinder(SMap,BI+7,PCent,X,pipeRadius);
	  // left/right uprights
	  ModelSupport::buildCylinder
	    (SMap,BI+8,PCent-X*(pipeXWidth/2.0),Z,pipeRadius);
	  ModelSupport::buildCylinder
	    (SMap,BI+9,PCent+X*(pipeXWidth/2.0),Z,pipeRadius);
	  // divider plane :
	  if (i<3)
	    ModelSupport::buildPlane
	      (SMap,BI+1,pipeOrgZ+Y*((pipeYStep[i]+pipeYStep[i+1])/2.0),Y);
	  BI+=10;
	}

      // divider [only a pair of left/right needed]
      // 45 deg divider
      ModelSupport::buildPlane(SMap,buildIndex+1003,
			       pipeOrgZ-X*(pipeXWidth/2.0),
			       -X+Z);
      ModelSupport::buildPlane(SMap,buildIndex+1004,
			       pipeOrgZ+X*(pipeXWidth/2.0),
			       X+Z);
    }
      
  return;
}

void
SquareFMask::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("SquareFMask","createObjects");

  const HeadRule frontSurfHR=ExternalCut::getRule("front");
  const HeadRule backSurfHR=ExternalCut::getRule("back");

  HeadRule HR;
  // inner void
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(103 -104 105 -106) : (203 -204 205 -206)");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,
		    HR*frontSurfHR*backSurfHR);

  const HeadRule frontCut=ModelSupport::getHeadRule
    (SMap,buildIndex,"3 -4 5 -6 (-103:104:-105:106)");
  const HeadRule backCut=ModelSupport::getHeadRule
    (SMap,buildIndex,"3 -4 5 -6 (-203:204:-205:206)");
  const HeadRule bothCut=ModelSupport::getHeadRule
    (SMap,buildIndex,"3 -4 5 -6 (-103:104:-105:106) (-203:204:-205:206)");

  if (pipeRadius>Geometry::zeroTol)
    {

      int BI(buildIndex+1000);
      const HeadRule topSurfHR=
	ModelSupport::getHeadRule(SMap,buildIndex,"-6");

      // Make pipes:
      HeadRule pipeHR[4];
      for(size_t i=0;i<4;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"-7M -1003 -1004");
	  CellMap::makeCell("PipeA",System,cellIndex++,waterMat,0.0,HR);
	  pipeHR[i].addUnion(HR);
	  
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"-8M 1003");
	  CellMap::makeCell("PipeLeft",System,cellIndex++,
			    waterMat,0.0,HR*topSurfHR);
	  pipeHR[i].addUnion(HR);
	  
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"-9M 1004");
	  CellMap::makeCell("PipeRight",System,cellIndex++,
			    waterMat,0.0,HR*topSurfHR);
	  pipeHR[i].addUnion(HR);
	  pipeHR[i].makeComplement();
	  BI+=10;
	}


      BI=buildIndex+1000;
      HeadRule fSurf=ModelSupport::getHeadRule(SMap,buildIndex,"11");
      for(size_t i=0;i<3;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,BI,"-1");
	  fSurf*=HR*pipeHR[i];
	  if (pipeYStep[i+1]<minLength)
	    {
	      CellMap::makeCell
		("Coll"+std::to_string(i),System,cellIndex++,
		 mat,0.0,fSurf*frontCut);
	    }
	  else if (pipeYStep[i]>minLength)
	    {
	      CellMap::makeCell
		("Coll"+std::to_string(i),System,cellIndex++,
		 mat,0.0,fSurf*backCut);
	    }
	  else
	    {
	      CellMap::makeCell
		("Coll"+std::to_string(i),System,cellIndex++,
		 mat,0.0,fSurf*bothCut);
	    }
	  fSurf=HR.complement();
	  BI+=10;
	}
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12");
      CellMap::makeCell("Coll3",System,cellIndex++,mat,0.0,
			fSurf*HR*pipeHR[3]*bothCut);
    }
  else   // two simple sections [NO pipes]
    {
      ELog::EM<<"ASDFASFD "<<ELog::endDiag;
      // metal [ inner section]
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"11 -101 3 -4 5 -6 (-103:104:-105:106)");
      CellMap::makeCell("FrontColl",System,cellIndex++,mat,0.0,HR);
      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"101 -12 3 -4 5 -6 (-203:204:-205:206)");
      CellMap::makeCell("BackColl",System,cellIndex++,mat,0.0,HR);
    }


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -7 (-103:104:-105:106)");
  CellMap::makeCell("FrontMat",System,cellIndex++,mat,0.0,
		    HR*frontSurfHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -8 (-203:204:-205:206)");
  CellMap::makeCell("BackMat",System,cellIndex++,mat,0.0,
		    HR*backSurfHR);


  
  // Front flange:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 7 -17");
  CellMap::makeCell("FrontFlange",System,cellIndex++,
		    flangeMat,0.0,HR*frontSurfHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 8 -18");
  CellMap::makeCell("BackFlange",System,cellIndex++,
		    flangeMat,0.0,HR*backSurfHR);
  
  if (flangeAOutRadius>=flangeBOutRadius)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"11 -12 -17 (-3:4:-5:6)");
      CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,HR);
      if (flangeAOutRadius>flangeBOutRadius+Geometry::zeroTol)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12  -17 18");
	  CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,HR*backSurfHR);
	}
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17");
    }
  else
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"11 -12 -18 (-3:4:-5:6)");
      CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,HR);
      if (flangeBOutRadius>flangeAOutRadius+Geometry::zeroTol)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -18 17");
	  CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,HR*frontSurfHR);
	}
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-18");
    }
  addOuterSurf(HR*frontSurfHR*backSurfHR);
  return;
}

void
SquareFMask::createLinks()
  /*!
    Construct the links for the system
  */
{
  ELog::RegMethod RegA("SquareFMask","createLinks");

  
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  
  const Geometry::Vec3D Axis[]={-X,X,-Z,Z};  
  const int surfN((flangeAOutRadius>flangeBOutRadius) ? 17 : 18);
  const double R(std::max(flangeAOutRadius,flangeBOutRadius));
  for(size_t i=0;i<4;i++)
    {
      FixedComp::setConnect(i+2,Origin+Axis[i]*R,Axis[i]);
      FixedComp::setLinkSurf(i+2,SMap.realSurf(buildIndex+surfN));
    }
  return;
}
   
void
SquareFMask::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("SquareFMask","createAllNoPopulate");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks(); 
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
