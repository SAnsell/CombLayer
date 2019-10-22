/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   saxsModel/AreaPlate.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "AreaPlate.h"

namespace saxsSystem
{

AreaPlate::AreaPlate(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,3),attachSystem::CellMap(),
  NWBin(1),NHBin(1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

AreaPlate::~AreaPlate()
  /*!
    Destructor
   */
{}

void
AreaPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("AreaPlate","populate");

  FixedRotate::populate(Control);

  NWBin=Control.EvalVar<size_t>(keyName+"NWBin");
  NHBin=Control.EvalVar<size_t>(keyName+"NHBin");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  thick=Control.EvalVar<double>(keyName+"Thick");
  
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}

void
AreaPlate::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("AreaPlate","createSurfaces");

  const double WStep=width/static_cast<double>(NWBin);
  const double HStep=height/static_cast<double>(NHBin);


  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(thick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(thick/2.0),Y);    

  int BI(buildIndex+100);
  Geometry::Vec3D WPt(Origin-X*(WStep*static_cast<double>(NWBin)/2.0));

  for(size_t i=0;i<=NWBin;i++)
    {
      ModelSupport::buildPlane(SMap,BI+3,WPt,X);
      WPt+=X*HStep;
      BI+=10;
    }

  
  BI=buildIndex+100;
  Geometry::Vec3D HPt(Origin-Z*(HStep*static_cast<double>(NHBin)/2.0));

  for(size_t i=0;i<=NHBin;i++)
    {
      ModelSupport::buildPlane(SMap,BI+5,HPt,Z);
      HPt+=Z*HStep;
      BI+=10;
    }

  return; 
}

void
AreaPlate::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("AreaPlate","createObjects");

  std::string Out;
  // First make inner/outer void/wall and top/base

  int BWI(buildIndex+100);
  int BHI(buildIndex+100);
  for(size_t i=0;i<NWBin;i++)
    {
      BHI=buildIndex+100;
      const std::string IStr(std::to_string(i));
	  
      for(size_t j=0;j<NHBin;j++)
	{
	  const std::string JStr(std::to_string(j));
	  Out=ModelSupport::getComposite
	    (SMap,buildIndex,BWI,BHI,"1 -2 3M -13M 5N -15N ");
	  makeCell("DetCell"+IStr+"x"+JStr,
		   System,cellIndex++,mainMat,0.0,Out);
	  BHI+=10;
	}
      BWI+=10;
    }

  Out=ModelSupport::getComposite
    (SMap,buildIndex,BWI,BHI,"1 -2 103 105 -3M -5N ");
  addOuterSurf(Out);


  return; 
}

void
AreaPlate::createLinks()
  /*!
    Creates a full attachment set
  */
{
  
  return;
}


void
AreaPlate::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
/*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to add
    \param sideIndex :: link point						
   */
{
  ELog::RegMethod RegA("AreaPlate","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
