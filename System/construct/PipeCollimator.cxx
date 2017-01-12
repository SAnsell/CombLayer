/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PipeCollimator.cxx
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PipeCollimator.h"


namespace constructSystem
{

PipeCollimator::PipeCollimator(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,2),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  collIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(collIndex+1),setFlag(0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  

PipeCollimator::PipeCollimator(const PipeCollimator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  collIndex(A.collIndex),cellIndex(A.cellIndex),
  setFlag(A.setFlag),innerStruct(A.innerStruct),
  outerStruct(A.outerStruct),length(A.length),mat(A.mat)
  /*!
    Copy constructor
    \param A :: PipeCollimator to copy
  */
{}

PipeCollimator&
PipeCollimator::operator=(const PipeCollimator& A)
  /*!
    Assignment operator
    \param A :: PipeCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      setFlag=A.setFlag;
      innerStruct=A.innerStruct;
      outerStruct=A.outerStruct;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}


void
PipeCollimator::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PipeCollimator","populate");

  FixedOffset::populate(Control);
  length=Control.EvalVar<double>(keyName+"Length");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  
  return;
}

void
PipeCollimator::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("PipeCollimator","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}


void
PipeCollimator::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("PipeCollimator","createSurface");

  ModelSupport::buildPlane(SMap,collIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,collIndex+2,Origin+Y*(length/2.0),Y);
  
  return;
}

void
PipeCollimator::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("PipeCollimator","createObjects");
  std::string Out;

  if ((setFlag & 2) !=2)
    throw ColErr::EmptyContainer("outerStruct not set");

  Out=ModelSupport::getComposite(SMap,collIndex,"1 -2");
  Out+=innerStruct.display()+outerStruct.display();
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  addCell("Main",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,collIndex,"1 -2");
  addOuterSurf(Out);
  return;
}

void
PipeCollimator::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("PipeCollimator","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(collIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(collIndex+2));      
  
  return;
}

void
PipeCollimator::setInner(const HeadRule& HR)
  /*!
    Set the inner volume
    \param HR :: Headrule of inner surfaces
  */
{
  ELog::RegMethod RegA("PipeCollimator","setInner");

  innerStruct=HR;
  innerStruct.makeComplement();
  setFlag ^= 1;
  return;
}

void
PipeCollimator::setInnerExclude(const HeadRule& HR)
  /*!
    Set the inner volume (without inverse)
    \param HR :: Headrule of inner surfaces
  */
{
  ELog::RegMethod RegA("PipeCollimator","setInnerExclude");

  innerStruct=HR;
  setFlag ^= 1;
  return;
}

void
PipeCollimator::setOuter(const HeadRule& HR)
  /*!
    Set the outer volume
    \param HR :: Headrule of outer surfaces
  */
{
  ELog::RegMethod RegA("PipeCollimator","setInner");
  
  outerStruct=HR;
  setFlag ^= 2;
  return;
}
  
void
PipeCollimator::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("PipeCollimator","createAllNoPopulate");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
