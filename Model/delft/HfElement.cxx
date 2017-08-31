/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/HfElement.cxx
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
#include "HfElement.h"

namespace delftSystem
{


HfElement::HfElement(const size_t XI,const size_t YI,
			       const std::string& Key,
			       const std::string& CKey) :
  FuelElement(XI,YI,Key),
  attachSystem::ContainedGroup("Track","Rod"),cntlKey(CKey),
  controlIndex(ModelSupport::objectRegister::Instance().
	       cell(ReactorGrid::getElementName(CKey,XI,YI)))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: Grid position
    \param YI :: Grid position 
    \param Key :: Keyname for basic cell
    \param CKey :: Keyname for control
  */
{}

HfElement::HfElement(const HfElement& A) : 
  FuelElement(A),attachSystem::ContainedGroup(A),
  cntlKey(A.cntlKey),controlIndex(A.controlIndex),
  cutSize(A.cutSize),absMat(A.absMat),
  bladeMat(A.bladeMat),absThick(A.absThick),absWidth(A.absWidth),
  bladeThick(A.bladeThick),
  absHeight(A.absHeight),lift(A.lift)
  /*!
    Copy constructor
    \param A :: HfElement to copy
  */
{}

HfElement&
HfElement::operator=(const HfElement& A)
  /*!
    Assignment operator
    \param A :: HfElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FuelElement::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      cutSize=A.cutSize;
      absMat=A.absMat;
      bladeMat=A.bladeMat;
      absThick=A.absThick;
      absWidth=A.absWidth;
      bladeThick=A.bladeThick;
      absHeight=A.absHeight;
      lift=A.lift;
    }
  return *this;
}


void
HfElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: Database system
  */
{
  ELog::RegMethod RegA("HfElement","populate");

  FuelElement::populate(Control);
  
  cladDepth=ReactorGrid::getDefElement<double>
    (Control,keyName+"CladDepth",XIndex,YIndex,cntlKey+"CladDepth");
  fuelDepth=ReactorGrid::getDefElement<double>
    (Control,keyName+"FuelDepth",XIndex,YIndex,cntlKey+"FuelDepth");
  waterDepth=ReactorGrid::getDefElement<double>
    (Control,keyName+"WaterDepth",XIndex,YIndex,cntlKey+"WaterDepth");

  // Create the exclude items
  cutSize=ReactorGrid::getElement<size_t>
    (Control,cntlKey+"CutSize",XIndex,YIndex);

  Exclude.clear();
  for(size_t i=0;i<cutSize;i++)
    {
      Exclude.insert(i);
      Exclude.insert(nElement-(i+1));
    }

  absMat=ReactorGrid::getMatElement(Control,cntlKey+"AbsMat",
				    XIndex,YIndex);
  bladeMat=ReactorGrid::getMatElement(Control,cntlKey+"BladeMat",
				       XIndex,YIndex);

  lift=ReactorGrid::getElement<double>
    (Control,cntlKey+"Lift",XIndex,YIndex);

  absThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"AbsThick",XIndex,YIndex);
  absWidth=ReactorGrid::getElement<double>
    (Control,cntlKey+"AbsWidth",XIndex,YIndex);
  absHeight=ReactorGrid::getElement<double>
    (Control,cntlKey+"AbsHeight",XIndex,YIndex);
  bladeThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"BladeThick",XIndex,YIndex);

  coolThick=ReactorGrid::getElement<double>
    (Control,cntlKey+"CoolThick",XIndex,YIndex);

  return;
}

