/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItem/CryoMagnetBase.cxx
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
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"

#include "CryoMagnetBase.h"

namespace constructSystem
{

      
CryoMagnetBase::CryoMagnetBase(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  layerIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(layerIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CryoMagnetBase::~CryoMagnetBase()
  /*!
    Destructor
  */
{}


void
CryoMagnetBase::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","populate");

  FixedOffset::populate(Control);

  // Master values
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  double T,R,Tmp;
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string KN=std::to_string(i);
      R=Control.EvalVar<double>(keyName+"LRadius"+KN);
      T=Control.EvalPair<double>(keyName+"LThick"+KN,keyName+"LThick");
      Tmp=Control.EvalPair<double>(keyName+"LTemp"+KN,keyName+"LTemp");
      if  (R>outerRadius-Geometry::zeroTol)
	throw ColErr::OrderError<double>(R,outerRadius,"R / outerRadius");
      if  (i && R<LRad.back()-Geometry::zeroTol)
	throw ColErr::OrderError<double>(R,LRad.back(),"R / LRad");
      LRad.push_back(R);
      LThick.push_back(T);
      LTemp.push_back(Tmp);
    }
  
  // Master values
  topOffset=Control.EvalVar<double>(keyName+"TopOffset");
  baseOffset=Control.EvalVar<double>(keyName+"BaseOffset");

  cutTopAngle=Control.EvalVar<double>(keyName+"CutTopAngle");
  cutBaseAngle=Control.EvalVar<double>(keyName+"CutBaseAngle");

  topThick=Control.EvalVar<double>(keyName+"TopThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");

  apertureWidth=Control.EvalVar<double>(keyName+"ApertureWidth");
  apertureHeight=Control.EvalVar<double>(keyName+"ApertureHeight");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  
  
  return;
}

void
CryoMagnetBase::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
CryoMagnetBase::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","createSurfaces");

  int SN(layerIndex+100);
  for(size_t i=0;i<LRad.size();i++)
    {
      ModelSupport::buildCylinder(SMap,SN+7,Origin,Z,LRad[i]);
      ModelSupport::buildCylinder(SMap,SN+17,Origin,Z,LRad[i]+LThick[i]);
      SN+=50;
    }
  ModelSupport::buildCylinder(SMap,layerIndex+7,Origin,Z,outerRadius);
  
  ModelSupport::buildCone(SMap,layerIndex+9,
			  Origin-Z*baseOffset,-Z,cutBaseAngle);
  
  ModelSupport::buildCone(SMap,layerIndex+19,
			  Origin+Z*topOffset,Z,cutTopAngle);

  ModelSupport::buildPlane(SMap,layerIndex+105,Origin-Z*baseOffset,Z);
  ModelSupport::buildPlane(SMap,layerIndex+106,Origin+Z*topOffset,Z);

  ModelSupport::buildPlane(SMap,layerIndex+5,Origin-Z*(baseOffset+baseThick),Z);
  ModelSupport::buildPlane(SMap,layerIndex+6,Origin+Z*(topOffset+topThick),Z);

  return; 
}

void
CryoMagnetBase::createObjects(Simulation& System)
  /*!
    Create the cryo magnet unit
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CryoMagnetBase","createObjects");

  std::string Out;

  for(size_t i=0;i<LRad.size();i++)
    {
      
    }
  Out=ModelSupport::getComposite(SMap,layerIndex," 5 -6 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,layerIndex," 5 -6 -7");
  addOuterSurf(Out);
  return; 
}

void
CryoMagnetBase::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("CryoMagnetBase","createLinks");

  return;
}

void
CryoMagnetBase::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("CryoMagnetBase","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
