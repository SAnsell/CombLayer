/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/HfElement.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "KGroup.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

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
  */
{}


void
HfElement::populate(const Simulation& System)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("HfElement","populate");
  const FuncDataBase& Control=System.getDataBase();

  FuelElement::populate(System);
  // Create the exclude items
  cutSize=ReactorGrid::getElement<size_t>
    (Control,cntlKey+"CutSize",XIndex,YIndex);

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
  bladeWidth=ReactorGrid::getElement<double>
    (Control,cntlKey+"BladeWidth",XIndex,YIndex);


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

  FuelElement::createSurfaces(RG,Exclude);

  int cIndex(controlIndex);
  for(int i=0;i<static_cast<int>(midCentre.size());i++)
    {
        Geometry::Vec3D midPt=midCentre[static_cast<size_t>(i)];
  
      // Front 
      ModelSupport::buildPlane(SMap,cIndex+1,
			       midPt-Y*absThick/2.0,Y);
      ModelSupport::buildPlane(SMap,cIndex+2,
			       midPt+Y*absThick/2.0,Y);
      ModelSupport::buildPlane(SMap,cIndex+3,
			       midPt-X*absWidth/2.0,X);
      ModelSupport::buildPlane(SMap,cIndex+4,
			       midPt+X*absWidth/2.0,X);

      ModelSupport::buildPlane(SMap,cIndex+5,
			       midPt+Z*(-absHeight/2.0+lift),Z); 
      ModelSupport::buildPlane(SMap,cIndex+6,
			       midPt+Z*(absHeight/2.0+lift),Z);
     
      ModelSupport::buildPlane(SMap,cIndex+11,
			       midPt-Y*(absThick/2.0+bladeThick),Y);
      ModelSupport::buildPlane(SMap,cIndex+12,
			       midPt+Y*(absThick/2.0+bladeThick),Y);
      ModelSupport::buildPlane(SMap,cIndex+13,
			       midPt-X*(absWidth/2.0+bladeWidth),X);
      ModelSupport::buildPlane(SMap,cIndex+14,
			       midPt+X*(absWidth/2.0+bladeWidth),X);
      cIndex+=50;
    }
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

  FuelElement::createObjects(System,Exclude);

  std::string Out;
  int cIndex(controlIndex);
  for(size_t i=0;i<midCentre.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,cIndex," 1 -2 3 -4 5 -6 ");
      ContainedGroup::addOuterUnionSurf("Rod",Out);      
      System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));
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
HfElement::createAll(Simulation& System,const FixedComp& FC,
		       const Geometry::Vec3D& OG)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Unit
    \param OG :: Origin
  */
{
  ELog::RegMethod RegA("HfElement","createAll(HfElement)");

  populate(System);
  createUnitVector(FC,OG);
  createSurfaces(FC);
  createObjects(System);
  createLinks();

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
