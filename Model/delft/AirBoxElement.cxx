/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/AirBoxElement.cxx
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
#include "BaseMap.h"
#include "CellMap.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "RElement.h"
#include "AirBoxElement.h"

namespace delftSystem
{


AirBoxElement::AirBoxElement(const size_t XI,const size_t YI,
			 const std::string& Key) :
  RElement(XI,YI,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: X coordinate
    \param YI :: Y coordinate
    \param Key :: BaseName 
  */
{}

AirBoxElement::AirBoxElement(const AirBoxElement& A) : 
  RElement(A),
  Width(A.Width),Depth(A.Depth),Height(A.Height),
  wallThick(A.wallThick),edgeGap(A.edgeGap),innerMat(A.innerMat),
  wallMat(A.wallMat),waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: AirBoxElement to copy
  */
{}

AirBoxElement&
AirBoxElement::operator=(const AirBoxElement& A)
  /*!
    Assignment operator
    \param A :: AirBoxElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RElement::operator=(A);
      Width=A.Width;
      Depth=A.Depth;
      Height=A.Height;
      wallThick=A.wallThick;
      edgeGap=A.edgeGap;
      innerMat=A.innerMat;
      wallMat=A.wallMat;
      waterMat=A.waterMat;
    }
  return *this;
}

void
AirBoxElement::populateWaterMat(const attachSystem::FixedComp& RG)
  /*!
    Populate the water material from a reactor grid default
    \param RG :: FixedComp to cast to a ReactorGrid
  */
{
  ELog::RegMethod RegA("AirBoxElement","populate");

  const ReactorGrid* RGPtr=dynamic_cast<const ReactorGrid*>(&RG);
  waterMat=(RGPtr) ? RGPtr->getWaterMat() : 0;
  return;
}
  
  
void
AirBoxElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: Database for variables
  */
{
  ELog::RegMethod RegA("AirBoxElement","populate");


  Width=ReactorGrid::getElement<double>
    (Control,keyName+"Width",XIndex,YIndex);
  Depth=ReactorGrid::getElement<double>
    (Control,keyName+"Depth",XIndex,YIndex);
  Height=ReactorGrid::getElement<double>
    (Control,keyName+"Height",XIndex,YIndex);

  edgeGap=ReactorGrid::getElement<double>
    (Control,keyName+"EdgeGap",XIndex,YIndex);
  wallThick=ReactorGrid::getElement<double>
    (Control,keyName+"WallThick",XIndex,YIndex);

  innerMat=ReactorGrid::getMatElement
    (Control,keyName+"InnerMat",XIndex,YIndex);
  wallMat=ReactorGrid::getMatElement
    (Control,keyName+"WallMat",XIndex,YIndex);
  
  return;
}

void
AirBoxElement::createUnitVector(const attachSystem::FixedComp& FC,
				const Geometry::Vec3D& OG)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Reactor Grid Unit
    \param OG :: Orgin
  */
{
  ELog::RegMethod RegA("AirBoxElement","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  Origin=OG;
  return;
}

void
AirBoxElement::createSurfaces(const attachSystem::FixedComp& RG)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
  */
{  
  ELog::RegMethod RegA("AirBoxElement","createSurfaces");


  // Planes [OUTER]:
  
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*Depth/2.0,Y); 
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*Width/2.0,X);
  SMap.addMatch(surfIndex+5,RG.getLinkSurf(5));
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*Height,Z);


  ModelSupport::buildPlane(SMap,surfIndex+11,
			   Origin-Y*(Depth/2.0-edgeGap),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,
			   Origin+Y*(Depth/2.0-edgeGap),Y);
  ModelSupport::buildPlane(SMap,surfIndex+13,
			   Origin-X*(Width/2.0-edgeGap),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,
			   Origin+X*(Width/2.0-edgeGap),X);

  ModelSupport::buildPlane(SMap,surfIndex+21,
			   Origin-Y*(Depth/2.0-edgeGap-wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+22,
			   Origin+Y*(Depth/2.0-edgeGap-wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+23,
			   Origin-X*(Width/2.0-edgeGap-wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+24,
			   Origin+X*(Width/2.0-edgeGap-wallThick),X);

  ModelSupport::buildPlane(SMap,surfIndex+25,
			   RG.getLinkPt(5)+Z*wallThick,Z);

  ModelSupport::buildPlane(SMap,surfIndex+26,
			   Origin+Z*(Height-wallThick),Z);


  ModelSupport::buildPlane(SMap,surfIndex+6,Z*Height,Z);


  return;
}

void
AirBoxElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("AirBoxElement","createObjects");

  std::string Out;
  // Outer Layers
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);      
  
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 1 -2 3 -4 5 -6 (-11:12:-13:14)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  // walls
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 11 -12 13 -14 5 -6 (-21:22:-23:24:-25:26)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 21 -22 23 -24 25 -26 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));


  return;
}


void
AirBoxElement::createLinks()
  /*!
    Creates lines [currently does nothing]
  */
{

  return;
}

void
AirBoxElement::createAll(Simulation& System,
			 const attachSystem::FixedComp& RG,
			 const Geometry::Vec3D& OG,
			 const FuelLoad&)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param RG :: Fixed Unit
    \param OG :: Origin
    \param :: FuelLoad not used
  */
{
  ELog::RegMethod RegA("AirBoxElement","createAll");
  populate(System.getDataBase());
  populateWaterMat(RG);
  createUnitVector(RG,OG);
  createSurfaces(RG);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
