/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LineShield.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurInter.h"
#include "surfDIter.h"

#include "LineShield.h"

namespace constructSystem
{

LineShield::LineShield(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  shieldIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(shieldIndex+1),activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

LineShield::LineShield(const LineShield& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  shieldIndex(A.shieldIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
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
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      activeFront=A.activeFront;
      activeBack=A.activeBack;
      frontSurf=A.frontSurf;
      frontCut=A.frontCut;
      backSurf=A.backSurf;
      backCut=A.backCut;
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

  if (activeFront)
    {
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
LineShield::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("LineShield","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  Origin+=Y*(length/2.0);
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
  if (!activeFront)
    ModelSupport::buildPlane(SMap,shieldIndex+1,Origin-Y*(length/2.0),Y);
      
  if (!activeBack)
    ModelSupport::buildPlane(SMap,shieldIndex+2,Origin+Y*(length/2.0),Y);

  if (activeFront)
    removeFrontOverLap();
  
  const double segStep(length/static_cast<double>(nSeg));
  double segLen(-length/2.0);
  int SI(shieldIndex+10);
  for(size_t i=1;i<nSeg;i++)
    {
      segLen+=segStep;
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*segLen,Y);
      SI+=10;
    }

  int WI(shieldIndex);
  for(size_t i=0;i<nWallLayers;i++)
    {
      ModelSupport::buildPlane(SMap,WI+3,
			       Origin-X*(left*wallFrac[i]),X);
      ModelSupport::buildPlane(SMap,WI+4,
			       Origin+X*(right*wallFrac[i]),X);
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
LineShield::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("LineShield","createObjects");

  std::string Out;
  
  const std::string frontStr
    (activeFront ? frontSurf.display()+frontCut.display() : 
     ModelSupport::getComposite(SMap,shieldIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display()+backCut.display() : 
     ModelSupport::getComposite(SMap,shieldIndex," -2 "));

  // Inner void is a single segment
  Out=ModelSupport::getComposite(SMap,shieldIndex," 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out+frontStr+backStr);

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
	       backStr);
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
  // Outer
  Out=ModelSupport::getComposite(SMap,WI,FI,RI," 3 -4 5M -6N ");
  addOuterSurf(Out+frontStr+backStr);

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

  const int WI(shieldIndex+(static_cast<int>(nWallLayers)-1)*10);
  const int RI(shieldIndex+(static_cast<int>(nRoofLayers)-1)*10);
  const int FI(shieldIndex+(static_cast<int>(nFloorLayers)-1)*10);

  if (!activeFront)
    {
      FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(shieldIndex+1));      
    }
  else
    {
      FixedComp::setLinkSurf(0,frontSurf,1,frontCut,0);      
      FixedComp::setConnect
        (0,SurInter::getLinePoint(Origin,Y,frontSurf,frontCut),-Y);
    }
  
  if (!activeBack)
    {
      FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(shieldIndex+2));
    }
  else
    {
      FixedComp::setLinkSurf(1,backSurf,1,backCut,0);      
      FixedComp::setConnect
        (1,SurInter::getLinePoint(Origin,Y,backSurf,backCut),Y);
    }
  FixedComp::setLinkSurf(2,-SMap.realSurf(WI+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(WI+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(FI+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(RI+6));



  return;
}
  
void
LineShield::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("LineShield","setFront");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeFront=1;
  frontSurf=FC.getMainRule(sideIndex);
  frontCut=FC.getCommonRule(sideIndex);
  frontSurf.populateSurf();
  frontCut.populateSurf();
  
  return;
}

void
LineShield::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Set back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("LineShield","setBack");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeBack=1;
  backSurf=FC.getMainRule(sideIndex);
  backCut=FC.getCommonRule(sideIndex);
  backSurf.populateSurf();
  backCut.populateSurf();
  
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
  const std::string Out=
    ModelSupport::getComposite(SMap,shieldIndex," 3 -4 5 -6 ");
  HeadRule HR(Out);
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
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
