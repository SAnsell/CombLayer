/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/IrradElement.cxx
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
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
    (Control,keyName+"Width",XIndex,YIndex);
  Depth=ReactorGrid::getElement<double>
    (Control,keyName+"Depth",XIndex,YIndex);

  endStop=ReactorGrid::getElement<double>
    (Control,keyName+"EndStop",XIndex,YIndex);
  topPlug=ReactorGrid::getElement<double>
    (Control,keyName+"TopPlug",XIndex,YIndex);
  beLen=ReactorGrid::getElement<double>
    (Control,keyName+"BeLen",XIndex,YIndex);
  topLocator=ReactorGrid::getElement<double>
    (Control,keyName+"TopLocator",XIndex,YIndex);
  plugZOffset=ReactorGrid::getElement<double>
    (Control,keyName+"PlugZOffset",XIndex,YIndex);
  plugThick=ReactorGrid::getElement<double>
    (Control,keyName+"PlugThick",XIndex,YIndex);
  locThick=ReactorGrid::getElement<double>
    (Control,keyName+"LocatorThick",XIndex,YIndex);

  
  sampleRadius=ReactorGrid::getElement<double>
    (Control,keyName+"SampleRad",XIndex,YIndex);
  sampleXOff=ReactorGrid::getElement<double>
    (Control,keyName+"SampleXOffset",XIndex,YIndex);
  sampleZOff=ReactorGrid::getElement<double>
    (Control,keyName+"SampleZOffset",XIndex,YIndex);

  vacRadius=ReactorGrid::getElement<double>
    (Control,keyName+"VacRadius",XIndex,YIndex);
  caseRadius=ReactorGrid::getElement<double>
    (Control,keyName+"CaseRadius",XIndex,YIndex);
  coreRadius=ReactorGrid::getElement<double>
    (Control,keyName+"CoreRadius",XIndex,YIndex);
  outerRadius=ReactorGrid::getElement<double>
    (Control,keyName+"OuterRadius",XIndex,YIndex);
  waterRadius=ReactorGrid::getElement<double>
    (Control,keyName+"WaterRadius",XIndex,YIndex);
  
  beMat=ReactorGrid::getMatElement
    (Control,keyName+"BeMat",XIndex,YIndex);
  pipeMat=ReactorGrid::getMatElement
    (Control,keyName+"PipeMat",XIndex,YIndex);
  sampleMat=ReactorGrid::getMatElement
    (Control,keyName+"SampleMat",XIndex,YIndex);
  waterMat=ReactorGrid::getMatElement
    (Control,keyName+"WaterMat",XIndex,YIndex);
  
  return;
}

void
IrradElement::createUnitVector(const attachSystem::FixedComp& FC,
			       const Geometry::Vec3D& OG)
  /*!
    Create the unit vectors
    - Y Down the beamline

    \param FC :: Reactor Grid Unit
    \param OG :: Origin
  */
{
  ELog::RegMethod RegA("IrradElement","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  Origin=OG;
  return;
}

void
IrradElement::createSurfaces(const attachSystem::FixedComp& RG)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
  */
{  
  ELog::RegMethod RegA("IrradElement","createSurface");
  
  ModelSupport::buildSphere(SMap,surfIndex+7,
	    Origin-X*sampleXOff+Z*sampleZOff,sampleRadius);
  ModelSupport::buildSphere(SMap,surfIndex+8,
	    Origin+X*sampleXOff+Z*sampleZOff,sampleRadius);
  
  ModelSupport::buildCylinder(SMap,surfIndex+17,
  	    Origin-X*sampleXOff,Z,vacRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+18,
  	    Origin+X*sampleXOff,Z,vacRadius);

  ModelSupport::buildCylinder(SMap,surfIndex+27,
  	    Origin-X*sampleXOff,Z,caseRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+28,
  	    Origin+X*sampleXOff,Z,caseRadius);

  // Core
  ModelSupport::buildCylinder(SMap,surfIndex+37,
  	    Origin,Z,coreRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+47,
  	    Origin,Z,outerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+57,
  	    Origin,Z,waterRadius);

  // Planes [OUTER]:
  
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*Depth/2.0,Y); 
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*Width/2.0,X);

  ModelSupport::buildPlane(SMap,surfIndex+11,
			   Origin-Y*(Depth/2.0-locThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,
			   Origin+Y*(Depth/2.0-locThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+13,
			   Origin-X*(Width/2.0-locThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,
			   Origin+X*(Width/2.0-locThick),X);

  SMap.addMatch(surfIndex+5,RG.getLinkSurf(5));
  Geometry::Vec3D BaseZ(RG.getLinkPt(5));
  
  BaseZ+=Z*endStop;
  ModelSupport::buildPlane(SMap,surfIndex+15,BaseZ,Z);
  BaseZ+=Z*beLen;
  ModelSupport::buildPlane(SMap,surfIndex+25,BaseZ,Z);  // top of Be
  BaseZ+=Z*topPlug;
  ModelSupport::buildPlane(SMap,surfIndex+35,BaseZ,Z);
  BaseZ+=Z*topLocator;
  ModelSupport::buildPlane(SMap,surfIndex+45,BaseZ,Z);

  // Inner tube
  ModelSupport::buildPlane(SMap,surfIndex+115,
			   Origin+Z*plugZOffset,Z);
  ModelSupport::buildPlane(SMap,surfIndex+125,
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

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -45 ");
  addOuterSurf(Out);      

  // Two samples:
  Out=ModelSupport::getComposite(SMap,surfIndex," -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,sampleMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," -8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,sampleMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,surfIndex," 125 -45 -17 7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 125 -45 -18 8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // Cladding 
  Out=ModelSupport::getComposite(SMap,surfIndex,"125 -45 17 -27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex,"125 -45 18 -28");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));
    
  // -- Change to full bore:
  Out=ModelSupport::getComposite(SMap,surfIndex,"125 -45 -37 27 28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex,"125 -45 -47 37 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));
  
  // Base Cape
  Out=ModelSupport::getComposite(SMap,surfIndex,"115 -125 -47");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));

  // Water Surround
  Out=ModelSupport::getComposite(SMap,surfIndex,"5 -45 -57 (-115:47)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  // Be surround
  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 3 -4 -25 15 57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,beMat,0.0,Out));
  // End Cap
  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 3 -4 5 -15 57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));
  // Top Cap
  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 3 -4 25 -35 57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));
  // Locator Inner Water
  Out=ModelSupport::getComposite(SMap,surfIndex,"11 -12 13 -14 35 -45 57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  // Locator OuterWalls
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 "1 -2 3 -4 (-11:12:-13:14) 35 -45");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMat,0.0,Out));


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
			const Geometry::Vec3D& OG,
			const FuelLoad&)
  /*!
    Global creation of the element block
    \param System :: Simulation to add vessel to
    \param RG :: Fixed Unit
    \param OG :: Origin
    \param :: FuelLoad not used [not fuel!]
  */
{
  ELog::RegMethod RegA("IrradElement","createAll");
  populate(System.getDataBase());

  createUnitVector(RG,OG);
  createSurfaces(RG);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
