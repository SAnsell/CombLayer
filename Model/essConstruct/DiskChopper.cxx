/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essConstruct/DiskChopper.cxx
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
#include <array>

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

#include "DiskBlades.h"
#include "DiskChopper.h"

namespace essConstruct
{

DiskChopper::DiskChopper(const std::string& Key) : 
  attachSystem::FixedRotateGroup(Key,"Main",6,"Beam",2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  centreFlag(0),offsetFlag(0),nDisk(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

DiskChopper::DiskChopper(const DiskChopper& A) : 
  attachSystem::FixedRotateGroup(A),
  attachSystem::ContainedComp(A),attachSystem::CellMap(A),
  centreFlag(A.centreFlag),offsetFlag(A.offsetFlag),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  diskGap(A.diskGap),nDisk(A.nDisk),DInfo(A.DInfo)
  /*!
    Copy constructor
    \param A :: DiskChopper to copy
  */
{}
  
DiskChopper&
DiskChopper::operator=(const DiskChopper& A)
  /*!
    Assignment operator
    \param A :: DiskChopper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotateGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      centreFlag=A.centreFlag;
      offsetFlag=A.offsetFlag;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      diskGap=A.diskGap;
      nDisk=A.nDisk;
      DInfo=A.DInfo;
    }
  return *this;
}


DiskChopper::~DiskChopper() 
  /*!
    Destructor
  */
{}


void
DiskChopper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("DiskChopper","populate");

  FixedRotateGroup::populate(Control);

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  nDisk=Control.EvalVar<size_t>(keyName+"NDisk");
  diskGap= (nDisk>1) ?
    Control.EvalVar<double>(keyName+"Gap") : 0.0;

  totalThick=0.0;
  for(size_t i=0;i<nDisk;i++)
    {
      DiskBlades DItem;
      const std::string kN=keyName+std::to_string(i);
      DItem.thick=Control.EvalTail<double>(kN,keyName,"Thick");
      DItem.innerThick=Control.EvalDefTail<double>
        (kN,keyName,"InnerThick",DItem.thick);
      DItem.innerMat=ModelSupport::EvalMat<int>
	(Control,kN+"InnerMat",keyName+"InnerMat");
      DItem.outerMat=ModelSupport::EvalMat<int>
	(Control,kN+"OuterMat",keyName+"OuterMat");
      totalThick+=DItem.thick;
      totalThick+=(i) ? diskGap : 0.0;
      
      const size_t NB=Control.EvalTail<size_t>(kN,keyName,"NBlades");
      for(size_t j=0;j<NB;j++)
	{
	  const std::string PAStr("PhaseAngle"+std::to_string(j));
	  const std::string OAStr("OpenAngle"+std::to_string(j));
	  const double pA=Control.EvalTail<double>(kN,keyName,PAStr);
	  const double oA=Control.EvalTail<double>(kN,keyName,OAStr);
	  DItem.addOpen(pA,oA);
	}
      DInfo.push_back(DItem);
    }
  return;
}

void
DiskChopper::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("DiskChopper","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(FC,sideIndex);
  beamFC.createUnitVector(FC,sideIndex);
  applyOffset();

  beamOrigin=beamFC.getCentre();
  beamAxis=beamFC.getY();

  double XYZ[3]={0,0,0};
  if (centreFlag && centreFlag<4 && centreFlag>-4)
    {
      const size_t index=static_cast<size_t>(std::abs(centreFlag)-1);
      XYZ[index] += (centreFlag<0) ?
	-(outerRadius+innerRadius)/2.0 : (outerRadius+innerRadius)/2.0;
    }
  if (offsetFlag)
    {
      const double TD((offsetFlag>0) ? -totalThick/2.0 : totalThick/2.0);
      XYZ[1] += TD;
      beamFC.applyShift(0.0,TD,0.0);
    }
  mainFC.applyShift(XYZ[0],XYZ[1],XYZ[2]);
      
  setDefault("Main");

  return;
}

void
DiskChopper::createUnitVector(const attachSystem::FixedComp& RFC,
			      const long int rotorIndex,
                              const attachSystem::FixedComp& BFC,
                              const long int beamIndex)
  /*!
    Create the unit vectors
    \param RFC :: Fixed component to link to
    \param rotorIndex :: Link point and direction [0 for origin]
    \param BFC :: Fixed component to link to
    \param beamIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("DiskChopper","createUnitVector");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(RFC,rotorIndex);
  beamFC.createUnitVector(BFC,beamIndex);

  applyOffset();

  beamOrigin=beamFC.getCentre();
  beamAxis=beamFC.getY();

  setDefault("Main");

  return;
}

void
DiskChopper::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("DiskChopper","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,outerRadius);
  int CI(buildIndex);
  Geometry::Vec3D DCent(Origin);

  for(const DiskBlades& DRef : DInfo)
    {
      ModelSupport::buildPlane(SMap,CI+1,DCent-Y*(DRef.thick/2.0),Y);
      ModelSupport::buildPlane(SMap,CI+2,DCent+Y*(DRef.thick/2.0),Y);
      ModelSupport::buildPlane(SMap,CI+11,DCent-Y*(DRef.innerThick/2.0),Y);
      ModelSupport::buildPlane(SMap,CI+12,DCent+Y*(DRef.innerThick/2.0),Y);
      // make inner blades
      int PI(CI);
      for(size_t index=0; index<DRef.getNPhase();index++)
	{
	  const double oA=DRef.getOpenAngle(index);
	  const double cA=DRef.getCloseAngle(index);
	  ModelSupport::buildPlaneRotAxis(SMap,PI+3,DCent,X,Y,oA);
	  ModelSupport::buildPlaneRotAxis(SMap,PI+4,DCent,X,Y,cA);
	  
	  PI+=10;
	}
      DCent+=Y*(DRef.thick+diskGap);
      CI+=500;
    }

  return;
}

void
DiskChopper::createObjects(Simulation& System)
  /*!
    Adds the chopper blades
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DiskChopper","createObjects");

  HeadRule HR;
  int CI(buildIndex);
  for(const DiskBlades& DRef : DInfo)
    {
      // Gap if there is previous item
      if (CI!=buildIndex)
	{
          // Inner :
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,CI-500,"12M -511M -7");
	  makeCell("Inner",System,cellIndex++,0,0.0,HR);
	  
          // Outer
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,CI-500,"2M -501M 7 -17");
	  makeCell("Outer",System,cellIndex++,0,0.0,HR);
	}
      
      // inner layer
      HR=ModelSupport::getHeadRule(SMap,buildIndex,CI,"11M -12M -7");
      makeCell("Inner",System,cellIndex++,DRef.getInnerMat(),0.0,HR);

      // Chopper opening
      const size_t NPhase=DRef.getNPhase();
      const HeadRule MainHR=
	ModelSupport::getHeadRule(SMap,buildIndex,CI,"1M -2M  7 -17");
      
      int PI(CI);       // current 
      int PN(CI+10);    // next
      for(size_t index=0;index<NPhase;index++)
	{
	  if (index==NPhase-1)
	    PN=CI;
	  // OPENING
	  double oA=DRef.getOpenAngle(index/2);
	  double cA=DRef.getCloseAngle(index/2);
	  if (cA-oA<180.0)
	    HR=ModelSupport::getHeadRule(SMap,PI,"3 -4");
	  else
	    HR=ModelSupport::getHeadRule(SMap,PI,"(3 : -4)");
	  makeCell("Outer",System,cellIndex++,0,0.0,HR*MainHR);
		  
	  // CLOSING
	  oA=DRef.getCloseAngle(index/2);
	  cA=DRef.getOpenAngle(1+(index/2));
	  if (cA-oA<180.0)
	    HR=ModelSupport::getHeadRule(SMap,PI,PN,"4 -3M");
	  else
	    HR=ModelSupport::getHeadRule(SMap,PI,PN,"(4 : -3M)");
	  makeCell("Outer",System,cellIndex++,
		   DRef.getOuterMat(),0.0,HR*MainHR);
	  PI+=10;
	  PN+=10;
	}

      if (DRef.innerThick-DRef.thick>Geometry::zeroTol)
        {
          HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 7 -17");
          makeCell("Outer",System,cellIndex++,0,0.0,HR);

          HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 7 -17");
	  makeCell("Outer",System,cellIndex++,0,0.0,HR);
        }

      CI+=500;
    }

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,CI-500,"11 -17 -12M");
  addOuterSurf(HR);      

  return;
}

void
DiskChopper::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("DiskChopper","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  mainFC.setConnect(0,Origin,-Y);
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  const int CLast(buildIndex+static_cast<int>(nDisk-1)*500);
  double L(-diskGap);  // one less gap !
  for(const DiskBlades& DRef : DInfo)
    L+=DRef.getThick()+diskGap;

  mainFC.setConnect(1,Origin+Y*L,Y);
  mainFC.setLinkSurf(1,SMap.realSurf(CLast+2));
  
  mainFC.setConnect(2,Origin+Y*(L/2.0)-X*outerRadius,-X);
  mainFC.setLinkSurf(2,SMap.realSurf(buildIndex+17));

  mainFC.setConnect(3,Origin+Y*(L/2.0)-Z*outerRadius,-Z);
  mainFC.setLinkSurf(3,SMap.realSurf(buildIndex+17));

  mainFC.setConnect(4,Origin+Y*(L/2.0)+Z*outerRadius,Z);
  mainFC.setLinkSurf(4,SMap.realSurf(buildIndex+17));

  mainFC.setConnect(5,Origin+Y*(L/2.0)+X*outerRadius,X);
  mainFC.setLinkSurf(5,SMap.realSurf(buildIndex+17));

  // MAIN BEAM:
  beamFC.setConnect(0,beamOrigin,-beamAxis);
  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  beamFC.setConnect(1,beamOrigin+Y*L,beamAxis);
  beamFC.setLinkSurf(1,SMap.realSurf(CLast+2));
  
  return;
}

void
DiskChopper::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("DiskChopper","createAll(FC)");
  
  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

void
DiskChopper::createAll(Simulation& System,
		       const attachSystem::FixedComp& rotFC,
		       const long int rotIndex,
		       const attachSystem::FixedComp& bFC,
		       const long int beamIndex)
                       
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param rotFC :: FixedComp for main rotor
    \param rotIndex :: Fixed Index for rotor
    \param bFC :: FixedComp for beam 
    \param beamIndex :: Fixed Index for beam
  */
{
  ELog::RegMethod RegA("DiskChopper","createAll(FC,FC)");
  
  populate(System.getDataBase());
  createUnitVector(rotFC,rotIndex,bFC,beamIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
