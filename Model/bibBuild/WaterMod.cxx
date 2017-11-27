/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/WaterMod.cxx
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
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "WaterMod.h"


namespace bibSystem
{

WaterMod::WaterMod(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  watIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(watIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

WaterMod::WaterMod(const WaterMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  watIndex(A.watIndex),cellIndex(A.cellIndex),
  width(A.width),height(A.height),
  depth(A.depth),wallThick(A.wallThick),waterMat(A.waterMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: WaterMod to copy
  */
{}

WaterMod&
WaterMod::operator=(const WaterMod& A)
  /*!
    Assignment operator
    \param A :: WaterMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      waterMat=A.waterMat;
      wallMat=A.wallMat;
    }
  return *this;
}


WaterMod::~WaterMod()
  /*!
    Destructor
  */
{}

void
WaterMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("WaterMod","populate");

  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  sideGap=Control.EvalVar<double>(keyName+"SideGap");
  frontGap=Control.EvalVar<double>(keyName+"FrontGap");
  backGap=Control.EvalVar<double>(keyName+"BackGap");
  vertGap=Control.EvalVar<double>(keyName+"VertGap");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");  
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");  

  return;
}


void
WaterMod::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("WaterMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
WaterMod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("WaterMod","createSurfaces");

  ModelSupport::buildPlane(SMap,watIndex+1,Origin-Y*depth/2.0,Y);  
  ModelSupport::buildPlane(SMap,watIndex+2,Origin+Y*depth/2.0,Y);  
  ModelSupport::buildPlane(SMap,watIndex+3,Origin-X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,watIndex+4,Origin+X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,watIndex+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,watIndex+6,Origin+Z*height/2.0,Z);  

  ModelSupport::buildPlane(SMap,watIndex+11,Origin-Y*(depth/2.0+wallThick),Y);  
  ModelSupport::buildPlane(SMap,watIndex+12,Origin+Y*(depth/2.0+wallThick),Y);  
  ModelSupport::buildPlane(SMap,watIndex+13,Origin-X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,watIndex+14,Origin+X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,watIndex+15,Origin-Z*(height/2.0+wallThick),Z); 
  ModelSupport::buildPlane(SMap,watIndex+16,Origin+Z*(height/2.0+wallThick),Z); 

  ModelSupport::buildPlane(SMap,watIndex+21,
			   Origin-Y*(depth/2.0+wallThick+frontGap),Y);  
  ModelSupport::buildPlane(SMap,watIndex+22,Origin+
			   Y*(depth/2.0+wallThick+backGap),Y);  
  ModelSupport::buildPlane(SMap,watIndex+23,Origin-
			   X*(width/2.0+wallThick+sideGap),X);  
  ModelSupport::buildPlane(SMap,watIndex+24,Origin+
			   X*(width/2.0+wallThick+sideGap),X);  
  ModelSupport::buildPlane(SMap,watIndex+25,Origin-
			   Z*(height/2.0+wallThick+vertGap),Z); 
  ModelSupport::buildPlane(SMap,watIndex+26,Origin+
			   Z*(height/2.0+wallThick+vertGap),Z); 

  return; 
}

void
WaterMod::createObjects(Simulation& System,
			const attachSystem::ContainedComp& CC)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("WaterMod","createObjects");
  
  std::string Out;

  // Water
  Out=ModelSupport::getComposite(SMap,watIndex,"1 -2 3 -4 5 -6");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,waterTemp,Out));

  // Wall of water moderator
  Out=ModelSupport::getComposite(SMap,watIndex,
				 "11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,waterTemp,Out));

  // Wall of water moderator
  Out=ModelSupport::getComposite(SMap,watIndex,
	     	 "21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)");
  Out+=CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,watIndex,"21 -22 23 -24 25 -26" );
  addOuterSurf(Out);

  return; 
}

void
WaterMod::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("WaterMod","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
  FixedComp::setConnect(0,Origin-Y*(depth/2.0+wallThick+frontGap),-Y);  
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+wallThick+backGap),Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick+sideGap),-X);  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick+sideGap),X);  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick+vertGap),-Z);  
  FixedComp::setConnect(5,Origin+Y*(height/2.0+wallThick+vertGap),Z);  

  // WHY HAVENT I WRITTEN AutoLinkSurf()
  FixedComp::setLinkSurf(0,-SMap.realSurf(watIndex+21));
  FixedComp::setLinkSurf(1,SMap.realSurf(watIndex+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(watIndex+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(watIndex+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(watIndex+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(watIndex+26));
  
  return;
}

void
WaterMod::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex,
		    const attachSystem::ContainedComp& CC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Side index
   */
{
  ELog::RegMethod RegA("WaterMod","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,CC);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
