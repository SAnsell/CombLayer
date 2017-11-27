/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/FuelElement.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDivide.h"
#include "surfRegister.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "FuelElement.h"


namespace delftSystem
{


FuelElement::FuelElement(const size_t XI,const size_t YI,
			 const std::string& Key) :
  RElement(XI,YI,Key),midCell(0),topCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: X coordinate
    \param YI :: Y coordinate
    \param Key :: Name of unit
  */
{}

FuelElement::FuelElement(const FuelElement& A) : 
  RElement(A),
  nElement(A.nElement),Exclude(A.Exclude),
  depth(A.depth),width(A.width),
  topHeight(A.topHeight),baseHeight(A.baseHeight),
  baseRadius(A.baseRadius),fuelHeight(A.fuelHeight),
  fuelWidth(A.fuelWidth),fuelDepth(A.fuelDepth),
  cladHeight(A.cladHeight),cladDepth(A.cladDepth),
  cladWidth(A.cladWidth),waterDepth(A.waterDepth),
  barRadius(A.barRadius),barOffset(A.barOffset),
  alMat(A.alMat),watMat(A.watMat),fuelMat(A.fuelMat),
  midCell(A.midCell),midCentre(A.midCentre),
  topCell(A.topCell)
  /*!
    Copy constructor
    \param A :: FuelElement to copy
  */
{}

FuelElement&
FuelElement::operator=(const FuelElement& A)
  /*!
    Assignment operator
    \param A :: FuelElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RElement::operator=(A);
      nElement=A.nElement;
      Exclude=A.Exclude;
      depth=A.depth;
      width=A.width;
      topHeight=A.topHeight;
      baseHeight=A.baseHeight;
      baseRadius=A.baseRadius;
      fuelHeight=A.fuelHeight;
      fuelWidth=A.fuelWidth;
      fuelDepth=A.fuelDepth;
      cladHeight=A.cladHeight;
      cladDepth=A.cladDepth;
      cladWidth=A.cladWidth;
      waterDepth=A.waterDepth;
      barRadius=A.barRadius;
      barOffset=A.barOffset;
      alMat=A.alMat;
      watMat=A.watMat;
      fuelMat=A.fuelMat;
      midCell=A.midCell;
      midCentre=A.midCentre;
      topCell=A.topCell;
    }
  return *this;
}

Geometry::Vec3D
FuelElement::plateCentre(const size_t Index) const
  /*!
    Return the plate centre based on the orign in the centre
    \param Index :: plateCentre value
    \return plateCentre
   */
{
  const double plateDepth(fuelDepth+cladDepth*2.0+waterDepth);
  return Origin+Y*plateDepth*
    (static_cast<double>(Index)-(static_cast<double>(nElement)-1.0)/2.0);
}

void
FuelElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("FuelElement","populate");

  nElement=ReactorGrid::getElement<size_t>(Control,keyName+"NFuel",
					   XIndex,YIndex);
 
  width=ReactorGrid::getElement<double>(Control,keyName+"Width",
					XIndex,YIndex);
  depth=ReactorGrid::getElement<double>(Control,keyName+"Depth",
					XIndex,YIndex);
 
  topHeight=ReactorGrid::getElement<double>(Control,keyName+"TopHeight",
					    XIndex,YIndex);
  baseHeight=ReactorGrid::getElement<double>(Control,keyName+"BaseHeight",
					     XIndex,YIndex);
  baseRadius=ReactorGrid::getElement<double>(Control,keyName+"BaseRadius",
					     XIndex,YIndex);
  fuelHeight=ReactorGrid::getElement<double>(Control,keyName+"FuelHeight",
					     XIndex,YIndex);
  fuelDepth=ReactorGrid::getElement<double>(Control,keyName+"FuelDepth",
					    XIndex,YIndex);
  fuelWidth=ReactorGrid::getElement<double>(Control,keyName+"FuelWidth",
					    XIndex,YIndex);
  cladHeight=ReactorGrid::getElement<double>(Control,keyName+"CladHeight",
					     XIndex,YIndex);
  cladDepth=ReactorGrid::getElement<double>(Control,keyName+"CladDepth",
					    XIndex,YIndex);
  cladWidth=ReactorGrid::getElement<double>(Control,keyName+"CladWidth",
					    XIndex,YIndex);

