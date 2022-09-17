/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TriangleShield.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
 *************************************************************************/
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
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "surfDIter.h"

#include "TriangleShield.h"

namespace constructSystem
{

TriangleShield::TriangleShield(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


TriangleShield::~TriangleShield() 
  /*!
    Destructor
  */
{}

void
TriangleShield::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("TriangleShield","removeFrontOverLap");

  if (frontActive())
    {
      size_t index(1);
      const double segStep(length/static_cast<double>(nSeg));
      // note : starts one step ahead of front.
      Geometry::Vec3D SP(Origin-Y*(length/2.0-segStep));

      while(index<nSeg && !getFrontRule().isValid(SP))
        {
          SP+=Y*segStep;
          index++;
        }
      index--;
      if (index>0)
        {
          length-=segStep*static_cast<double>(index);
          nSeg-=index;
          Origin+=Y*(static_cast<double>(index)*segStep/2.0);
          ELog::EM<<"Removal["<<keyName<<"] of Active segment == "
                  <<index<<ELog::endDiag;
        }
    }
  
  return;
}
  
void
TriangleShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TriangleShield","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  leftAngle=Control.EvalDefVar<double>(keyName+"LeftAngle",0.0);
  rightAngle=Control.EvalDefVar<double>(keyName+"RightAngle",0.0);
  left=Control.EvalVar<double>(keyName+"Left");
  right=Control.EvalVar<double>(keyName+"Right");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  if (!FrontBackCut::backActive())
    {
      endWall=Control.EvalDefVar<double>(keyName+"EndWall",0.0);
      endVoid=Control.EvalDefVar<double>(keyName+"EndVoid",0.0);
      if (endWall>Geometry::zeroTol)
	{
	  nEndLayers=Control.EvalDefVar<size_t>(keyName+"NEndLayers",0);
	  ModelSupport::populateDivide(Control,nEndLayers,keyName+"EndMat",
				       defMat,endMat);
	  ModelSupport::populateDivideLen(Control,nEndLayers,keyName+"EndLen",
					  endWall,endFrac);
	  endFrac.push_back(1.0);
	}
    }
  
  defMat=ModelSupport::EvalDefMat(Control,keyName+"DefMat",0);

  nSeg=Control.EvalDefVar<size_t>(keyName+"NSeg",1);
  nWallLayers=Control.EvalVar<size_t>(keyName+"NWallLayers");
  ModelSupport::populateDivide(Control,nWallLayers,keyName+"WallMat",
			       defMat,wallMat);
  ModelSupport::populateDivideLen(Control,nWallLayers,keyName+"WallLen",
				  std::min(left,right),wallFrac);
  wallFrac.push_back(1.0);
  
  nRoofLayers=Control.EvalVar<size_t>(keyName+"NRoofLayers");
  ModelSupport::populateDivide(Control,nRoofLayers,keyName+"RoofMat",
			       defMat,roofMat);
  ModelSupport::populateDivideLen(Control,nRoofLayers,keyName+"RoofLen",
				  height,roofFrac);
  roofFrac.push_back(1.0);
  
  nFloorLayers=Control.EvalVar<size_t>(keyName+"NFloorLayers");
  ModelSupport::populateDivide(Control,nFloorLayers,keyName+"FloorMat",
			       defMat,floorMat);
  ModelSupport::populateDivideLen(Control,nFloorLayers,keyName+"FloorLen",
				  depth,floorFrac);
  floorFrac.push_back(1.0);
  
  return;
}

void
TriangleShield::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("TriangleShield","createSurfaces");

  removeFrontOverLap();
  // Inner void
  if (!FrontBackCut::frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }

  if (!FrontBackCut::backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+2));
      if (endWall>Geometry::zeroTol)
	{
	  ModelSupport::buildPlane(SMap,buildIndex+1002,
				   Origin+Y*(length/2.0-endWall),Y);
	  if (endVoid>Geometry::zeroTol)
	    ModelSupport::buildCylinder(SMap,buildIndex+1007,
					Origin,Y,endVoid);
	}
	    
    }
  //create back cut for wall
  //  const int BSurf=getBackRule().getPrimarySurface();
  
  const double segStep(length/static_cast<double>(nSeg));
  double segLen(-length/2.0);
  int SI(buildIndex+10);
  for(size_t i=1;i<nSeg;i++)
    {
      segLen+=segStep;
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*segLen,Y);
      SI+=10;
    }

  // wall rotation
  const Geometry::Quaternion QLeft=
    Geometry::Quaternion::calcQRotDeg(leftAngle,Z);
  const Geometry::Quaternion QRight=
    Geometry::Quaternion::calcQRotDeg(-rightAngle,Z);
  Geometry::Vec3D XL(X);
  Geometry::Vec3D XR(X);
  QLeft.rotate(XL);
  QRight.rotate(XR);

  const Geometry::Vec3D RotOrigin=Origin-Y*(length/2.0);
  int WI(buildIndex);
  for(size_t i=0;i<nWallLayers;i++)
    { 
      ModelSupport::buildPlane(SMap,WI+3,
			       RotOrigin-XL*(left*wallFrac[i]),XL);
      ModelSupport::buildPlane(SMap,WI+4,
			       RotOrigin+XR*(right*wallFrac[i]),XR);
      WI+=10;
    }

  int RI(buildIndex);
  for(size_t i=0;i<nRoofLayers;i++)
    {
      ModelSupport::buildPlane(SMap,RI+6,
			       Origin+Z*(height*roofFrac[i]),Z);
      RI+=10;
    }

  int FI(buildIndex);
  for(size_t i=0;i<nFloorLayers;i++)
    {
      ModelSupport::buildPlane(SMap,FI+5,
			       Origin-Z*(depth*floorFrac[i]),Z);
      FI+=10;
    }  
  return;
}