void
HfElement::createSurfaces(const attachSystem::FixedComp& RG)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid (surf 5)
  */
{  
  ELog::RegMethod RegA("HfElement","createSurface");

  FuelElement::createSurfaces(RG);
  if (!cutSize) return;
  
  int cIndex(controlIndex);

  //
  // Work away from cut element:
  //
  Geometry::Vec3D OP=plateCentre(cutSize-1);
  // Blade
  ModelSupport::buildPlane(SMap,cIndex+1,OP-Y*bladeThick/2.0,Y);
  ModelSupport::buildPlane(SMap,cIndex+2,OP+Y*bladeThick/2.0,Y);
  OP-=Y*(coolThick+(bladeThick+absThick)/2.0);
  
  ModelSupport::buildPlane(SMap,cIndex+11,OP-Y*(absThick/2.0),Y);
  ModelSupport::buildPlane(SMap,cIndex+12,OP+Y*(absThick/2.0),Y);
  OP-=Y*(coolThick+(bladeThick+absThick)/2.0);
  
  ModelSupport::buildPlane(SMap,cIndex+21,OP-Y*bladeThick/2.0,Y);
  ModelSupport::buildPlane(SMap,cIndex+22,OP+Y*bladeThick/2.0,Y);
  cIndex+=50;

  OP=plateCentre(nElement-cutSize);
  // Blade
  ModelSupport::buildPlane(SMap,cIndex+1,OP-Y*bladeThick/2.0,Y);
  ModelSupport::buildPlane(SMap,cIndex+2,OP+Y*bladeThick/2.0,Y);
  OP+=Y*(coolThick+(bladeThick+absThick)/2.0);
  
  ModelSupport::buildPlane(SMap,cIndex+11,OP-Y*(absThick/2.0),Y);
  ModelSupport::buildPlane(SMap,cIndex+12,OP+Y*(absThick/2.0),Y);
  OP+=Y*(coolThick+(bladeThick+absThick)/2.0);
  
  ModelSupport::buildPlane(SMap,cIndex+21,OP-Y*bladeThick/2.0,Y);
  ModelSupport::buildPlane(SMap,cIndex+22,OP+Y*bladeThick/2.0,Y);

  ModelSupport::buildPlane(SMap,controlIndex+3,OP-X*absWidth/2.0,X);
  ModelSupport::buildPlane(SMap,controlIndex+4,OP+X*absWidth/2.0,X);
  ModelSupport::buildPlane(SMap,controlIndex+5,
			   OP+Z*(-fuelHeight/2.0+lift),Z); 
  ModelSupport::buildPlane(SMap,controlIndex+6,
			   OP+Z*(absHeight-fuelHeight/2.0+lift),Z); 

  return;
}

void
HfElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("HfElement","createObjects");

  FuelElement::createObjects(System);

  if (!cutSize) return;

  std::string Out;
  int cIndex(controlIndex);
  const Geometry::Vec3D cutPos[]=
    { plateCentre((cutSize+1)/2),
      plateCentre(nElement-(cutSize+1)/2) };

  for(size_t i=0;i<2;i++)  // front / back
    {
      Out=ModelSupport::getComposite(SMap,cIndex,surfIndex,
				     " 1 -2 23M -24M 25M -26M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,bladeMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,cIndex,controlIndex,
				     " 11 -12 3M -4M 5M -6M ");
      ContainedGroup::addOuterUnionSurf("Rod",Out);      
      System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,cIndex,surfIndex,
				     " 21 -22 23M -24M 25M -26M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,bladeMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,cIndex,"(-1:2) (-21:22)");
      addWaterExclude(System,cutPos[i],Out);

      cIndex+=50;
    }	  


  return;
}


void
HfElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{

  return;
}

void
HfElement::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const Geometry::Vec3D& OG,
		     const FuelLoad& FuelSystem)
  /*!
    Global creation of element
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Unit
    \param OG :: Origin
    \param FuelSystem :: Fuelfor main elements
  */
{
  ELog::RegMethod RegA("HfElement","createAll(HfElement)");

  populate(System.getDataBase());
  createUnitVector(FC,OG);
  createSurfaces(FC);
  createObjects(System);
  createLinks();

  FuelElement::layerProcess(System,FuelSystem);

  const std::vector<int>& IC=getInsertCells();
  if (!IC.empty())
    addAllInsertCell(IC.front());
  std::vector<int>::const_iterator vc;
  for(vc=midCell.begin();vc!=midCell.end();vc++)
    {
      ContainedGroup::addInsertCell("Rod",*vc);
      ContainedGroup::addInsertCell("Track",*vc);
    }
  
  if (topCell)
    {
      ContainedGroup::addInsertCell("Rod",topCell);
      ContainedGroup::addInsertCell("Track",topCell);
    }
  FuelElement::insertObjects(System);       
  ContainedGroup::insertObjects(System);

  return;
}


} // NAMESPACE delftSystem
