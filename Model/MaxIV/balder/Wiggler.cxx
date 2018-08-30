/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderWiggler.cxx
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
#include <array>

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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "Wiggler.h"

namespace xraySystem
{

Wiggler::Wiggler(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  wigIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(wigIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Wiggler::~Wiggler() 
  /*!
    Destructor
  */
{}

void
Wiggler::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Wiggler","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  blockWidth=Control.EvalVar<double>(keyName+"BlockWidth");
  blockDepth=Control.EvalVar<double>(keyName+"BlockDepth");
  blockHGap=Control.EvalVar<double>(keyName+"BlockHGap");
  blockVGap=Control.EvalVar<double>(keyName+"BlockVGap");

  blockVCorner=Control.EvalDefVar<double>(keyName+"BlockVCorner",-1.0);
  blockHCorner=Control.EvalDefVar<double>(keyName+"BlockHCorner",-1.0);

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  blockMat=ModelSupport::EvalMat<int>(Control,keyName+"BlockMat");

  return;
}

void
Wiggler::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Wiggler","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
Wiggler::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Wiggler","createSurfaces");

  // Outer surf
  ModelSupport::buildPlane(SMap,wigIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,wigIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,wigIndex+3,
			   Origin-X*(blockWidth+blockHGap/2.0),X);
  ModelSupport::buildPlane(SMap,wigIndex+4,
			   Origin+X*(blockWidth+blockHGap/2.0),X);

  ModelSupport::buildPlane(SMap,wigIndex+5,
			   Origin-Z*(blockDepth+blockVGap/2.0),Z);
  ModelSupport::buildPlane(SMap,wigIndex+6,
			   Origin+Z*(blockDepth+blockVGap/2.0),Z);


  // Inner block surf:
  ModelSupport::buildPlane(SMap,wigIndex+13,Origin-X*(blockHGap/2.0),X);
  ModelSupport::buildPlane(SMap,wigIndex+14,Origin+X*(blockHGap/2.0),X);

  ModelSupport::buildPlane(SMap,wigIndex+15,Origin-Z*(blockVGap/2.0),Z);
  ModelSupport::buildPlane(SMap,wigIndex+16,Origin+Z*(blockVGap/2.0),Z);

  // care here because undefined behavour if either VCorner / HCorner
  // equal zero.
  if (blockVCorner>Geometry::zeroTol &&
      blockHCorner>Geometry::zeroTol)
    {
      // All normalize into centre:
      Geometry::Vec3D CX(Origin-X*(blockHGap/2.0)-Z*(blockVGap/2.0));
      ModelSupport::buildPlane(SMap,wigIndex+103,
			       CX-X*blockHCorner,
			       CX-Z*blockVCorner,
			       CX-Z*blockVCorner+Y*length,
			       Origin-CX);
      CX=Origin+X*(blockHGap/2.0)-Z*(blockVGap/2.0);
      ModelSupport::buildPlane(SMap,wigIndex+104,
			       CX+X*blockHCorner,
			       CX-Z*blockVCorner,
			       CX-Z*blockVCorner+Y*length,
			       Origin-CX);
      CX=Origin-X*(blockHGap/2.0)+Z*(blockVGap/2.0);
      ModelSupport::buildPlane(SMap,wigIndex+113,
			       CX-X*blockHCorner,
			       CX+Z*blockVCorner,
			       CX+Z*blockVCorner+Y*length,
			       Origin-CX);
      CX=Origin+X*(blockHGap/2.0)+Z*(blockVGap/2.0);
      ModelSupport::buildPlane(SMap,wigIndex+114,
			       CX+X*blockHCorner,
			       CX+Z*blockVCorner,
			       CX+Z*blockVCorner+Y*length,
			       Origin-CX);
    }			       
  
  return;
}

void
Wiggler::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Wiggler","createObjects");

  std::string Out;

  // four blocks
  Out=ModelSupport::getSetComposite(SMap,wigIndex,"1 -2 3 -13 5 -15 -103 ");
  makeCell("BlockDL",System,cellIndex++,blockMat,0.0,Out);

  Out=ModelSupport::getSetComposite(SMap,wigIndex,"1 -2 14 -4 5 -15 -104 ");
  makeCell("BlockDR",System,cellIndex++,blockMat,0.0,Out);

  Out=ModelSupport::getSetComposite(SMap,wigIndex,"1 -2 3 -13 16 -6 -113 ");
  makeCell("BlockUL",System,cellIndex++,blockMat,0.0,Out);

  Out=ModelSupport::getSetComposite(SMap,wigIndex,"1 -2 14 -4 16 -6 -114 ");
  makeCell("BlockUR",System,cellIndex++,blockMat,0.0,Out);

  // care here because undefined behavour if either VCorner / HCorner
  // equal zero.
  if (blockVCorner>Geometry::zeroTol &&
      blockHCorner>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 103 -13 -15 ");
      makeCell("CornerDL",System,cellIndex++,voidMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 104 14 -15 ");
      makeCell("CornerDR",System,cellIndex++,voidMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 113 -13 16 ");
      makeCell("CornerUL",System,cellIndex++,voidMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 114 14 16 ");
      makeCell("CornerUR",System,cellIndex++,voidMat,0.0,Out);
    }
  
  if (blockHGap>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 13 -14 5 -15 ");
      makeCell("HGap",System,cellIndex++,voidMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 13 -14 16 -6 ");
      makeCell("HGap",System,cellIndex++,voidMat,0.0,Out);
    }
  if (blockVGap>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 3 -4 15 -16 ");
      makeCell("VGap",System,cellIndex++,voidMat,0.0,Out);
    }

  
  Out=ModelSupport::getComposite(SMap,wigIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);      

  return;
}

void
Wiggler::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Wiggler","createLinks");
  
  setConnect(0,Origin-Y*(length/2.0),-Y);
  setConnect(1,Origin+Y*(length/2.0),Y);
  
  setLinkSurf(0,-SMap.realSurf(wigIndex+1));
  setLinkSurf(1,SMap.realSurf(wigIndex+2));

  return;
}

void
Wiggler::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Wiggler","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
