/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LineShield.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "CellMap.h"
#include "surfDIter.h"

#include "LineShield.h"

namespace constructSystem
{

LineShield::LineShield(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  shieldIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(shieldIndex+1),activeFront(0),activeBack(0),
  activeDivide(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

LineShield::~LineShield() 
  /*!
    Destructor
  */
{}

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
  ELog::EM<<"O == "<<Origin<<ELog::endDiag;
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

  int WI(shieldIndex);
  ELog::EM<<"NWall = "<<wallFrac.size()<<ELog::endDiag;
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
    (activeFront ? frontSurf.display() : 
     ModelSupport::getComposite(SMap,shieldIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display() : 
     ModelSupport::getComposite(SMap,shieldIndex," -2 "));
  const std::string divStr
    (activeDivide ? divideSurf.display() :  "");


  const std::string FBStr=frontStr+backStr+divStr;

  // Inner is a single component
  // Void + inner 
  Out=ModelSupport::getComposite(SMap,shieldIndex," 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+FBStr));
  addCell("Void",cellIndex-1);

  // Walls are contained:
  int WI(shieldIndex);
  for(size_t i=1;i<nWallLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,WI,shieldIndex," 13 -3 5M -6M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[i],0.0,Out+FBStr));
            
      Out=ModelSupport::getComposite(SMap,WI,shieldIndex," 4 -14 5M -6M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[i],0.0,Out+FBStr));
      WI+=10;
    }
 
  // Roof on top of walls are contained:
  int SI(shieldIndex);
  for(size_t i=1;i<nRoofLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,SI,shieldIndex," 3M -4M -16 6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,roofMat[i],0.0,Out+FBStr));
      SI+=10;
    }

  // Floor complete:
  int FI(shieldIndex);
  for(size_t i=1;i<nRoofLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,FI,WI," 3M -4M -5 15 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,floorMat[i],0.0,Out+FBStr));
      FI+=10;
    }
  
  // Outer
  Out=ModelSupport::getComposite(SMap,WI,SI,FI," 3 -4 5M -6N ");
  addOuterSurf(Out+FBStr);

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

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(2,Origin-X*left,-X);
  FixedComp::setConnect(3,Origin+X*right,X);
  FixedComp::setConnect(4,Origin-Z*depth,-Z);
  FixedComp::setConnect(5,Origin+Z*height,Z);

  if (activeFront)
    FixedComp::setLinkSurf(0,frontSurf);      
  else
    FixedComp::setLinkSurf(0,-SMap.realSurf(shieldIndex+1));      

  if (activeBack)
    FixedComp::setLinkSurf(1,backSurf);      
  else
    FixedComp::setLinkSurf(1,SMap.realSurf(shieldIndex+2));      

  FixedComp::setLinkSurf(2,-SMap.realSurf(shieldIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(shieldIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(shieldIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(shieldIndex+6));
  
  return;
}

void
LineShield::setDivider(const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Set divider surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("LineShield","setDivider");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeDivide=1;
  if (sideIndex>0)
    {
      const size_t SI(static_cast<size_t>(sideIndex-1));
      divideSurf=FC.getCommonRule(SI);
    }
  else
    {
      const size_t SI(static_cast<size_t>(-sideIndex-1));
      divideSurf=FC.getCommonRule(SI);
      divideSurf.makeComplement();
    }
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
  if (sideIndex>0)
    {
      const size_t SI(static_cast<size_t>(sideIndex-1));
      frontSurf=FC.getMainRule(SI);
    }
  else
    {
      const size_t SI(static_cast<size_t>(-sideIndex-1));
      frontSurf=FC.getMainRule(SI);
      frontSurf.makeComplement();
    }
  return;
}
  
void
LineShield::setBack(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set Back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("LineShield","setBack");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeBack=1;
  if (sideIndex>0)
    backSurf=FC.getMainRule(static_cast<size_t>(sideIndex-1));
  else
    {
      backSurf=FC.getMainRule(static_cast<size_t>(-sideIndex-1));
      backSurf.makeComplement();
    }
  return;
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