  waterDepth=ReactorGrid::getElement<double>(Control,keyName+"WaterDepth",
					     XIndex,YIndex);

  barRadius=ReactorGrid::getElement<double>(Control,keyName+"BarRadius",
					    XIndex,YIndex);
  barOffset=ReactorGrid::getElement<double>(Control,keyName+"BarOffset",
					    XIndex,YIndex);

  alMat=ReactorGrid::getMatElement(Control,keyName+"AlMat",
				   XIndex,YIndex);
  fuelMat=ReactorGrid::getMatElement(Control,keyName+"FuelMat",
				     XIndex,YIndex);
  watMat=ReactorGrid::getMatElement(Control,keyName+"WaterMat",
				    XIndex,YIndex);
  
  nFuel=ReactorGrid::getElement<size_t>(Control,keyName+"NFuelDivide",
					XIndex,YIndex);
  makeFuelDivider();
  return;
}

void
FuelElement::makeFuelDivider()
  /*!
    Populate the fuel divider fraction 
  */
{
  ELog::RegMethod RegA("FuelElement","makeFuelDivider");
  
  fuelFrac.clear();
  
  for(size_t i=1;i<nFuel;i++)
    {
      fuelFrac.push_back(static_cast<double>(i)/
			 static_cast<double>(nFuel));
    }

  return;
}


bool
FuelElement::isFuel(const size_t nE) const
  /*!
    Test ot see if nE element is a fuel cell
    \param nE :: Fuel elment number
    \return 1 on success
   */
{
  
  if (nE>=nElement) return 0;
  return (Exclude.find(nE)==Exclude.end()) 
    ? 1 : 0;
}

