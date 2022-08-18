/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/CouplePipe.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "VacVessel.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "CouplePipe.h"


namespace moderatorSystem
{

CouplePipe::CouplePipe(const std::string& Key)  :
  attachSystem::FixedUnit(Key,0),
  GOuter("gOuter"),HInner("hInner")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CouplePipe::CouplePipe(const CouplePipe& A) : 
  attachSystem::FixedUnit(A),
  GOuter(A.GOuter),HInner(A.HInner),
  Xoffset(A.Xoffset),Yoffset(A.Yoffset),outMat(A.outMat),
  outAlMat(A.outAlMat),outVacMat(A.outVacMat),innerAlMat(A.innerAlMat),
  innerMat(A.innerMat),outRadius(A.outRadius),outAlRadius(A.outAlRadius),
  outVacRadius(A.outVacRadius),innerAlRadius(A.innerAlRadius),
  innerRadius(A.innerRadius),fullLen(A.fullLen),hydMat(A.hydMat),
  hydWallMat(A.hydWallMat),hydTemp(A.hydTemp),hXoffset(A.hXoffset),
  hYoffset(A.hYoffset),hLen(A.hLen),hRadius(A.hRadius),hThick(A.hThick)
  /*!
    Copy constructor
    \param A :: CouplePipe to copy
  */
{}

CouplePipe&
CouplePipe::operator=(const CouplePipe& A)
  /*!
    Assignment operator
    \param A :: CouplePipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      GOuter=A.GOuter;
      HInner=A.HInner;
      Xoffset=A.Xoffset;
      Yoffset=A.Yoffset;
      outMat=A.outMat;
      outAlMat=A.outAlMat;
      outVacMat=A.outVacMat;
      innerAlMat=A.innerAlMat;
      innerMat=A.innerMat;
      outRadius=A.outRadius;
      outAlRadius=A.outAlRadius;
      outVacRadius=A.outVacRadius;
      innerAlRadius=A.innerAlRadius;
      innerRadius=A.innerRadius;
      fullLen=A.fullLen;
      hydMat=A.hydMat;
      hydWallMat=A.hydWallMat;
      hydTemp=A.hydTemp;
      hXoffset=A.hXoffset;
      hYoffset=A.hYoffset;
      hLen=A.hLen;
      hRadius=A.hRadius;
      hThick=A.hThick;
    }
  return *this;
}

CouplePipe::~CouplePipe() 
  /*!
    Destructor
  */
{}

void
CouplePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Simulation to use
  */
{
  ELog::RegMethod RegA("CouplePipe","populate");
  
  hydTemp=Control.EvalVar<double>("hydrogenModTemp");
  hydMat=ModelSupport::EvalMat<int>(Control,"hydrogenModMat");
  hydWallMat=ModelSupport::EvalMat<int>(Control,"hydrogenAlMat");

  Xoffset=Control.EvalVar<double>(keyName+"XOffset"); 
  Yoffset=Control.EvalVar<double>(keyName+"YOffset"); 
  
  fullLen=Control.EvalVar<double>(keyName+"FullLen");
 
  outRadius=Control.EvalVar<double>(keyName+"OutRad"); 
  outAlRadius=Control.EvalVar<double>(keyName+"OutAlRad"); 
  outVacRadius=Control.EvalVar<double>(keyName+"OutVacRad"); 
  innerAlRadius=Control.EvalVar<double>(keyName+"InnerAlRad"); 
  innerRadius=Control.EvalVar<double>(keyName+"InnerRad"); 

  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat"); 
  outAlMat=ModelSupport::EvalMat<int>(Control,keyName+"OutAlMat"); 
  outVacMat=ModelSupport::EvalMat<int>(Control,keyName+"OutVacMat"); 
  innerAlMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerAlMat"); 
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat"); 

  hXoffset=Control.EvalVar<double>(keyName+"HydXOff"); 
  hYoffset=Control.EvalVar<double>(keyName+"HydYOff"); 
  hLen=Control.EvalVar<double>(keyName+"HydLen"); 
  hRadius=Control.EvalVar<double>(keyName+"HydRad"); 
  hThick=Control.EvalVar<double>(keyName+"HydThick"); 
  
  return;
}
  

void
CouplePipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			     const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards NIMROD
    - X Across the moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("CouplePipe","createUnitVector");

  FixedComp::createUnitVector(CUnit);
  const attachSystem::LinkUnit& LU=CUnit.getLU(sideIndex);
  Origin=LU.getConnectPt();

  return;
}

void 
CouplePipe::insertOuter(Simulation& System,
                        const VacVessel& VC)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param VC :: Vacuum vessel
  */
{
  ELog::RegMethod RegA("CouplePipe","insertOuter");

  const long int activeSide(5);
  // Inner Points
  GOuter.addPoint(VC.getSurfacePoint(0,activeSide)+X*Xoffset+Y*Yoffset);
  GOuter.addPoint(VC.getSurfacePoint(1,activeSide)+X*Xoffset+Y*Yoffset);
  GOuter.addPoint(VC.getSurfacePoint(2,activeSide)+X*Xoffset+Y*Yoffset);
  GOuter.addPoint(VC.getSurfacePoint(3,activeSide)+X*Xoffset+Y*Yoffset);
  GOuter.addPoint(VC.getSurfacePoint(4,activeSide)+
                  X*Xoffset+Y*Yoffset-Z*fullLen);
  GOuter.setActive(0,3);
  GOuter.setActive(1,7);
  GOuter.setActive(2,31);

  GOuter.addRadius(innerRadius,innerMat,0.0);
  GOuter.addRadius(innerAlRadius,innerAlMat,0.0);
  GOuter.addRadius(outVacRadius,outVacMat,0.0);
  GOuter.addRadius(outAlRadius,outAlMat,0.0);
  GOuter.addRadius(outRadius,outMat,0.0);
 
  GOuter.build(System);
  return;
}

void
CouplePipe::insertPipes(Simulation& System,const VacVessel& VC)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param VC :: Vacuum vessel
  */
{
  ELog::RegMethod RegA("CouplePipe","insertPipes");

  // Hydrogen inner
  HInner.addPoint(VC.getSurfacePoint(3,5)+
		  X*(Xoffset+hXoffset)+
		  Y*(Yoffset+hYoffset)-Z*fullLen);
  // added
  HInner.addPoint(VC.getSurfacePoint(3,5)+
		  X*(Xoffset+hXoffset)+
		  Y*(Yoffset+hYoffset)+Z*hLen);
  

  //  HInner.addPoint(Origin+X*hXoffset+Y*hYoffset);
  //  HInner.addPoint(Origin+X*hXoffset+Y*hYoffset+Z*hLen);

  HInner.addRadius(hRadius-hThick,hydMat,hydTemp);
  HInner.addRadius(hRadius,innerAlMat,hydTemp);
 
  HInner.build(System);
  return;
}

  
void
CouplePipe::build(Simulation& System,
		      const attachSystem::FixedComp& FUnit,
		      const size_t sideIndex,
		      const VacVessel& VCell)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: FixUnit
    \param VCell :: Vacuum vessel
  */
{
  ELog::RegMethod RegA("CouplePipe","createAll");


  populate(System.getDataBase());

  createUnitVector(FUnit,sideIndex);
  insertOuter(System,VCell);
  
  insertPipes(System,VCell);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
