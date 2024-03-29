/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/BeamShutter.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "BeamShutter.h"


namespace constructSystem
{

BeamShutter::BeamShutter(const std::string& Key) :
  attachSystem::FixedRotateGroup(Key,"Main",6,"Void",2,"Beam",2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  

void
BeamShutter::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BeamShutter","populate");

  attachSystem::FixedRotateGroup::populate(Control);
  liftZStep=Control.EvalVar<double>(keyName+"LiftZStep");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  double T;
  double TTotal(0.0);
  for(size_t i=0;i<nLayers;i++)
    {
      const std::string IStr=std::to_string(i);
      if (i+1==nLayers)
	Thick.push_back(length-TTotal);
      else
	{
	  T=Control.EvalVar<double>(keyName+"Thick"+IStr);
	  Thick.push_back(T);
	  TTotal+=T;
	}
      Mat.push_back(ModelSupport::EvalMat<int>
		    (Control,keyName+"Mat"+IStr));
    }
  if (TTotal>length-Geometry::zeroTol)
    throw ColErr::RangeError<double>(TTotal,0.0,length,"TTotal exceeds length");

  surroundThick=Control.EvalVar<double>(keyName+"SurroundThick");
  topVoid=Control.EvalVar<double>(keyName+"TopVoid");
  surroundMat=ModelSupport::EvalMat<int>(Control,keyName+"SurroundMat");
  
  return;
}

void
BeamShutter::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("BeamShutter","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& voidFC=getKey("Void");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  beamFC.createUnitVector(FC,sideIndex);
  voidFC.createUnitVector(FC,sideIndex);
  mainFC.createUnitVector(FC,sideIndex);

  applyOffset();

  voidFC.applyShift(0,0,liftZStep);
  mainFC.applyShift(0,0,liftZStep);

  setDefault("Main");
  return;
}


void
BeamShutter::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("BeamShutter","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(surroundThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(surroundThick+width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(surroundThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(surroundThick+height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+105,
		 Origin-Z*(liftZStep+surroundThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
		 Origin+Z*(-liftZStep+topVoid+surroundThick+height/2.0),Z);

  int CN(buildIndex);
  Geometry::Vec3D POrg(Origin);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildPlane(SMap,CN+1,POrg,Y);
      POrg += Y*Thick[i];
      CN+=10;
    }
  // back plate
  ModelSupport::buildPlane(SMap,CN+1,POrg,Y);

  
  return;
}

void
BeamShutter::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("BeamShutter","createObjects");
  HeadRule HR;

  int CN(buildIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,CN,"1M -11M 3 -4 5 -6");
      makeCell("Main",System,cellIndex++,Mat[i],0.0,HR);
      CN+=10;
    }
  CN-=10;
  // Surround
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,CN,"1 -11M 13 -14 15 -16 (-3:4:-5:6)");
  makeCell("Surround",System,cellIndex++,surroundMat,0.0,HR);

  // lower void if present
  if (liftZStep>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,CN,"1 -11M 13 -14 105 -15");
      makeCell("Void",System,cellIndex++,0,0.0,HR);
    }

  // top void if present
  if (liftZStep-topVoid < Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,CN,"1 -11M 13 -14 16 -106");
      makeCell("Void",System,cellIndex++,0,0.0,HR);
	    
    }

  if (liftZStep>=Geometry::zeroTol)
    HR=ModelSupport::getHeadRule
      (SMap,buildIndex,CN,"1 -11M 13 -14 105 -106");
  else
    HR=ModelSupport::getHeadRule
      (SMap,buildIndex,CN,"1 -11M 13 -14 15 -106");
  addOuterSurf(HR);
  return;
}

void
BeamShutter::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("BeamShutter","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& voidFC=FixedGroup::getKey("Void");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*length,Y);
  mainFC.setConnect(2,Origin-X*(surroundThick+width/2.0),-X);
  mainFC.setConnect(3,Origin+X*(surroundThick+width/2.0),X);
  mainFC.setConnect(4,Origin-Z*(surroundThick+height/2.0),-Z);
  mainFC.setConnect(5,Origin-Z*(surroundThick+height/2.0),Z);

  const int CN(static_cast<int>(nLayers)*10+buildIndex);
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(CN+1));
  mainFC.setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  mainFC.setLinkSurf(3,SMap.realSurf(buildIndex+14));
  mainFC.setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  mainFC.setLinkSurf(5,SMap.realSurf(buildIndex+16));

  // These are protected from ZVertial re-orientation
  const Geometry::Vec3D& BC(beamFC.getCentre());
  
  beamFC.setConnect(0,BC,-Y);
  beamFC.setConnect(1,BC+Y*length,Y);
  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(CN+1));

  // These are protected from ZVertial re-orientation
  const Geometry::Vec3D& VC(voidFC.getCentre());
  
  voidFC.setConnect(0,VC,-Y);
  voidFC.setConnect(1,VC+Y*length,Y);
  voidFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  voidFC.setLinkSurf(1,SMap.realSurf(CN+1));

  return;
}

  
void
BeamShutter::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("BeamShutter","createAllNoPopulate");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
