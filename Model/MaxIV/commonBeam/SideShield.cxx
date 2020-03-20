/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/SideShield.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "SideShield.h"


namespace xraySystem
{

SideShield::SideShield(const std::string& mainKey) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(mainKey,6),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  baseName(mainKey)
  /*!
    Default constructor
    \param mainKey :: Key name for variables
  */
{}

SideShield::SideShield(const std::string& baseKey,
		       const std::string& mainKey) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(mainKey,6),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  baseName(baseKey)
  /*!
    Default constructor
    \param baseKey :: Base Key name for variables (fallback)
    \param mainKey :: Key name for variables
  */
{}
  
void
SideShield::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SideShield","populate");

  FixedOffset::populate(baseName,Control);
  
  depth=Control.EvalTail<double>(keyName,baseName,"Depth");
  height=Control.EvalTail<double>(keyName,baseName,"Height");
  length=Control.EvalTail<double>(keyName,baseName,"Length");
  outStep=Control.EvalDefTail<double>(keyName,baseName,"OutStep",0.0);

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  
  return;
}

void
SideShield::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    Note that Y points OUT of the ring
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("SideShield","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  const HeadRule& HR=ExternalCut::getRule("Wall");

  const int SN=HR.getPrimarySurface();
  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(HR.getSurface(SN));

  if (PPtr)
    {
      Geometry::Vec3D PAxis=PPtr->getNormal();
      if (Y.dotProd(PAxis)*SN < -Geometry::zeroTol)
	PAxis*=-1;
      FixedComp::reOrientate(1,PAxis);
      Origin=SurInter::getLinePoint(Origin,Y,PPtr);
    }
  applyOffset();
  return;
}

void
SideShield::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SideShield","createSurface");

  // InnerWall and OuterWall MUST be set
  if (!ExternalCut::isActive("Wall") || std::abs(outStep)>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*outStep,Y);
      if (outStep>-Geometry::zeroTol)
	ExternalCut::setCutSurf("Wall",SMap.realSurf(buildIndex+1));
    }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth,Y);
      
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(length/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(length/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

    
  return;
}

void
SideShield::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("SideShield","createObjects");

  std::string Out;

  if (outStep < -Geometry::zeroTol)  // outStep step into wall:
    {
      const std::string wallStr=ExternalCut::getRuleStr("Wall");
      Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6");
      makeCell("Slab",System,cellIndex++,mat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," -1  3 -4 5 -6");
      makeCell("Void",System,cellIndex++,0,0.0,Out+wallStr);
      Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 -6");
    }
  else
    {
      const std::string wallStr=ExternalCut::getRuleStr("Wall");
      Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 -6");
      makeCell("Slab",System,cellIndex++,mat,0.0,Out+wallStr);
    }
  addOuterSurf(Out);
  return;
}

void
SideShield::createLinks()
  /*!
    Construct the links for the system
    Note that Y points OUT of the ring
  */
{
  
  ELog::RegMethod RegA("SideShield","createLinks");

  if (outStep>-Geometry::zeroTol)
    ExternalCut::createLink("Wall",*this,0,Origin-Y*outStep,-Y);
  else
    {
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
      FixedComp::setConnect(0,Origin,-Y);
    }

  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setConnect(1,Origin+Y*depth,Y);

  return;
}

void
SideShield::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("SideShield","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

  
}  // NAMESPACE xraySystem
