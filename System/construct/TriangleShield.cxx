/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TriangleShield.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurInter.h"
#include "surfDIter.h"

#include "TriangleShield.h"

namespace constructSystem
{

TriangleShield::TriangleShield(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  shieldIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(shieldIndex+1)
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
  
  FixedOffset::populate(Control);

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
	  ModelSupport::populateDivide(Control,nRoofLayers,keyName+"EndMat",
				       defMat,endMat);
	  ModelSupport::populateDivideLen(Control,nRoofLayers,keyName+"EndLen",
					  endWall,endFrac);
	  endFrac.push_back(1.0);
	}
    }
  
  defMat=ModelSupport::EvalDefMat<int>(Control,keyName+"DefMat",0);

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
TriangleShield::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("TriangleShield","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  Origin+=Y*(length/2.0);
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
      ModelSupport::buildPlane(SMap,shieldIndex+1,Origin-Y*(length/2.0),Y);
      setFront(SMap.realSurf(shieldIndex+1));
    }

  if (!FrontBackCut::backActive())
    {
      ModelSupport::buildPlane(SMap,shieldIndex+2,Origin+Y*(length/2.0),Y);
      setBack(-SMap.realSurf(shieldIndex+2));
      if (endWall>Geometry::zeroTol)
	{
	  ModelSupport::buildPlane(SMap,shieldIndex+1002,
				   Origin+Y*(length/2.0-endWall),Y);
	  if (endVoid>Geometry::zeroTol)
	    ModelSupport::buildCylinder(SMap,shieldIndex+1007,
					Origin,Y,endVoid);
	}
	    
    }
  //create back cut for wall
  //  const int BSurf=getBackRule().getPrimarySurface();
  
  const double segStep(length/static_cast<double>(nSeg));
  double segLen(-length/2.0);
  int SI(shieldIndex+10);
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
  int WI(shieldIndex);
  for(size_t i=0;i<nWallLayers;i++)
    { 
      ModelSupport::buildPlane(SMap,WI+3,
			       RotOrigin-XL*(left*wallFrac[i]),XL);
      ModelSupport::buildPlane(SMap,WI+4,
			       RotOrigin+XR*(right*wallFrac[i]),XR);
      WI+=10;
    }

  int RI(shieldIndex);
  for(size_t i=0;i<nRoofLayers;i++)
    {
      ModelSupport::buildPlane(SMap,RI+6,
			       Origin+Z*(height*roofFrac[i]),Z);
      RI+=10;
    }

  int FI(shieldIndex);
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

  std::string Out;
  
  const std::string frontStr=frontRule();
  const std::string backStr=backRule();
  const std::string backEndStr=(endWall>Geometry::zeroTol) ?
    ModelSupport::getComposite(SMap,shieldIndex," -1002 " ) :
    backStr;
  
  // Inner void is a single segment
  Out=ModelSupport::getComposite(SMap,shieldIndex," 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+
				   frontStr+backEndStr));
  addCell("Void",cellIndex-1);

  // Loop over all segments:
  std::string FBStr;
  int SI(shieldIndex);
  int WI(shieldIndex),RI(shieldIndex),FI(shieldIndex);    
  for(size_t index=0;index<nSeg;index++)
    {
      FBStr=((index) ?
	     ModelSupport::getComposite(SMap,SI," 2 ") :
	     frontStr);
      FBStr+= ((index+1!=nSeg) ?
	       ModelSupport::getComposite(SMap,SI," -12 ") :
	       backEndStr);
      SI+=10; 

      // Inner is a single component
      // Walls are contained:
      WI=shieldIndex;
      for(size_t i=1;i<nWallLayers;i++)
	{
	  Out=ModelSupport::getComposite(SMap,WI,shieldIndex," 13 -3 5M -6M ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,wallMat[i],0.0,Out+FBStr));
		 
	  Out=ModelSupport::getComposite(SMap,WI,shieldIndex," 4 -14 5M -6M ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,wallMat[i],0.0,Out+FBStr));
	  WI+=10;
	}

      // Roof on top of walls are contained:
      RI=shieldIndex;
      for(size_t i=1;i<nRoofLayers;i++)
	{
	  Out=ModelSupport::getComposite(SMap,RI,shieldIndex," 3M -4M -16 6 ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,roofMat[i],0.0,Out+FBStr));
	  RI+=10;
	}
    
      // Floor complete:
      FI=shieldIndex;
      for(size_t i=1;i<nFloorLayers;i++)
	{
	  Out=ModelSupport::getComposite(SMap,FI,WI," 3M -4M -5 15 ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,floorMat[i],0.0,Out+FBStr));
	  FI+=10;
	}
      
      // Left corner
      RI=shieldIndex;
      for(size_t i=1;i<nRoofLayers;i++)
	{
	  WI=shieldIndex;
	  for(size_t j=1;j<nWallLayers;j++)
	    {
	      const int mat((i>j) ? roofMat[i] : wallMat[j]);

	      Out=ModelSupport::getComposite(SMap,WI,RI," -3 13 6M -16M ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+FBStr));
	      
	      Out=ModelSupport::getComposite(SMap,WI,RI," 4 -14 6M -16M ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+FBStr));
	      WI+=10;
	    }
	  RI+=10;
	}
    }

  if (endWall>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,WI,FI,RI," 3 -4 5M -6N ");
      Out+=ModelSupport::getSetComposite(SMap,shieldIndex," 1002 1007 " );
      Out+=backStr;
      System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
      if (endVoid>Geometry::zeroTol)
	{
	  Out=ModelSupport::getComposite(SMap,shieldIndex," 1002 -1007 " );
	  Out+=backStr;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
	  addCell("Void",cellIndex-1);	  
	}
    }
  
  // Outer
  Out=ModelSupport::getComposite(SMap,WI,FI,RI," 3 -4 5M -6N ");
  addOuterSurf(Out+frontStr+backStr);

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

  const int WI(shieldIndex+(static_cast<int>(nWallLayers)-1)*10);
  const int RI(shieldIndex+(static_cast<int>(nRoofLayers)-1)*10);
  const int FI(shieldIndex+(static_cast<int>(nFloorLayers)-1)*10);



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
  const std::string Out=
    ModelSupport::getComposite(SMap,shieldIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
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
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
