/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/IrradElement.cxx
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
 ****************************************************************************/
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <climits>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <boost/multi_array.hpp>

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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "RElement.h"
#include "IrradElement.h"

namespace delftSystem
{


IrradElement::IrradElement(const size_t XI,const size_t YI,
			   const std::string& Key) :
  RElement(XI,YI,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: Grid index  [A-F]
    \param YI :: Grid index 
    \param Key :: Keyname for block
  */
{}

IrradElement::IrradElement(const IrradElement& A) : 
  RElement(A),
  Width(A.Width),Depth(A.Depth),beLen(A.beLen),
  topPlug(A.topPlug),topLocator(A.topLocator),endStop(A.endStop),
  plugThick(A.plugThick),plugZOffset(A.plugZOffset),
  locThick(A.locThick),sampleRadius(A.sampleRadius),
  sampleXOff(A.sampleXOff),sampleZOff(A.sampleZOff),
  vacRadius(A.vacRadius),caseRadius(A.caseRadius),
  coreRadius(A.coreRadius),outerRadius(A.outerRadius),
  waterRadius(A.waterRadius),beMat(A.beMat),pipeMat(A.pipeMat),
  sampleMat(A.sampleMat),waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: IrradElement to copy
  */
{}

IrradElement&
IrradElement::operator=(const IrradElement& A)
  /*!
    Assignment operator
    \param A :: IrradElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RElement::operator=(A);
      Width=A.Width;
      Depth=A.Depth;
      beLen=A.beLen;
      topPlug=A.topPlug;
      topLocator=A.topLocator;
      endStop=A.endStop;
      plugThick=A.plugThick;
      plugZOffset=A.plugZOffset;
      locThick=A.locThick;
      sampleRadius=A.sampleRadius;
      sampleXOff=A.sampleXOff;
      sampleZOff=A.sampleZOff;
      vacRadius=A.vacRadius;
      caseRadius=A.caseRadius;
      coreRadius=A.coreRadius;
      outerRadius=A.outerRadius;
      waterRadius=A.waterRadius;
      beMat=A.beMat;
      pipeMat=A.pipeMat;
      sampleMat=A.sampleMat;
      waterMat=A.waterMat;
    }
  return *this;
}

void
IrradElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("IrradElement","populate");

  Width=ReactorGrid::getElement<double>
    (Control,baseName+"Width",XIndex,YIndex);
  Depth=ReactorGrid::getElement<double>
    (Control,baseName+"Depth",XIndex,YIndex);

  endStop=ReactorGrid::getElement<double>
    (Control,baseName+"EndStop",XIndex,YIndex);
  topPlug=ReactorGrid::getElement<double>
    (Control,baseName+"TopPlug",XIndex,YIndex);
  beLen=ReactorGrid::getElement<double>
    (Control,baseName+"BeLen",XIndex,YIndex);
  topLocator=ReactorGrid::getElement<double>
    (Control,baseName+"TopLocator",XIndex,YIndex);
  plugZOffset=ReactorGrid::getElement<double>
    (Control,baseName+"PlugZOffset",XIndex,YIndex);
  plugThick=ReactorGrid::getElement<double>
    (Control,baseName+"PlugThick",XIndex,YIndex);
  locThick=ReactorGrid::getElement<double>
    (Control,baseName+"LocatorThick",XIndex,YIndex);

  
  sampleRadius=ReactorGrid::getElement<double>
    (Control,baseName+"SampleRad",XIndex,YIndex);
  sampleXOff=ReactorGrid::getElement<double>
    (Control,baseName+"SampleXOffset",XIndex,YIndex);
  sampleZOff=ReactorGrid::getElement<double>
    (Control,baseName+"SampleZOffset",XIndex,YIndex);

  vacRadius=ReactorGrid::getElement<double>
    (Control,baseName+"VacRadius",XIndex,YIndex);
  caseRadius=ReactorGrid::getElement<double>
    (Control,baseName+"CaseRadius",XIndex,YIndex);
  coreRadius=ReactorGrid::getElement<double>
    (Control,baseName+"CoreRadius",XIndex,YIndex);
  outerRadius=ReactorGrid::getElement<double>
    (Control,baseName+"OuterRadius",XIndex,YIndex);
  waterRadius=ReactorGrid::getElement<double>
    (Control,baseName+"WaterRadius",XIndex,YIndex);
  
  beMat=ReactorGrid::getMatElement
    (Control,baseName+"BeMat",XIndex,YIndex);
  pipeMat=ReactorGrid::getMatElement
    (Control,baseName+"PipeMat",XIndex,YIndex);
  sampleMat=ReactorGrid::getMatElement
    (Control,baseName+"SampleMat",XIndex,YIndex);
  waterMat=ReactorGrid::getMatElement
    (Control,baseName+"WaterMat",XIndex,YIndex);
  
  return;
}


void
IrradElement::createSurfaces()
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
  */
{  
  ELog::RegMethod RegA("IrradElement","createSurface");
  
  ModelSupport::buildSphere(SMap,buildIndex+7,
	    Origin-X*sampleXOff+Z*sampleZOff,sampleRadius);
  ModelSupport::buildSphere(SMap,buildIndex+8,
	    Origin+X*sampleXOff+Z*sampleZOff,sampleRadius);
  
  ModelSupport::buildCylinder(SMap,buildIndex+17,
  	    Origin-X*sampleXOff,Z,vacRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+18,
  	    Origin+X*sampleXOff,Z,vacRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+27,
  	    Origin-X*sampleXOff,Z,caseRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+28,
  	    Origin+X*sampleXOff,Z,caseRadius);

  // Core
  ModelSupport::buildCylinder(SMap,buildIndex+37,
  	    Origin,Z,coreRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+47,
  	    Origin,Z,outerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+57,
  	    Origin,Z,waterRadius);

  // Planes [OUTER]:
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*Depth/2.0,Y); 
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*Width/2.0,X);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(Depth/2.0-locThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(Depth/2.0-locThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(Width/2.0-locThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(Width/2.0-locThick),X);


  double baseShift(endStop);
  makeShiftedSurf(SMap,"BasePlate",buildIndex+15,Z,baseShift);
  baseShift+=beLen;
  makeShiftedSurf(SMap,"BasePlate",buildIndex+25,Z,baseShift);
  baseShift+=topPlug;
  makeShiftedSurf(SMap,"BasePlate",buildIndex+35,Z,baseShift);
  baseShift+=topLocator;
  makeShiftedSurf(SMap,"BasePlate",buildIndex+45,Z,baseShift);

  // Inner tube
  ModelSupport::buildPlane(SMap,buildIndex+115,
			   Origin+Z*plugZOffset,Z);
  ModelSupport::buildPlane(SMap,buildIndex+125,
			   Origin+Z*(plugZOffset+plugThick),Z);

  return;
}

void
IrradElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("IrradElement","createObjects");

  const HeadRule& baseHR=getRule("BasePlate");
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 -45");
  addOuterSurf(HR*baseHR);      

  // Two samples:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  System.addCell(cellIndex++,sampleMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8");
  System.addCell(cellIndex++,sampleMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"125 -45 -17 7");
  System.addCell(cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"125 -45 -18 8");
  System.addCell(cellIndex++,0,0.0,HR);
  // Cladding 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"125 -45 17 -27");
  System.addCell(cellIndex++,pipeMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"125 -45 18 -28");
  System.addCell(cellIndex++,pipeMat,0.0,HR);
    
  // -- Change to full bore:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"125 -45 -37 27 28");
  System.addCell(cellIndex++,waterMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"125 -45 -47 37");
  System.addCell(cellIndex++,pipeMat,0.0,HR);
  
  // Base Cape
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"115 -125 -47");
  System.addCell(cellIndex++,pipeMat,0.0,HR);

  // Water Surround
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-45 -57 (-115:47)");
  System.addCell(cellIndex++,waterMat,0.0,HR*baseHR);
  // Be surround
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 -25 15 57");
  System.addCell(cellIndex++,beMat,0.0,HR);
  // End Cap
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 -15 57");
  System.addCell(cellIndex++,pipeMat,0.0,HR*baseHR);
  // Top Cap
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 25 -35 57");
  System.addCell(cellIndex++,pipeMat,0.0,HR);
  // Locator Inner Water
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 13 -14 35 -45 57");
  System.addCell(cellIndex++,waterMat,0.0,HR);
  // Locator OuterWalls
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "1 -2 3 -4 (-11:12:-13:14) 35 -45");
  System.addCell(cellIndex++,pipeMat,0.0,HR);


  return;
}


void
IrradElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{

  return;
}

void
IrradElement::createAll(Simulation& System,
			const attachSystem::FixedComp& RG,
			const long int sideIndex)
  /*!
    Global creation of the element block
    \param System :: Simulation to add vessel to
    \param RG :: Fixed Unit
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("IrradElement","createAll");
  populate(System.getDataBase());

  createUnitVector(RG,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
