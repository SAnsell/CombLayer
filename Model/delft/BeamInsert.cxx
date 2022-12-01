/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeamInsert.cxx
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "FixedRotate.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "beamSlot.h"
#include "BeamInsert.h"

namespace delftSystem
{

BeamInsert::BeamInsert(const std::string& Key)  :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamInsert::BeamInsert(const BeamInsert& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  length(A.length),radius(A.radius),
  nSlots(A.nSlots),Holes(A.Holes),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: BeamInsert to copy
  */
{}

BeamInsert&
BeamInsert::operator=(const BeamInsert& A)
  /*!
    Assignment operator
    \param A :: BeamInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      length=A.length;
      radius=A.radius;
      nSlots=A.nSlots;
      Holes=A.Holes;
      mat=A.mat;
    }
  return *this;
}


BeamInsert::~BeamInsert() 
 /*!
   Destructor
 */
{}

void
BeamInsert::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: FuncDataBase to use
 */
{
  ELog::RegMethod RegA("BeamInsert","populate");
  
  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  nSlots=Control.EvalVar<size_t>(keyName+"NSlots");

  // Create Holes
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::string::size_type pos=keyName.find("Insert");
  const std::string BIStr(keyName.substr(0,pos));
  for(size_t i=0;i<nSlots;i++)
    {
      Holes.push_back(std::shared_ptr<beamSlot>
		      (new beamSlot(BIStr+"Slot",static_cast<int>(i+1))));
      OR.addObject(BIStr+"Slot"+std::to_string(i+1),Holes.back());
    }
  
  return;
}
  
void
BeamInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamInsert","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin,Y,radius);

  return;
}

void
BeamInsert::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeamInsert","createObjects");
  
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  System.addCell(cellIndex++,mat,0.0,HR);

  addOuterSurf(HR);
    
  return;
}

void
BeamInsert::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin+X*radius,X);

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));

  return;
}

void
BeamInsert::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: link point [signed]
  */
{
  ELog::RegMethod RegA("BeamInsert","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  for(size_t i=0;i<nSlots;i++)
    {
      Holes[i]->addInsertCell(cellIndex-1);
      Holes[i]->createAll(System,*this,0);
    }
  return;
}

  
}  // NAMESPACE delftSystem
