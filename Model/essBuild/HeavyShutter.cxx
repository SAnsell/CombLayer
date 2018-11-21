/****************************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/HeavyShutter.cxx
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include <numeric>
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "HeavyShutter.h"

namespace essSystem
{

HeavyShutter::HeavyShutter(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

HeavyShutter::HeavyShutter(const HeavyShutter& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  width(A.height),height(A.height),
  wallThick(A.wallThick),guideLodging(A.guideLodging),separatorThick(A.separatorThick),slabThick(A.slabThick),
  mainMat(A.mainMat),slabMat(A.slabMat)
  /*!
    Copy constructor
    \param A :: HeavyShutter to copy
  */
{}

HeavyShutter&
HeavyShutter::operator=(const HeavyShutter& A)
  /*!
    Assignment operator
    \param A :: HeavyShutter to copy
    \return *this
  */
{
  if (this!=&A)
  {
    attachSystem::ContainedComp::operator=(A);
    attachSystem::FixedOffset::operator=(A);
    attachSystem::CellMap::operator=(A);
    width=A.width;
    height=A.height;
    wallThick=A.wallThick;
    guideLodging=A.guideLodging;
    separatorThick=A.separatorThick;
    slabThick=A.slabThick;
    mainMat=A.mainMat;
    slabMat=A.slabMat;
  }
  return *this;
}

HeavyShutter::~HeavyShutter() 
  /*!
    Destructor
  */
{}

void
HeavyShutter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HeavyShutter","populate");
  
  FixedOffset::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  guideLodging=Control.EvalVar<double>(keyName+"GuideLodging");
  separatorThick=Control.EvalVar<double>(keyName+"SeparatorThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  
  for (size_t i=0;i<Control.EvalVar<size_t>(keyName+"NSlabs");i++)
  {
    slabThick.push_back(Control.EvalVar<double>(keyName+"SlabThick"+std::to_string(i)));
    slabMat.push_back(ModelSupport::EvalMat<int>(Control,keyName+"SlabMat"+std::to_string(i)));
  }

  return;
}

void
HeavyShutter::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin/radius
    \param sideIndex :: Side for monolith
  */
{
  ELog::RegMethod RegA("HeavyShutter","createUnitVector");
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
HeavyShutter::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("HeavyShutter","createSurface");
  
  // Total length due to the slabs and the separators
  double length=std::accumulate(slabThick.begin(),slabThick.end(),0.0);
  length+=separatorThick*static_cast<double>(slabThick.size()-1);
  
  // Casing: internal walls
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-X*(width/2),X);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+X*(width/2),X);
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Z*(height/2),Z);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Z*(height/2),Z);
    
  // Casing: external walls
  ModelSupport::buildPlane(SMap,buildIndex+1101,Origin-X*(width/2+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+1102,Origin+X*(width/2+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+1201,Origin-Z*(height/2+wallThick+guideLodging),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1202,Origin+Z*(height/2+wallThick),Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2+wallThick),Y);  // external wall casing - first
  double thick = 0;
  for (size_t i=0;i<slabThick.size();i++)
  {
    ModelSupport::buildPlane(SMap,buildIndex+static_cast<int>(i*2+2),Origin-Y*(length/2)+Y*(thick),Y);
    ModelSupport::buildPlane(SMap,buildIndex+static_cast<int>(i*2+3),Origin-Y*(length/2)+Y*(slabThick[i]+thick),Y);
    thick+=slabThick[i]+separatorThick;
  }
  ModelSupport::buildPlane(SMap,buildIndex+static_cast<int>((slabThick.size()+1)*2),Origin+Y*(length/2+wallThick),Y);  // external wall casing - last
  
  return;
}

void
HeavyShutter::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HeavyShutter","createObjects");
  
  std::string Out;
  
  std::string word;
  
  for (size_t i=0;i<slabThick.size();i++)
  {
    word=" "+std::to_string(i*2+2)+" -"+std::to_string(i*2+3);
    word+=" +101 -102 +201 -202";
    Out=ModelSupport::getComposite(SMap,buildIndex,word);
    System.addCell(MonteCarlo::Object(cellIndex++,slabMat[i],0.0,Out));

    // Separators    
    if (i!=slabThick.size()-1)
    {
      word=" "+std::to_string(i*2+3)+" -"+std::to_string(i*2+4);
      word+=" +101 -102 +201 -202";
      Out=ModelSupport::getComposite(SMap,buildIndex,word);
      System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out));
    }
  }
  
  word=" 1 -"+std::to_string((slabThick.size()+1)*2);
  word+=" 1101 -1102 +1201 -1202";
  word+=" (-2 : "+std::to_string((slabThick.size()+1)*2-1)+" : -101 : 102 : -201 : 202 )";
  Out=ModelSupport::getComposite(SMap,buildIndex,word);
  System.addCell(MonteCarlo::Object(cellIndex++,mainMat,0.0,Out));
  addCell("Casing",cellIndex-1);
  
  word=" 1 -"+std::to_string((slabThick.size()+1)*2)+" 1101 -1102 1201 -1202";
  Out=ModelSupport::getComposite(SMap,buildIndex,word);
  addOuterSurf(Out);
  
  return;
}

void
HeavyShutter::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("HeavyShutter","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(length/2+wallThick),-Y);
  FixedComp::setConnect(1,Origin+Y*(length/2+wallThick),Y);
  FixedComp::setConnect(2,Origin-X*(width/2+wallThick),-X);
  FixedComp::setConnect(3,Origin+X*(width/2+wallThick),X);
  FixedComp::setConnect(4,Origin-Z*(height/2+wallThick+guideLodging),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2+wallThick),Z);
  
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  return;
}

void
HeavyShutter::createAll(Simulation& System,
                        const attachSystem::FixedComp& FC,
                        const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: Side of monolith
  */
{
  ELog::RegMethod RegA("HeavyShutter","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);
  
  return;
}

}  // NAMESPACE essSystem
