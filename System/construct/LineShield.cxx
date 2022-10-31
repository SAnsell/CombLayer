/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LineShield.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurInter.h"
#include "surfDIter.h"

#include "LineShield.h"

namespace constructSystem
{

LineShield::LineShield(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

LineShield::LineShield(const LineShield& A) : 
  attachSystem::FixedOffset(A),
  attachSystem::ContainedComp(A),
  attachSystem::FrontBackCut(A),
  attachSystem::CellMap(A),
  length(A.length),left(A.left),right(A.right),
  height(A.height),depth(A.depth),defMat(A.defMat),
  nSeg(A.nSeg),nWallLayers(A.nWallLayers),
  wallFrac(A.wallFrac),wallMat(A.wallMat),
  nRoofLayers(A.nRoofLayers),roofFrac(A.roofFrac),
  roofMat(A.roofMat),nFloorLayers(A.nFloorLayers),
  floorFrac(A.floorFrac),floorMat(A.floorMat)
  /*!
    Copy constructor
    \param A :: LineShield to copy
  */
{}

LineShield&
LineShield::operator=(const LineShield& A)
  /*!
    Assignment operator
    \param A :: LineShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      left=A.left;
      right=A.right;
      height=A.height;
      depth=A.depth;
      defMat=A.defMat;
      nSeg=A.nSeg;
      nWallLayers=A.nWallLayers;
      wallFrac=A.wallFrac;
      wallMat=A.wallMat;
      nRoofLayers=A.nRoofLayers;
      roofFrac=A.roofFrac;
      roofMat=A.roofMat;
      nFloorLayers=A.nFloorLayers;
      floorFrac=A.floorFrac;
      floorMat=A.floorMat;
    }
  return *this;
}

LineShield::~LineShield() 
  /*!
    Destructor
  */
{}

void
LineShield::removeFrontOverLap()
  /*!
    Remove segments that are completly covered by the
    active front.
  */
{
  ELog::RegMethod RegA("LineShield","removeFrontOverLap");

  if (isActive("front"))
    {
      HeadRule frontSurf=getRule("front");
      size_t index(1);
      const double segStep(length/static_cast<double>(nSeg));
      // note : starts one step ahead of front.
      Geometry::Vec3D SP(Origin-Y*(length/2.0-segStep));
      frontSurf.populateSurf();
      while(index<nSeg && !frontSurf.isValid(SP))
        {
          SP+=Y*segStep;
          index++;
        }
      index--;
      if (index>0)
        {
	  const double LUnit=segStep*static_cast<double>(index);
          length-=LUnit;
          nSeg-=index;
          Origin+=Y*(LUnit/2.0);
          ELog::EM<<"Removal["<<keyName<<"] of Active segment == "
                  <<index<<ELog::endDiag;
        }
    }
  
  return;
}
  
void
LineShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("LineShield","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  left=Control.EvalVar<double>(keyName+"Left");
  right=Control.EvalVar<double>(keyName+"Right");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  
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
LineShield::createSurfaces()
  /*!
    Create the surfaces. Note that layers is not used
    because we want to break up the objects into sub components
  */
{
  ELog::RegMethod RegA("LineShield","createSurfaces");

  // Inner void
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  else
    removeFrontOverLap();
  
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }
  
  const double segStep(length/static_cast<double>(nSeg));
  double segLen(-length/2.0);
  int SI(buildIndex+10);
  for(size_t i=1;i<nSeg;i++)
    {
      segLen+=segStep;
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*segLen,Y);
      SI+=10;
    }

  int WI(buildIndex);
  for(size_t i=0;i<nWallLayers;i++)
    {
      ModelSupport::buildPlane(SMap,WI+3,
			       Origin-X*(left*wallFrac[i]),X);
      ModelSupport::buildPlane(SMap,WI+4,
			       Origin+X*(right*wallFrac[i]),X);
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
LineShield::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("LineShield","createObjects");

  HeadRule HR;
  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  // Inner void is a single segment
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,HR*frontHR*backHR);

  // Loop over all segments:
  HeadRule FBHR;
  int SI(buildIndex);
  int WI(buildIndex),RI(buildIndex),FI(buildIndex);    
  for(size_t index=0;index<nSeg;index++)
    {
      FBHR=  (index) ? HeadRule(SMap,SI,2) : frontHR;
      FBHR*= (index+1!=nSeg) ?  HeadRule(SMap,SI,-12) : backHR;
      SI+=10; 

      // Inner is a single component
      // Walls are contained:
      WI=buildIndex;
      for(size_t i=1;i<nWallLayers;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,WI,buildIndex,"13 -3 5M -6M");
	  System.addCell(cellIndex++,wallMat[i],0.0,HR*FBHR);
		 
	  HR=ModelSupport::getHeadRule(SMap,WI,buildIndex,"4 -14 5M -6M");
	  System.addCell(cellIndex++,wallMat[i],0.0,HR*FBHR);
	  WI+=10;
	}

      // Roof on top of walls are contained:
      RI=buildIndex;
      for(size_t i=1;i<nRoofLayers;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,RI,buildIndex,"3M -4M -16 6");
	  System.addCell(cellIndex++,roofMat[i],0.0,HR*FBHR);
	  RI+=10;
	}
    
      // Floor complete:
      FI=buildIndex;
      for(size_t i=1;i<nFloorLayers;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,FI,WI," 3M -4M -5 15");
	  System.addCell(cellIndex++,floorMat[i],0.0,HR*FBHR);
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
	      System.addCell(cellIndex++,mat,0.0,HR*FBHR);
	      
	      HR=ModelSupport::getHeadRule(SMap,WI,RI,"4 -14 6M -16M");
	      System.addCell(cellIndex++,mat,0.0,HR*FBHR);
	      WI+=10;
	    }
	  RI+=10;
	}
    }
  // Outer
  HR=ModelSupport::getHeadRule(SMap,WI,FI,RI,"3 -4 5M -6N");
  addOuterSurf(HR*frontHR*backHR);

  return;
}

void
LineShield::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("LineShield","createLinks");

  FixedComp::setConnect(2,Origin-X*left,-X);
  FixedComp::setConnect(3,Origin+X*right,X);
  FixedComp::setConnect(4,Origin-Z*depth,-Z);
  FixedComp::setConnect(5,Origin+Z*height,Z);

  const int WI(buildIndex+(static_cast<int>(nWallLayers)-1)*10);
  const int RI(buildIndex+(static_cast<int>(nRoofLayers)-1)*10);
  const int FI(buildIndex+(static_cast<int>(nFloorLayers)-1)*10);

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setLinkSurf(2,-SMap.realSurf(WI+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(WI+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(FI+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(RI+6));
  return;
}
  


HeadRule
LineShield::getXSectionIn() const
  /*!
    Get the line shield inner void section
    \return HeadRule of cross-section
   */
{
  ELog::RegMethod RegA("LineShield","getXSectionIn");
  HeadRule HR=
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  HR.populateSurf();
  return HR;
}
  
    
void
LineShield::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("LineShield","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,length/2.0);

  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
