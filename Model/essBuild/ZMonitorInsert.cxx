/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ZMonitorInsert.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ZMonitorInsert.h"

namespace essSystem
{

ZMonitorInsert::ZMonitorInsert(const std::string& Key)  :
  attachSystem::ContainedGroup("Full"),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  monIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(monIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index of guide unit
  */
{}


ZMonitorInsert::~ZMonitorInsert() 
  /*!
    Destructor
  */
{}


void
ZMonitorInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Func Data Base to use
  */
{
  ELog::RegMethod RegA("ZMonitorInsert","populate");

  FixedOffset::populate(Control);

  nSegments=Control.EvalVar<size_t>(keyName+"NSegments");

  double IR,OR,H,AA;
  int M;
  for(size_t i=0;i<nSegments;i++)
    {
      const std::string numStr(std::to_string(i));
      IR=Control.EvalVar<double>(keyName+"InnerRadius"+numStr);
      OR=Control.EvalVar<double>(keyName+"OuterRadius"+numStr);
      H=Control.EvalVar<double>(keyName+"Height"+numStr);
      AA=Control.EvalVar<double>(keyName+"ArcAngle"+numStr);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+numStr,keyName+"Mat");

      height.push_back(H);
      innerRadii.push_back(IR);
      outerRadii.push_back(OR);
      arcAngle.push_back(AA);
      mat.push_back(M);
    }
  return;
}
  
void
ZMonitorInsert::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Mid-point of inner bay 
  */
{
  ELog::RegMethod RegA("ZMonitorInsert","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
ZMonitorInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZMonitorInsert","createSurface");

  int GI(monIndex);
  double THeight(0.0);
  for(size_t i=0;i<nSegments;i++)
    {
      ModelSupport::buildPlane(SMap,GI+5,Origin+Z*THeight,Z);
      ModelSupport::buildCylinder(SMap,GI+7,Origin,Z,innerRadii[i]);
      ModelSupport::buildCylinder(SMap,GI+17,Origin,Z,outerRadii[i]);
      ModelSupport::buildPlaneRotAxis(SMap,GI+3,Origin,X,Z,-arcAngle[i]/2.0);
      ModelSupport::buildPlaneRotAxis(SMap,GI+4,Origin,X,Z,arcAngle[i]/2.0);
      THeight+=height[i];
      GI+=20;
    }
  ModelSupport::buildPlane(SMap,GI+5,Origin+Z*THeight,Z);
  
  return;
}


void
ZMonitorInsert::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ZMonitorInsert","createObjects");

  std::string Out;
  int GI(monIndex);
  for(size_t i=0;i<nSegments;i++)
    {
      Out=ModelSupport::getComposite(SMap,monIndex,GI," 7 -17 3 -4 5 -25 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],0,Out));
      addOuterUnionSurf("Full",Out);
      GI+=20;
    }      

  return;
}

void
ZMonitorInsert::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("ZMonitorInsert","createLinks");

    

  return;
}

void
ZMonitorInsert::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: side to used
  */
{
  ELog::RegMethod RegA("ZMonitorInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
