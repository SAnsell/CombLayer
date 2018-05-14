/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TubeDetBox.cxx
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
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "TubeDetBox.h"


namespace constructSystem
{

TubeDetBox::TubeDetBox(const std::string& Key,const size_t index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+StrFunc::makeString(index),6),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  baseName(Key),ID(index),
  detIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(detIndex+1),nDet(0)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param index :: ID nubmer
  */
{}

TubeDetBox::TubeDetBox(const TubeDetBox& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  baseName(A.baseName),ID(A.ID),detIndex(A.detIndex),
  cellIndex(A.cellIndex),centRadius(A.centRadius),
  tubeRadius(A.tubeRadius),wallThick(A.wallThick),
  height(A.height),detMat(A.detMat),wallMat(A.wallMat),
  nDet(A.nDet),DPoints(A.DPoints)
  /*!
    Copy constructor
    \param A :: TubeDetBox to copy
  */
{}

TubeDetBox&
TubeDetBox::operator=(const TubeDetBox& A)
  /*!
    Assignment operator
    \param A :: TubeDetBox to copy
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
      centRadius=A.centRadius;
      tubeRadius=A.tubeRadius;
      wallThick=A.wallThick;
      height=A.height;
      wallMat=A.wallMat;
      detMat=A.detMat;
      nDet=A.nDet;
      DPoints=A.DPoints;
    }
  return *this;
}


TubeDetBox::~TubeDetBox()
  /*!
    Destructor
   */
{}

void
TubeDetBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("TubeDetBox","populate");

  FixedOffset::populate(Control);

  active=Control.EvalDefPair<int>(keyName,baseName,"Active",1);

  tubeRadius=Control.EvalPair<double>(keyName,baseName,"TubeRadius");
  wallThick=Control.EvalPair<double>(keyName,baseName,"WallThick");
  centRadius=Control.EvalDefPair<double>(keyName,baseName,"CentRadius",0.0);

  // make a gap between tubes
  if (centRadius<tubeRadius+wallThick)
    centRadius=tubeRadius+wallThick*1.5;
  
  height=Control.EvalPair<double>(keyName,baseName,"Height");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat",
                                     baseName+"WallMat");
  detMat=ModelSupport::EvalMat<int>(Control,keyName+"DetMat",
                                     baseName+"DetMat");

  nDet=Control.EvalPair<size_t>(keyName,baseName,"NDetectors");

  gap=Control.EvalDefPair<double>(keyName,baseName,"Gap",0.5);
  outerThick=Control.EvalDefPair<double>(keyName,baseName,"OuterThick",0.5);
  if (Control.hasVariable(keyName+"OuterMat"))
    outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  else
    outerMat=ModelSupport::EvalDefMat<int>(Control,baseName+"OuterMat",-1);

  filterMat=ModelSupport::EvalDefMat<int>(Control,baseName+"FilterMat",0);
  filterMat=ModelSupport::EvalDefMat<int>(Control,keyName+"FilterMat",filterMat);
  return;
}

