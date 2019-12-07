/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/WallCut.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LinkSupport.h"
#include "ExternalCut.h"
#include "WallCut.h"


namespace constructSystem
{

WallCut::WallCut(const std::string& Key,const size_t ID)  :
  attachSystem::FixedOffset(Key+std::to_string(ID),6),
  attachSystem::ContainedComp(),attachSystem::ExternalCut(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param ID :: Index number
  */
{}

WallCut::WallCut(const WallCut& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  baseName(A.baseName),insertKey(A.insertKey),
  height(A.height),width(A.width),length(A.length),
  mat(A.mat),matTemp(A.matTemp)
  /*!
    Copy constructor
    \param A :: WallCut to copy
  */
{}

WallCut&
WallCut::operator=(const WallCut& A)
  /*!
    Assignment operator
    \param A :: WallCut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      insertKey=A.insertKey;
      height=A.height;
      width=A.width;
      length=A.length;
      mat=A.mat;
      matTemp=A.matTemp;
    }
  return *this;
}

  
WallCut::~WallCut() 
  /*!
    Destructor
  */
{}

void
WallCut::populateKey(const FuncDataBase& Control)
  /*!
    Populate key : Determine the keyname for insertion
    \param Control :: Variable Database
  */
{  
  ELog::RegMethod RegA("WallCut","populateKey");
  
  insertKey=Control.EvalTail<std::string>(keyName,baseName,"InsertKey");
  return;
}
  
void
WallCut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to copy
  */
{
  ELog::RegMethod RegA("WallCut","populate");

  FixedOffset::populate(baseName,Control);

  height=Control.EvalTail<double>(keyName,baseName,"Height");
  width=Control.EvalTail<double>(keyName,baseName,"Width");
  length=Control.EvalTail<double>(keyName,baseName,"Length");


  mat=ModelSupport::EvalDefMat<int>(Control,keyName+"Mat",
				    baseName+"Mat",0);
  matTemp=Control.EvalDefTail<double>
    (keyName,baseName,"MatTemp",0.0);
  
  return;
}
  
void
WallCut::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit that it is connected to 
    \param sideIndex :: Index for side
  */
{
  ELog::RegMethod RegA("WallCut","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  FixedOffset::applyOffset();

  return;
}

void
WallCut::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("WallCut","createSurfaces");
    

  if (length>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),Y);  
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),Y);
    }
  if (width>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,buildIndex+3,
			       Origin-X*(width/2.0),X);  
      ModelSupport::buildPlane(SMap,buildIndex+4,
			       Origin+X*(width/2.0),X);
    }
  if (height>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,buildIndex+5,
			       Origin-Z*(height/2.0),Z);  
      ModelSupport::buildPlane(SMap,buildIndex+6,
			       Origin+Z*(height/2.0),Z);
    }

 return;
}


void
WallCut::createObjects(Simulation& System)
  /*!
    Adds the main component
    \param System :: Simulation to create objects in
    \param wallBoundary :: External boundary rule 
  */
{
  ELog::RegMethod RegA("WallCut","createObjects");

  const HeadRule& wallBoundary=
    ExternalCut::getRule("WallBoundary");
  
  std::string Out;
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  Out+=wallBoundary.display();
  System.addCell(MonteCarlo::Object(cellIndex++,mat,matTemp,Out));
  
  return;
}

void
WallCut::createLinks()
  /*!
    Create linkes
  */
{
  ELog::RegMethod RegA("WallCut","createLinks");

  const HeadRule& wallBoundary=
    ExternalCut::getRule("WallBoundary");
    
  if (length>Geometry::zeroTol)
    {
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
      FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
      FixedComp::setConnect(1,Origin+Y*(length/2.0),Y); 
    }
  else
    {
      attachSystem::calcBoundaryLink(*this,0,wallBoundary,Origin,-Y);
      attachSystem::calcBoundaryLink(*this,1,wallBoundary,Origin,Y);
    }

  if (width>Geometry::zeroTol)
    {
      FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
      FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
      FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
      FixedComp::setConnect(3,Origin+X*(width/2.0),X); 
    }
  else
    {
      attachSystem::calcBoundaryLink(*this,2,wallBoundary,Origin,-X);
      attachSystem::calcBoundaryLink(*this,3,wallBoundary,Origin,X);
    }
  
  if (height>Geometry::zeroTol)
    {
      FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
      FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
      FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
      FixedComp::setConnect(5,Origin+Z*(height/2.0),Z); 
    }
  else
    {
      attachSystem::calcBoundaryLink(*this,4,wallBoundary,Origin,-Z);
      attachSystem::calcBoundaryLink(*this,5,wallBoundary,Origin,Z);
    }

  
  return;
}

   
void
WallCut::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FC :: Object for origin
    \param sideIndex :: linkPoint for axis
    \param wallBoundary :: External boundary rule 
  */
{
  ELog::RegMethod RegA("WallCut","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE constructSystem