void
FuelElement::createUnitVector(const attachSystem::FixedComp& FC,
			      const Geometry::Vec3D& OG)
  /*!
    Create the unit vectors
    \param FC :: Reactor Grid Unit
    \param OG :: Origin
  */
{
  ELog::RegMethod RegA("FuelElement","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  Origin=OG;
  if (Origin.abs()>1e4)
    {
      ELog::EM<<"Fuel: "<<keyName<<" "<<Origin<<ELog::endDiag;
      ELog::EM<<"Fuel: "<<XIndex<<" "<<YIndex<<ELog::endDiag;
      ELog::EM<<"Y "<<keyName<<" "<<Z<<ELog::endErr;
    }
  return;
}

void
FuelElement::createSurfaces(const attachSystem::FixedComp& RG)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
  */
{  
  ELog::RegMethod RegA("FuelElement","createSurface(exclude)");

  ModelSupport::buildPlane(SMap,surfIndex+1,
			   Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,
			   Origin+Y*depth/2.0,Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,
			   Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,surfIndex+4,
			   Origin+X*width/2.0,X);
  const double tHeight(fuelHeight/2.0+cladHeight+topHeight);

  SMap.addMatch(surfIndex+5,RG.getLinkSurf(5));
  ModelSupport::buildPlane(SMap,surfIndex+6,
   			   Origin+Z*tHeight,Z);
  // Width numbers:
  ModelSupport::buildPlane(SMap,surfIndex+13,
			   Origin-X*fuelWidth/2.0,X);
  ModelSupport::buildPlane(SMap,surfIndex+14,
			   Origin+X*fuelWidth/2.0,X);
  ModelSupport::buildPlane(SMap,surfIndex+15,
			   Origin-Z*fuelHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,
			   Origin+Z*fuelHeight/2.0,Z);
  // Cladding
  ModelSupport::buildPlane(SMap,surfIndex+23,
			   Origin-X*(fuelWidth/2.0+cladWidth),X);
  ModelSupport::buildPlane(SMap,surfIndex+24,
			   Origin+X*(fuelWidth/2.0+cladWidth),X);
  ModelSupport::buildPlane(SMap,surfIndex+25,
			   Origin-Z*(fuelHeight/2.0+cladHeight),Z);
  ModelSupport::buildPlane(SMap,surfIndex+26,
			   Origin+Z*(fuelHeight/2.0+cladHeight),Z);

  int surfOffset(surfIndex+100);
  size_t excFlag(0);
  for(size_t i=0;i<nElement;i++)
    {
      const Geometry::Vec3D POrg=plateCentre(i);
      if (Exclude.find(i)==Exclude.end())
	{
	  ModelSupport::buildPlane(SMap,surfOffset+11,
				   POrg-Y*fuelDepth/2.0,Y);
	  ModelSupport::buildPlane(SMap,surfOffset+12,
				   POrg+Y*fuelDepth/2.0,Y);
	  ModelSupport::buildPlane(SMap,surfOffset+21,
				   POrg-Y*(fuelDepth/2.0+cladDepth),Y);
	  ModelSupport::buildPlane(SMap,surfOffset+22,
				   POrg+Y*(fuelDepth/2.0+cladDepth),Y);
	  surfOffset+=100;
	  if (excFlag)
	    {
	      midCentre.push_back((plateCentre(excFlag-1)+
				   plateCentre(i-1))/2.0);
	      excFlag=0;
	    }
	}
      else if (!excFlag)
	excFlag=i+1;
    }

  // Special for the end point
  if (excFlag)
    {
      midCentre.push_back((plateCentre(excFlag-1)+
			   plateCentre(nElement-1))/2.0);      
    }

  // TOP HOLDER:
  ModelSupport::buildCylinder(SMap,surfIndex+37,
			      Origin+Z*(tHeight-barOffset),
			      X,barRadius);

  // Base Holder
  ModelSupport::buildCylinder(SMap,surfIndex+47,Origin,Z,baseRadius);
  
  return;
}

void
FuelElement::createSurfaces(const attachSystem::FixedComp& RG,
			    const size_t cStart,const size_t cEnd)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
    \param cStart :: Exclude start for control object [inclusive]
    \param cEnd :: Exclude end for control object [inclusive]
  */
{  
  ELog::RegMethod RegA("FuelElement","createSurface");

  Exclude.clear();
  for(size_t i=cStart;i<=cEnd && i<nElement;i++)
    Exclude.insert(i);

  createSurfaces(RG);
  return;
}

void
FuelElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("FuelElement","createObjects");

  std::string Out;
  // Outer Layers
  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);      

  // Outer plates:
  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 3 -23 25 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 24 -4 25 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  int surfOffset(surfIndex+100);
  // Front water plate
  Out=ModelSupport::getComposite(SMap,surfIndex,surfOffset,
				 " 1 -21M 23 -24 25 -26 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,watMat,0.0,Out));
  waterCells.push_back(cellIndex-1);
  //  const double plateDepth(fuelDepth+cladDepth*2.0+waterDepth);
  // First Point

  // Mid point of surfaces 13,14,15,16:

  // FIND MAX Item
  size_t nElmMax(0);
  for(size_t i=nElement-1;!nElmMax && i>0;i--)
    if (Exclude.find(i)==Exclude.end())
      nElmMax=i;

  
  for(size_t i=0;i<nElement;i++)
    {
      if (Exclude.find(i)==Exclude.end())
	{
	  // Fuel
	  Out=ModelSupport::getComposite(SMap,surfIndex,surfOffset,
					 " 11M -12M 13 -14 15 -16 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,fuelMat,0.0,Out));
	  fuelCells.push_back(cellIndex-1);
	  fuelCentre.push_back(plateCentre(i));
	  // Cladding
	  Out=ModelSupport::getComposite(SMap,surfIndex,surfOffset,
					 " 21M -22M 23 -24 25 -26 "
					 " (-11M:12M:-13:14:-15:16) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
	  
	  // Water (def material) [ one + 

	  if (i!=nElement-1 && Exclude.find(i+1)==Exclude.end())
	    Out=ModelSupport::getComposite(SMap,surfIndex,surfOffset,
					   " 22M -121M 23 -24 25 -26 ");
	  else if (i==nElmMax)
	    Out=ModelSupport::getComposite(SMap,surfIndex,surfOffset,
					     " 22M -2 23 -24 25 -26 ");
	  else 
	    {
	      Out=ModelSupport::getComposite(SMap,surfIndex,surfOffset,
					     " 22M -121M 23 -24 25 -26 ");
	    }
	  System.addCell(MonteCarlo::Qhull(cellIndex++,watMat,0.0,Out));
	  waterCells.push_back(cellIndex-1);

	  surfOffset+=100;      
	}	  
      else if (midCell.empty() || midCell.back()!=cellIndex-1)
	midCell.push_back(cellIndex-1);
    }

  // -----  TOP  ----
  if (midCell.empty())
    {
      // Holder:
      Out=ModelSupport::getComposite(SMap,surfIndex,"23 -24 -37 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
      // Water in space
      Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 23 -24 26 -6 37 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,watMat,0.0,Out));
    }
  else
    {
      // ONLY Water in space
      Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 23 -24 26 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,watMat,0.0,Out));
    }
  topCell=cellIndex-1;

  // -------- BASE -----------------
  Out=ModelSupport::getComposite(SMap,surfIndex,"5 -25 -47 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,watMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,"5 -25 47 3 -4 1 -2");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  return;
}

