/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/BulletPlates.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Goran Skoro
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
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BulletPlates.h"

namespace ts1System
{

BulletPlates::BulletPlates(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


BulletPlates::~BulletPlates() 
 /*!
   Destructor
 */
{}

void
BulletPlates::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("BulletPlates","populate");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  // Blocks:
  nBlock=Control.EvalVar<size_t>(keyName+"NBlocks");
  for(size_t i=0;i<nBlock;i++)
    {
      const std::string BNum=StrFunc::makeString(i);
      tBlock.push_back(Control.EvalVar<double>
		       (keyName+"Thick"+BNum));
      blockType.push_back(Control.EvalVar<std::string>
			  (keyName+"BlockType"+BNum));
    }


  radius=Control.EvalVar<double>(keyName+"Radius");
  taThick=Control.EvalVar<double>(keyName+"TaThick");
  
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}
  
void
BulletPlates::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed compontent [front of target void vessel]
    - Y Down the beamline
  */
{
  ELog::RegMethod RegA("BulletPlates","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  
  return;
}

void
BulletPlates::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BulletPlates","createSurface");

  // First layer [Bulk]
  ModelSupport::buildCylinder(SMap,ptIndex+7,
			      Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,ptIndex+17,
			      Origin,Y,radius+taThick);
  Geometry::Vec3D FPt(Origin);
  int SI(ptIndex);
  for(size_t i=0;i<nBlock;i++)
    {
      if (blockType[i]=="Tungsten")
	{
	  ModelSupport::buildPlane(SMap,SI+1,FPt,Y);           // ta front
	  ModelSupport::buildPlane(SMap,SI+11,FPt+Y*taThick,Y);  // W front
	  ModelSupport::buildPlane(SMap,SI+12,
				   FPt+Y*(taThick+tBlock[i]),Y);  // W Back
	  ModelSupport::buildPlane(SMap,SI+2,
				   FPt+Y*(2*taThick+tBlock[i]),Y); // taBack
	  FPt+=Y*(tBlock[i]+2*taThick);
	}
      else if (blockType[i]=="Water")
	{
	  ModelSupport::buildPlane(SMap,SI+1,FPt,Y);           // front
	  ModelSupport::buildPlane(SMap,SI+2,FPt+Y*tBlock[i],Y);  // H2O back
	  FPt+=Y*tBlock[i];
	}
      SI+=100;
    }

  // set link
  FixedComp::setConnect(1,FPt,Y);
  
  return;
}

void
BulletPlates::createObjects(Simulation& System)
  /*!
    Adds the target component
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BulletPlates","createObjects");

  std::string Out;
  
  int SI(ptIndex);
  for(size_t i=0;i<nBlock;i++)
    {
      if (blockType[i]=="Tungsten")
	{
	  Out=ModelSupport::getComposite(SMap,ptIndex,SI,"1M -11M -17");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

	  Out=ModelSupport::getComposite(SMap,ptIndex,SI,"11M -12M -7");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));

	  Out=ModelSupport::getComposite(SMap,ptIndex,SI,"11M -12M 7 -17");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

	  Out=ModelSupport::getComposite(SMap,ptIndex,SI,"12M -2M -17");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
	}
      else if (blockType[i]=="Water")
	{
	  Out=ModelSupport::getComposite(SMap,ptIndex,SI,"1M -2M -17");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
	}
      SI+=100;
    }

  SI-=100;
  Out=ModelSupport::getComposite(SMap,ptIndex,SI,"1 -17 -2M");
  addOuterSurf(Out);
  
  return;
}

void
BulletPlates::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("BulletPlates","createLinks");


  FixedComp::setConnect(0,Origin,-Y);


  FixedComp::setConnect(2,Origin-X*(radius+taThick),-X);
  FixedComp::setConnect(3,Origin+X*(radius+taThick),X);
  FixedComp::setConnect(4,Origin-Z*(radius+taThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(radius+taThick),Z);

  // Set Connect surfaces:
  const int SI(ptIndex+static_cast<int>(nBlock-1)*100);
  FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(ptIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(ptIndex+17));
  FixedComp::setLinkSurf(4,SMap.realSurf(ptIndex+17));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+17));

  return;
}

void
BulletPlates::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("BulletPlates","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
