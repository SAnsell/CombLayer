/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/MultiChannel.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "MultiChannel.h"


namespace constructSystem
{

MultiChannel::MultiChannel(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,2),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  collIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(collIndex+1),setFlag(0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  

void
MultiChannel::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MultiChannel","populate");

  FixedOffset::populate(Control);

  nBlades=Control.EvalVar<size_t>(keyName+"NBlades");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  length=Control.EvalVar<double>(keyName+"Length");

  
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  
  return;
}

void
MultiChannel::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("MultiChannel","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}


void
MultiChannel::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("MultiChannel","createSurface");

  ModelSupport::buildPlane(SMap,collIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,collIndex+2,Origin+Y*(length/2.0),Y);
  
  return;
}

void
MultiChannel::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("MultiChannel","createObjects");
  std::string Out;

  if ((setFlag & 2) !=2)
    throw ColErr::EmptyContainer("outerStruct not set");

  Out=ModelSupport::getComposite(SMap,collIndex,"1 -2");
  // Out+=innerStruct.display()+outerStruct.display();
  
  // System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  // addCell("Main",cellIndex-1);
  
  // Out=ModelSupport::getComposite(SMap,collIndex,"1 -2");
  // addOuterSurf(Out);
  return;
}

void
MultiChannel::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("MultiChannel","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(collIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(collIndex+2));      
  
  return;
}

void
MultiChannel::setFaces(const int BS,const int TS)
  /*!
    Set the top/base surface numbers
    \param BS :: Base surface number
    \param TS :: Top surface number
  */
{
  ELog::RegMethod RegA("MultiChannel","setFaces");

  baseSurf=BS;
  topSurf=TS;
  setFlag ^= 1;
  return;
}

void
MultiChannel::setFaces(const attachSystem::FixedComp& FC,
		       const long int BS,const long int TS)
  /*!
    Set the top/base surface numbers
    \param FC :: Fixed Component to use
    \param BS :: Base surface link point
    \param TS :: Top surface link point
  */
{
  ELog::RegMethod RegA("MultiChannel","setFaces<FC>");

  baseSurf=FC.getSignedLinkSurf(BS);
  topSurf=FC.getSignedLinkSurf(TS);
  setFlag ^= 1;
  return;
}
  
void
MultiChannel::setDivider(const HeadRule& HR)
  /*!
    Set the inner volume
    \param HR :: Headrule of inner surfaces
  */
{
  ELog::RegMethod RegA("MultiChannel","setInner");

  divider=HR;
  setFlag ^= 2;
  return;
}

void
MultiChannel::setLeftRight(const HeadRule& LR,const HeadRule& RR)
  /*!
    Set the left/right sides
    \param LR :: HeadRule of left surface
    \param RR :: HeadRule of right surface
  */
{
  ELog::RegMethod RegA("MultiChannel","setLeftRight");

  leftStruct=LR;
  rightStruct=RR;
  setFlag ^= 4;
  return;
}

void
MultiChannel::setLeftRight(const FixedComp& FCA,const long int lIndex,
			   const FixedComp& FCB,const long int rIndex)

  /*!
    Set the left/right sides
    \param FCA :: Left fixed comp
    \param lIndex :: left side index
    \param FCB :: Left fixed comp
    \param rIndex :: right side index
  */
{
  ELog::RegMethod RegA("MultiChannel","setLeftRight(FC)");

  leftStruct=FCA.getSignedFullRule(lIndex);
  rightStruct=FCB.getSignedFullRule(rIndex);
  setFlag ^= 4;
  return;
}


  
  
void
MultiChannel::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("MultiChannel","createAllNoPopulate");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
