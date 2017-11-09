/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/ControlElement.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfDIter.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "RElement.h"
#include "FuelElement.h"
#include "ControlElement.h"

#include "debugMethod.h"

namespace delftSystem
{


ControlElement::ControlElement(const size_t XI,const size_t YI,
			       const std::string& Key,
			       const std::string& CKey) :
  FuelElement(XI,YI,Key),
  attachSystem::ContainedGroup("Track","Rod","Cap"),cntlKey(CKey),
  controlIndex(ModelSupport::objectRegister::Instance().
	       cell(ReactorGrid::getElementName(CKey,XI,YI)))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: Grid position
    \param YI :: Grid position
    \param Key :: Keyname for fuel part of cell
    \param CKey :: Keyname for control part of cell
  */
{}


ControlElement::ControlElement(const ControlElement& A) : 
  FuelElement(A),attachSystem::ContainedGroup(A),
  cntlKey(A.cntlKey),controlIndex(A.controlIndex),
  cStartIndex(A.cStartIndex),cEndIndex(A.cEndIndex),
  lift(A.lift),voidRadius(A.voidRadius),
  voidOffset(A.voidOffset),absThick(A.absThick),
  caseThick(A.caseThick),waterThick(A.waterThick),
  plateThick(A.plateThick),innerLength(A.innerLength),
  outerLength(A.outerLength),outerCapLen(A.outerCapLen),
  endCap(A.endCap),absMat(A.absMat),cladMat(A.cladMat)
  /*!
    Copy constructor
    \param A :: ControlElement to copy
  */
{}

ControlElement&
ControlElement::operator=(const ControlElement& A)
  /*!
    Assignment operator
    \param A :: ControlElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FuelElement::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      cStartIndex=A.cStartIndex;
      cEndIndex=A.cEndIndex;
      lift=A.lift;
      voidRadius=A.voidRadius;
      voidOffset=A.voidOffset;
      absThick=A.absThick;
      caseThick=A.caseThick;
      waterThick=A.waterThick;
      plateThick=A.plateThick;
      innerLength=A.innerLength;
      outerLength=A.outerLength;
      outerCapLen=A.outerCapLen;
      endCap=A.endCap;
      absMat=A.absMat;
      cladMat=A.cladMat;
    }
  return *this;
}

void
ControlElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("ControlElement","populate");
  
  FuelElement::populate(Control);
  cStartIndex=ReactorGrid::getElement<size_t>(Control,cntlKey+"CStartIndex",
					   XIndex,YIndex);
  cEndIndex=ReactorGrid::getElement<size_t>(Control,cntlKey+"CEndIndex",
					    XIndex,YIndex);
 
  absMat=ReactorGrid::getMatElement(Control,cntlKey+"AbsMat",
				    XIndex,YIndex);
  cladMat=ReactorGrid::getMatElement(Control,cntlKey+"CladMat",
				     XIndex,YIndex);


  lift=ReactorGrid::getElement<double>
    (Control,cntlKey+"Lift",XIndex,YIndex);

  voidRadius=ReactorGrid::getElement<double>
    (Control,cntlKey+"VoidRadius",XIndex,YIndex);
  voidOffset=ReactorGrid::getElement<double>
    (Control,cntlKey+"VoidOffset",XIndex,YIndex);
  absThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"AbsThick",XIndex,YIndex);
  caseThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"CaseThick",XIndex,YIndex);
  waterThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"WaterThick",XIndex,YIndex);
  plateThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"PlateThick",XIndex,YIndex);
  innerLength=ReactorGrid::getElement<double>
    (Control,cntlKey+"InnerLength",XIndex,YIndex);
  outerLength=ReactorGrid::getElement<double>
    (Control,cntlKey+"OuterLength",XIndex,YIndex);
  outerCapLen=ReactorGrid::getElement<double>
    (Control,cntlKey+"OuterCapLen",XIndex,YIndex);
  
  endCap=ReactorGrid::getElement<double>
    (Control,cntlKey+"EndCap",XIndex,YIndex);

  return;
}

void
ControlElement::createSurfaces(const attachSystem::FixedComp& RG)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid (surf 5)
  */
{  
  ELog::RegMethod RegA("ControlElement","createSurface");


  FuelElement::createSurfaces(RG,cStartIndex,cEndIndex);

  if (midCentre.empty())
    throw ColErr::EmptyValue<void>("midCentre");

  const Geometry::Vec3D midPt(midCentre.front());  
  // Void / Dividers
  ModelSupport::buildPlane(SMap,controlIndex+3,
			   midPt-X*voidOffset,X);

  ModelSupport::buildPlane(SMap,controlIndex+4,
			   midPt+X*voidOffset,X);

  ModelSupport::buildCylinder(SMap,controlIndex+7,
			      midPt-X*voidOffset,
			      Z,voidRadius);
  ModelSupport::buildCylinder(SMap,controlIndex+8,
			      midPt+X*voidOffset,
			      Z,voidRadius);

  // B4C
  ModelSupport::buildPlane(SMap,controlIndex+11,
			   midPt-Y*absThick/2.0,Y);
  ModelSupport::buildPlane(SMap,controlIndex+12,
			   midPt+Y*absThick/2.0,Y);
  ModelSupport::buildCylinder(SMap,controlIndex+17,
			      midPt-X*voidOffset,
			      Z,absThick/2.0);
  ModelSupport::buildCylinder(SMap,controlIndex+18,
			      midPt+X*voidOffset,
			      Z,absThick/2.0);

  // Al 
  ModelSupport::buildPlane(SMap,controlIndex+21,
			   midPt-Y*(absThick/2.0+caseThick),Y);
  ModelSupport::buildPlane(SMap,controlIndex+22,
			   midPt+Y*(absThick/2.0+caseThick),Y);

  ModelSupport::buildCylinder(SMap,controlIndex+27,
			      midPt-X*voidOffset,
			      Z,(absThick/2.0+caseThick));
  ModelSupport::buildCylinder(SMap,controlIndex+28,
			      midPt+X*voidOffset,
			      Z,(absThick/2.0+caseThick));

  // Outer Plates [Non-moving]
  ModelSupport::buildPlane(SMap,controlIndex+31,
	     midPt-Y*(absThick/2.0+caseThick+waterThick),Y);
  ModelSupport::buildPlane(SMap,controlIndex+32,
	     midPt+Y*(absThick/2.0+caseThick+waterThick),Y);

  ModelSupport::buildPlane(SMap,controlIndex+41,
	     midPt-Y*(absThick/2.0+caseThick+waterThick+plateThick),Y);
  ModelSupport::buildPlane(SMap,controlIndex+42,
	     midPt+Y*(absThick/2.0+caseThick+waterThick+plateThick),Y);
  // Cap 
  ModelSupport::buildPlane(SMap,controlIndex+46,
	   midPt+Z*(outerCapLen+outerLength-
			(fuelHeight/2.0+cladHeight)),Z);

  // Top / bottom : Outer Plates:
  ModelSupport::buildPlane(SMap,controlIndex+16,
			   midPt+Z*(outerLength-(fuelHeight/2.0+cladHeight)),Z);
  // Lift Part
  ModelSupport::buildPlane(SMap,controlIndex+25,
			   midPt+Z*(lift-innerLength/2.0),Z);
  ModelSupport::buildPlane(SMap,controlIndex+26,
			   midPt+Z*(lift+innerLength/2.0),Z);
  // End Cap (on lift)
  ModelSupport::buildPlane(SMap,controlIndex+35,
			   midPt+Z*(lift-innerLength/2.0-endCap),Z);

  return;
}

void
ControlElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("ControlElement","createObjects");

