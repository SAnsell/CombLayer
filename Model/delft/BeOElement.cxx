/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeOElement.cxx
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
#include "shutterBlock.h"
#include "groupRange.h"
#include "objectGroups.h"
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
#include "BeOElement.h"

namespace delftSystem
{

BeOElement::BeOElement(const size_t XI,const size_t YI,
		       const std::string& Key) :
  RElement(XI,YI,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: X grid coordinate 
    \param YI :: Y grid coordinate
    \param Key :: Blockname
  */
{}

void
BeOElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BeOElement","populate");

  Width=ReactorGrid::getElement<double>
    (Control,baseName+"Width",XIndex,YIndex);
  Depth=ReactorGrid::getElement<double>
    (Control,baseName+"Depth",XIndex,YIndex);
  TopHeight=ReactorGrid::getElement<double>
    (Control,baseName+"TopHeight",XIndex,YIndex);

  wallThick=ReactorGrid::getElement<double>
    (Control,baseName+"WallThick",XIndex,YIndex);
  coolThick=ReactorGrid::getElement<double>
    (Control,baseName+"CoolThick",XIndex,YIndex);

  beMat=ReactorGrid::getMatElement
    (Control,baseName+"Mat",XIndex,YIndex);
  coolMat=ReactorGrid::getMatElement
    (Control,baseName+"CoolMat",XIndex,YIndex);
  wallMat=ReactorGrid::getMatElement
    (Control,baseName+"WallMat",XIndex,YIndex);
  
  return;
}

void
BeOElement::createUnitVector(const attachSystem::FixedComp& FC,
			     const Geometry::Vec3D& OG)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Reactor Grid Unit
    \param OG :: Origin
  */
{
  ELog::RegMethod RegA("BeOElement","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  Origin=OG;
  return;
}

void
BeOElement::createSurfaces(const attachSystem::FixedComp& RG)
  /*!
    Creates/duplicates the surfaces for this block
    \param RG :: Reactor grid
  */
{  
  ELog::RegMethod RegA("BeOElement","createSurface");

  // Planes [OUTER]:
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*Depth/2.0,Y); 
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Z*TopHeight,Z);

  double T(wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(Depth/2.0-T),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(Depth/2.0-T),Y); 
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(Width/2.0-T),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(Width/2.0-T),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,RG.getLinkPt(5)+Z*T,Z);

  T+=coolThick;
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(Depth/2.0-T),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(Depth/2.0-T),Y); 
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(Width/2.0-T),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(Width/2.0-T),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,RG.getLinkPt(5)+Z*T,Z);

  SMap.addMatch(buildIndex+5,RG.getLinkSurf(5));

  return;
}

void
BeOElement::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("BeOElement","createObjects");

  std::string Out;
  // Outer Layers
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);      

  Out+=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14:-15)");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -6 "
				  " (-21 : 22 : -23 : 24 : -25)");
  System.addCell(MonteCarlo::Object(cellIndex++,coolMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 23 -24 25 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,beMat,0.0,Out));

  return;
}


void
BeOElement::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{

  return;
}

void
BeOElement::createAll(Simulation& System,
                      const attachSystem::FixedComp& RG,
                      const Geometry::Vec3D& OG,
                      const FuelLoad&)
/*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param RG :: Fixed Unit
    \param OG :: Orgin
  */
{
  ELog::RegMethod RegA("BeOElement","createAll");

  populate(System.getDataBase());
  createUnitVector(RG,OG);
  createSurfaces(RG);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}


} // NAMESPACE delftSystem