void
FuelElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{

  return;
}

void
FuelElement::addWaterExclude(Simulation& System,
			     const Geometry::Vec3D& Pt,
			     const std::string& ExcStr)
  /*!
    Find water cells that contain the given point
    \param System :: Simulation to get cells from
    \param Pt :: Point to search for
    \param ExcStr :: Exclude String
   */
{
  ELog::RegMethod RegA("FuelElement","addWaterExclude");

  std::vector<int>::const_iterator vc;
  for(vc=waterCells.begin();vc!=waterCells.end();vc++)
    {
      MonteCarlo::Qhull* OPtr=System.findQhull(*vc);
      if (OPtr && OPtr->isValid(Pt))
	{
	  OPtr->addSurfString(ExcStr);
	  return;
	}
    }
  return;

}
     
void
FuelElement::layerProcess(Simulation& System,const FuelLoad& FuelSystem)
  /*!
    Layer all the fuel elements.
    Note all calls go to FuelSystem.getMaterial

    \param System :: Simulation
    \param FuelSystem :: Fuel Load system for materials
  */
{
  ELog::RegMethod RegA("FuelElement","layerProcess");

  if (nFuel<2) return;
  // All fuel cells
  int SI(surfIndex+4001);
  for(size_t i=0;i<fuelCells.size();i++)
    {
      ModelSupport::surfDivide DA;
      for(size_t j=1;j<nFuel;j++)
	{
	  DA.addFrac(fuelFrac[j-1]);
	  
	  DA.addMaterial
	    (FuelSystem.getMaterial(XIndex+1,YIndex+1,i+1,j,fuelMat));
	}
      DA.addMaterial(FuelSystem.getMaterial(XIndex+1,YIndex+1,i+1,
					    nFuel,fuelMat));
      DA.init();
      DA.setCellN(fuelCells[i]);
      DA.setOutNum(cellIndex,SI);
      DA.makePair<Geometry::Plane>(SMap.realSurf(surfIndex+15),
				   SMap.realSurf(surfIndex+16));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
      SI+=100;
    }
  return;
}

void
FuelElement::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const Geometry::Vec3D& OG,
		       const FuelLoad& FuelSystem)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Unit
    \param OG :: Origin
    \param FuelSystem :: Default fuel load
  */
{
  ELog::RegMethod RegA("FuelElement","createAll(FuelElement)");

  populate(System.getDataBase());

  createUnitVector(FC,OG);
  createSurfaces(FC,ULONG_MAX,ULONG_MAX);
  createObjects(System); //,ULONG_MAX,ULONG_MAX);
  createLinks();
  layerProcess(System,FuelSystem);
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