void
TriangleShield::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("TriangleShield","createObjects");

  HeadRule HR;
  
  const HeadRule& frontHR=getFrontRule();
  const HeadRule& backHR=getBackRule();
  const HeadRule backEndHR=(endWall>Geometry::zeroTol) ?
    HeadRule(SMap,buildIndex,-1002) : backHR;
  
  // Inner void is a single segment
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,
	   HR*frontHR+backEndHR);

  // Loop over all segments:
  HeadRule fbHR;
  int SI(buildIndex);
  int WI(buildIndex),RI(buildIndex),FI(buildIndex);    
  for(size_t index=0;index<nSeg;index++)
    {
      fbHR=(index) ?
	     ModelSupport::getHeadRule(SMap,SI,"2") :
	     frontHR;
      fbHR*= (index+1!=nSeg) ?
	       ModelSupport::getHeadRule(SMap,SI,"-12") :
	       backEndHR;
      SI+=10; 

      // Inner is a single component
      // Walls are contained:
      WI=buildIndex;
      for(size_t i=1;i<nWallLayers;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,WI,buildIndex,"13 -3 5M -6M");
	  System.addCell(cellIndex++,wallMat[i],0.0,HR*fbHR);
		 
	  HR=ModelSupport::getHeadRule(SMap,WI,buildIndex,"4 -14 5M -6M");
	  System.addCell(cellIndex++,wallMat[i],0.0,HR*fbHR);
	  WI+=10;
	}

      // Roof on top of walls are contained:
      RI=buildIndex;
      for(size_t i=1;i<nRoofLayers;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,RI,buildIndex,"3M -4M -16 6");
	  System.addCell(cellIndex++,roofMat[i],0.0,HR*fbHR);
	  RI+=10;
	}
    
      // Floor complete:
      FI=buildIndex;
      for(size_t i=1;i<nFloorLayers;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,FI,WI,"3M -4M -5 15");
	  System.addCell(cellIndex++,floorMat[i],0.0,HR*fbHR);
	  FI+=10;
	}
      
      // Left corner
      RI=buildIndex;
      for(size_t i=1;i<nRoofLayers;i++)
	{
	  WI=buildIndex;
	  for(size_t j=1;j<nWallLayers;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);

	      HR=ModelSupport::getHeadRule(SMap,WI,RI,"-3 13 6M -16M");
	      System.addCell(cellIndex++,mat,0.0,HR*fbHR);
	      
	      HR=ModelSupport::getHeadRule(SMap,WI,RI,"4 -14 6M -16M");
	      System.addCell(cellIndex++,mat,0.0,HR*fbHR);
	      WI+=10;
	    }
	  RI+=10;
	}
    }

  if (endWall>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,WI,FI,RI,"3 -4 5M -6N");
      HR*=ModelSupport::getSetHeadRule(SMap,buildIndex,"1002 1007");
      System.addCell(cellIndex++,defMat,0.0,HR*backHR);
      if (endVoid>Geometry::zeroTol)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1002 -1007");
	  makeCell("Void",System,cellIndex++,0,0.0,HR*backHR);
	}
    }
  
  // Outer
  HR=ModelSupport::getHeadRule(SMap,WI,FI,RI,"3 -4 5M -6N");
  addOuterSurf(HR*frontHR*backHR);

  return;
}

void
TriangleShield::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("TriangleShield","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);
  
  FixedComp::setConnect(2,Origin-X*left,-X);
  FixedComp::setConnect(3,Origin+X*right,X);
  FixedComp::setConnect(4,Origin-Z*depth,-Z);
  FixedComp::setConnect(5,Origin+Z*height,Z);

  const int WI(buildIndex+(static_cast<int>(nWallLayers)-1)*10);
  const int RI(buildIndex+(static_cast<int>(nRoofLayers)-1)*10);
  const int FI(buildIndex+(static_cast<int>(nFloorLayers)-1)*10);



  FixedComp::setLinkSurf(2,-SMap.realSurf(WI+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(WI+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(FI+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(RI+6));
  
  return;
}
  

HeadRule
TriangleShield::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("TriangleShield","getXSectionIn");
  HeadRule HR=
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  HR.populateSurf();
  
  return HR;
}
  
    
void
TriangleShield::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("TriangleShield","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem
