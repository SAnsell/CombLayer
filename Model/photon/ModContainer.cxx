/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/ModContainer.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "RingSeal.h"
#include "RingFlange.h"
#include "ModContainer.h"


namespace photonSystem
{
      
ModContainer::ModContainer(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,12),
  attachSystem::CellMap(),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(modIndex+1),
  FrontFlange(new constructSystem::RingFlange(keyName+"FFlange")),
  BackFlange(new constructSystem::RingFlange(keyName+"BFlange"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(FrontFlange);
  OR.addObject(BackFlange);

}

ModContainer::ModContainer(const ModContainer& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),  
  modIndex(A.modIndex),cellIndex(A.cellIndex),length(A.length),
  radius(A.radius),thick(A.thick),mat(A.mat),temp(A.temp),
  FrontFlange(new constructSystem::RingFlange(*A.FrontFlange))
  
  /*!
    Copy constructor
    \param A :: ModContainer to copy
  */
{}

ModContainer&
ModContainer::operator=(const ModContainer& A)
  /*!
    Assignment operator
    \param A :: ModContainer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      radius=A.radius;
      thick=A.thick;
      mat=A.mat;
      temp=A.temp;
    }
  return *this;
}

ModContainer::~ModContainer()
  /*!
    Destructor
  */
{}

ModContainer*
ModContainer::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new ModContainer(*this);
}

void
ModContainer::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("ModContainer","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  thick=Control.EvalVar<double>(keyName+"Thick");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  temp=Control.EvalDefVar<double>(keyName+"Temp",0.0);
  
  return;
}

void
ModContainer::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point surface to use as origin/basis.
  */
{
  ELog::RegMethod RegA("ModContainer","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();


  return;
}

void
ModContainer::createSurfaces()
  /*!
    Create simple structures
  */
{
  ELog::RegMethod RegA("ModContainer","createSurfaces");

  // Outer surfaces:
  ModelSupport::buildPlane(SMap,modIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,modIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildCylinder(SMap,modIndex+7,
			      Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,modIndex+17,
			      Origin,Y,radius+thick);

  return; 
}

void
ModContainer::createObjects(Simulation& System)
  /*!
    Create the tubed moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("ModContainer","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,modIndex," 1 -2 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,modIndex," 1 -2 7 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  addCell("Skin",cellIndex-1);
  

  Out=ModelSupport::getComposite(SMap,modIndex," 1 -2 -17 ");
  addOuterSurf(Out);

  return; 
}

void
ModContainer::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("ModContainer","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(modIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(modIndex+2));

  FixedComp::setConnect(2,Origin-X*(radius+thick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(modIndex+17));
  
  FixedComp::setConnect(3,Origin+X*(radius+thick),-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(modIndex+17));

  FixedComp::setConnect(4,Origin-Z*(radius+thick),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(modIndex+17));

  FixedComp::setConnect(5,Origin+Z*(radius+thick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(modIndex+17));

  FixedComp::setConnect(8,Origin-X*radius,X);
  FixedComp::setLinkSurf(8,-SMap.realSurf(modIndex+7));

  return;
}

const attachSystem::FixedComp&
ModContainer::getFrontFlange() const
  /*!
    Access to front flange
    \return Flange object
   */
{
 return *FrontFlange;
}
 
const attachSystem::FixedComp&
ModContainer::getBackFlange() const
  /*!
    Access to back flange
    \return Flange object
   */
{
 return *BackFlange;
}
  
void
ModContainer::buildFlanges(Simulation& System)
  /*!
    Build the external flanges
    \param System :: Simulation for building
   */
{
  ELog::RegMethod RegA("ModContainer","buildFlanges");

  FrontFlange->addInsertCell(getCells("Skin"));
  FrontFlange->addInsertCell(getCells("Void"));
  FrontFlange->addInsertCell(getInsertCells());
  FrontFlange->setInnerExclude(getFullRule(3));
  FrontFlange->createAll(System,*this,2);

  
  BackFlange->addInsertCell(getCells("Skin"));
  BackFlange->addInsertCell(getCells("Void"));
  BackFlange->addInsertCell(getInsertCells());
  BackFlange->setInnerExclude(getFullRule(3));
  BackFlange->createAll(System,*this,1);

  return;
}
  
void
ModContainer::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("ModContainer","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  buildFlanges(System);
  insertObjects(System);       


  return;
}

}  // NAMESPACE photonSystem
