/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/PreMod.cxx
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
#include "surfExpand.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "PreMod.h"

namespace moderatorSystem
{

PreMod::PreMod(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  preIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(preIndex+1),centOrgFlag(1),
  divideSurf(0),targetSurf(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PreMod::PreMod(const PreMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  preIndex(A.preIndex),cellIndex(A.cellIndex),
  centOrgFlag(A.centOrgFlag),width(A.width),height(A.height),
  depth(A.depth),alThickness(A.alThickness),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat),divideSurf(A.divideSurf),
  targetSurf(A.targetSurf)
  /*!
    Copy constructor
    \param A :: PreMod to copy
  */
{}

PreMod&
PreMod::operator=(const PreMod& A)
  /*!
    Assignment operator
    \param A :: PreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      centOrgFlag=A.centOrgFlag;
      width=A.width;
      height=A.height;
      depth=A.depth;
      alThickness=A.alThickness;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      divideSurf=A.divideSurf;
      targetSurf=A.targetSurf;
    }
  return *this;
}

PreMod::~PreMod() 
  /*!
    Destructor
  */
{}

void
PreMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function dataase
  */
{
  ELog::RegMethod RegA("PreMod","populate");
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  alThickness=Control.EvalVar<double>(keyName+"AlThick");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
  
void
PreMod::createUnitVector(const attachSystem::FixedComp& FC,
                         const long int orgIndex,
			 const long int basisIndex)
  /*!
    Create the unit vectors
    \param FC :: Component to connect to
    \param orgIndex :: side to connect 
    \param basisIndex :: basis index
  */
{
  ELog::RegMethod RegA("PreMod","createUnitVector");

  FixedComp::createUnitVector(FC,orgIndex,basisIndex);
  return;
}
  
void
PreMod::createSurfaces(const attachSystem::FixedComp& FC,
		       const long int baseIndex)
  /*!
    Create All the surfaces
    \param FC :: Fixed unit that connects to this moderator
    \param baseIndex :: base number
  */
{
  ELog::RegMethod RegA("PreMod","createSurface");

  const Geometry::Vec3D& cAxis=FC.getLinkAxis(baseIndex);
  const int cFlag=(cAxis.dotProd(Z)<-0.8) ? -1 : 1;

  if (centOrgFlag)
    Origin-=Y*(depth/2.0);

  // Outer DIVIDE PLANE/Cylinder
  if (divideSurf)
    SMap.addMatch(preIndex+1,divideSurf);
  else
    ModelSupport::buildPlane(SMap,preIndex+1,Origin,Y);

  SMap.addMatch(preIndex+5,cFlag*FC.getLinkSurf(baseIndex));
  if (targetSurf)
    SMap.addMatch(preIndex+7,targetSurf);  // This is a cylinder [hopefully]

  // Outer surfaces:
  ModelSupport::buildPlane(SMap,preIndex+2,Origin+Y*depth,Y);
  ModelSupport::buildPlane(SMap,preIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,preIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,preIndex+6,Origin+cAxis*height,cAxis);

  // Inner surfaces:
  ModelSupport::buildPlane(SMap,preIndex+12,Origin+Y*(depth-alThickness),Y);
  ModelSupport::buildPlane(SMap,preIndex+13,Origin-X*(width/2.0-alThickness),X);
  ModelSupport::buildPlane(SMap,preIndex+14,Origin+X*(width/2.0-alThickness),X);
  ModelSupport::buildPlane(SMap,preIndex+16,Origin+cAxis*(height-alThickness),cAxis);

  Geometry::Surface* SX;
  Geometry::Plane* PX;  
  if (targetSurf)
    {
      SX=ModelSupport::surfaceCreateExpand
	(SMap.realSurfPtr(preIndex+7),alThickness);
      SX->setName(preIndex+17);
      SMap.registerSurf(preIndex+17,SX);
    }

  if (SMap.realSurf(preIndex+5)<0)
    {
      SX=ModelSupport::surfaceCreateExpand
	(SMap.realSurfPtr(preIndex+5),-alThickness);
      PX=dynamic_cast<Geometry::Plane*>(SX);
      if (PX)
	PX->mirrorSelf();
    }
  else
    SX=ModelSupport::surfaceCreateExpand
      (SMap.realSurfPtr(preIndex+5),alThickness);

  SX->setName(preIndex+15);
  SMap.registerSurf(preIndex+15,SX);
      
  // divider:
  if (SMap.realSurf(preIndex+1)<0)
    {
      SX=ModelSupport::surfaceCreateExpand
	(SMap.realSurfPtr(preIndex+1),-alThickness);
      PX=dynamic_cast<Geometry::Plane*>(SX);
      if (PX)
	PX->mirrorSelf();
    }
  else
    SX=ModelSupport::surfaceCreateExpand
      (SMap.realSurfPtr(preIndex+1),alThickness);

  SX->setName(preIndex+11);
  SMap.registerSurf(preIndex+11,SX);

  return;
}

void
PreMod::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreMod","createObjects");
  
  std::string Out;
  Out=ModelSupport::getSetComposite(SMap,preIndex,"1 -2 3 -4 5 -6 7");
  addOuterSurf(Out);

  Out+=ModelSupport::getSetComposite(SMap,preIndex,
				    "(-11:12:-13:14:-15:16:-17)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  Out=ModelSupport::getSetComposite(SMap,preIndex,"11 -12 13 -14 15 -16 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  return;
}

void
PreMod::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("PreMod","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(preIndex+1));
  
  FixedComp::setConnect(1,Origin+Y*depth,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(preIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(preIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(preIndex+4));
        
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(preIndex+5));

  FixedComp::setConnect(4,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(preIndex+6));
  
  return;
}
  
void
PreMod::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		  const long int baseIndex,
		  const bool rFlag)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param baseIndex :: base number
    \param FC :: Fixed unit that connects to this moderator
    \param rFlag :: Decide if to rotate 180 about Z axis
  */
{
  ELog::RegMethod RegA("PreMod","createAll");
  populate(System.getDataBase());  
  createUnitVector(FC,baseIndex,0);
  
  if (rFlag) FixedComp::applyRotation(Z,180.0);
  createSurfaces(FC,baseIndex);
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