void
TubeDetBox::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp for origin
  */
{
  ELog::RegMethod RegA("TubeDetBox","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
TubeDetBox::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("TubeDetBox","createSurfaces");



  //main inner box
  
  // tube caps:
  ModelSupport::buildPlane(SMap,detIndex+5,
			   Origin-Z*(height/2.0),Z);    
  ModelSupport::buildPlane(SMap,detIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);    
  ModelSupport::buildPlane(SMap,detIndex+6,
			   Origin+Z*(height/2.0),Z);    
  ModelSupport::buildPlane(SMap,detIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);    

  
  const Geometry::Vec3D XGap(X*(2.0*centRadius));

  if (outerMat>=0)
    {
      const Geometry::Vec3D XDist=XGap*(static_cast<double>(nDet)/2.0);
      const Geometry::Vec3D ZDist=Z*(height/2.0+wallThick);

      ModelSupport::buildPlane(SMap,detIndex+2001,Origin-Y*(centRadius+gap),Y);
      ModelSupport::buildPlane(SMap,detIndex+2002,Origin+Y*(centRadius+gap),Y);
      ModelSupport::buildPlane(SMap,detIndex+2003,Origin-XDist-X*gap,X);
      ModelSupport::buildPlane(SMap,detIndex+2004,Origin+XDist+X*gap,X);

      ModelSupport::buildPlane(SMap,detIndex+2005,Origin-ZDist-Z*gap,Z);
      ModelSupport::buildPlane(SMap,detIndex+2006,Origin+ZDist+Z*gap,Z);

      ModelSupport::buildPlane(SMap,detIndex+2012,
			       Origin+Y*(centRadius+gap+outerThick),Y);
      ModelSupport::buildPlane(SMap,detIndex+2013,
			       Origin-XDist-X*(gap+outerThick),X);
      ModelSupport::buildPlane(SMap,detIndex+2014,
			       Origin+XDist+X*(gap+outerThick),X);

      ModelSupport::buildPlane(SMap,detIndex+2015,
			       Origin-ZDist-Z*(gap+outerThick),Z);
      ModelSupport::buildPlane(SMap,detIndex+2016,
			       Origin+ZDist+Z*(gap+outerThick),Z);
    }


  if (nDet)
    {  
      Geometry::Vec3D tubeCent(Origin-XGap*(static_cast<double>(nDet-1)/2.0));
      int DI(detIndex);
      for(size_t i=0;i<nDet;i++)
	{
	  ModelSupport::buildCylinder(SMap,DI+7,tubeCent,Z,tubeRadius);
	  ModelSupport::buildCylinder(SMap,DI+17,tubeCent,Z,
				      tubeRadius+wallThick);
	  DI+=100;
	  tubeCent+=XGap;
	}
      
      ModelSupport::buildPlane(SMap,detIndex+1001,Origin-Y*centRadius,Y);
      ModelSupport::buildPlane(SMap,detIndex+1002,Origin+Y*centRadius,Y);
      
      ModelSupport::buildPlane(SMap,detIndex+1003,
			       Origin-XGap*(static_cast<double>(nDet)/2.0),X);
      ModelSupport::buildPlane(SMap,detIndex+1004,
			       Origin+XGap*(static_cast<double>(nDet)/2.0),X);
    }

  
  return; 
}

void
TubeDetBox::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("TubeDetBox","createObjects");

  if (active)
    {
      std::string Out;
      std::string mainBody;
      
      int DI(detIndex);
      for(size_t i=0;i<nDet;i++)
        {
          Out=ModelSupport::getComposite(SMap,detIndex,DI," 5 -6 -7M ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,detMat,0.0,Out));
          addCell("Tubes",cellIndex-1);
          addCell("Tube"+StrFunc::makeString(i),cellIndex-1);
          
          Out=ModelSupport::getComposite(SMap,detIndex,DI,
                                         " 15 -16 (-5:6:7M) -17M ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
          
          mainBody+=ModelSupport::getComposite(SMap,DI," 17 ");
          DI+=100;
        }
	  // Main body:
      Out= ModelSupport::getComposite(SMap,detIndex,
					  " 1001 -1002 15 -16  1003 -1004 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+mainBody));
      if (outerMat>=0) 
	{
	  Out= ModelSupport::getComposite
	    (SMap,detIndex," 1001 -2002 2005 -2006  2003 -2004  "
                           " (1002:-15:16:-1003:1004)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
	  // filter
	  Out= ModelSupport::getComposite
	    (SMap,detIndex," 2001 -1001 -2002 2005 -2006  2003 -2004  ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,filterMat,0.0,Out));
	  // outer wall
	  Out= ModelSupport::getComposite
	    (SMap,detIndex," 2001 -2012 2015 -2016  2013 -2014  "
                           " (2002:-2003:2004:-2005:2006)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));
	  Out= ModelSupport::getComposite
	    (SMap,detIndex," 2001 -2012 2015 -2016  2013 -2014 ");
	}
      addOuterSurf(Out);
    }
  return; 
}

void
TubeDetBox::createLinks()
  /*!
    Creates a full attachment set
    Currently only the tube centres
  */
{
  ELog::RegMethod RegA("TubeDetBox","createLinks");
  
  FixedComp::setNConnect(nDet+6);

  if (nDet)
    {
      int DI(detIndex);
      const Geometry::Vec3D XGap(X*(2.0*centRadius));
      Geometry::Vec3D tubeCent(Origin-XGap*(static_cast<double>(nDet-1)/2.0));
      for(size_t i=0;i<nDet;i++)
	{
	  FixedComp::setConnect(i,tubeCent,-Y);
	  FixedComp::setLinkSurf(i,SMap.realSurf(DI+7));
	  tubeCent+=XGap;
	  DI+=100;
	}
    }
  
  return;
}

void
TubeDetBox::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to add
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("TubeDetBox","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