  FuelElement::createObjects(System);

  std::string Out;

  // First create the outer Al layers:
  Out=ModelSupport::getComposite(SMap,controlIndex,surfIndex,
				 " 23M -24M 25M 41 -31 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  ContainedGroup::addOuterUnionSurf("Track",Out);      
  Out=ModelSupport::getComposite(SMap,controlIndex,surfIndex,
				 " 23M -24M 25M 32 -42 -16");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  ContainedGroup::addOuterUnionSurf("Track",Out);      
  // Top Cap
  Out=ModelSupport::getComposite(SMap,controlIndex,surfIndex,
				 " 23M -24M  32 -2M 16 -46");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  ContainedGroup::addOuterUnionSurf("Cap",Out);      
  Out=ModelSupport::getComposite(SMap,controlIndex,surfIndex,
				 " 23M -24M 1M -31 16 -46");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  ContainedGroup::addOuterUnionSurf("Cap",Out);      


  // Build Inner Core:
  // voids
  Out=ModelSupport::getComposite(SMap,controlIndex,"-7 25 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,controlIndex,"-8 25 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // B4C [3 parts to avoid null point error]
  Out=ModelSupport::getComposite(SMap,controlIndex,"-17 -3 25 -26 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,controlIndex,"-18 4 25 -26 8");
  System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,controlIndex,"7 8 11 -12 3 -4 25 -26 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));

  // Out=ModelSupport::getComposite(SMap,controlIndex,
  // 				 "11 -12 (-17:3) (-18:-4) 7 8 25 -26");
  // System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));
  
  // Cladding [3 parts]
  Out=ModelSupport::getComposite(SMap,controlIndex,"-27 -3 25 -26 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,controlIndex,"-28 4 25 -26 18");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,controlIndex,
				 "21 -22 (-11:12) 3 -4 25 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladMat,0.0,Out));
  
  // End Cap [3 block to avoid error]
  Out=ModelSupport::getComposite(SMap,controlIndex,"-27 -3 35 -25");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,controlIndex,"-28 4 35 -25");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,controlIndex,"21 -22 3 -4 35 -25");
  System.addCell(MonteCarlo::Qhull(cellIndex++,cladMat,0.0,Out));
  // Exclude
  Out=ModelSupport::getComposite(SMap,controlIndex," -27 -3 35 -26 "); 
  ContainedGroup::addOuterUnionSurf("Rod",Out);      
  Out=ModelSupport::getComposite(SMap,controlIndex," -28 4 35 -26 "); 
  ContainedGroup::addOuterUnionSurf("Rod",Out);      
  Out=ModelSupport::getComposite(SMap,controlIndex," 21 -22 35 -26 3 -4 "); 
  ContainedGroup::addOuterUnionSurf("Rod",Out);      
  // Out=ModelSupport::getComposite(SMap,controlIndex,
  // 				 "(-27:3) (-28:-4) 21 -22 35 -26 "); 
  ContainedGroup::addOuterUnionSurf("Rod",Out);      

  return;
}


void
ControlElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{
  return;
}

void
ControlElement::createAll(Simulation& System,
                          const attachSystem::FixedComp& FC,
			  const Geometry::Vec3D& OG,
			  const FuelLoad& FuelSystem)
  /*!
    Global creation of the control item
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Unit
    \param OG :: Origin
    \param FuelSystem :: Fuel load for plates
  */
{
  ELog::RegMethod RegA("ControlElement","createAll(ControlElement)");

  populate(System.getDataBase());

  createUnitVector(FC,OG);
  createSurfaces(FC);
  createObjects(System);
  createLinks();

  const std::vector<int>& IC=getInsertCells();
  if (!IC.empty())
    addAllInsertCell(IC.front());

  ContainedGroup::addInsertCell("Rod",midCell.front());
  ContainedGroup::addInsertCell("Rod",topCell);
  ContainedGroup::addInsertCell("Track",midCell.front());
  ContainedGroup::addInsertCell("Track",topCell);
  ContainedGroup::addInsertCell("Cap",topCell);

  FuelElement::layerProcess(System,FuelSystem);
  FuelElement::insertObjects(System);       
  ContainedGroup::insertObjects(System);

  return;
}


} // NAMESPACE delftSystem
