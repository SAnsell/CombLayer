/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/beamSlot.cxx
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "beamSlot.h"

namespace delftSystem
{

beamSlot::beamSlot(const std::string& Key,const int SN)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key+std::to_string(SN),6),
  attachSystem::ExternalCut(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param SN :: slot number  (id number)
  */
{}

beamSlot::beamSlot(const beamSlot& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedRotate(A),
  attachSystem::ExternalCut(A),
  baseName(A.baseName),xSize(A.xSize),
  zSize(A.zSize)
  /*!
    Copy constructor
    \param A :: beamSlot to copy
  */
{}

beamSlot&
beamSlot::operator=(const beamSlot& A)
  /*!
    Assignment operator
    \param A :: beamSlot to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      xSize=A.xSize;
      zSize=A.zSize;
    }
  return *this;
}


beamSlot::~beamSlot() 
 /*!
   Destructor
 */
{}

void
beamSlot::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable data base
 */
{
  ELog::RegMethod RegA("beamSlot","populate");
  FixedRotate::populate(Control);
  
  // First get inner widths:
  
  xSize=Control.EvalTail<double>(keyName,baseName,"XSize");
  zSize=Control.EvalTail<double>(keyName,baseName,"ZSize");

  endThick=Control.EvalTail<double>(keyName,baseName,"EndThick");
  divideThick=Control.EvalTail<double>(keyName,baseName,"DivideThick");

  NChannels=Control.EvalTail<size_t>(keyName,baseName,"NChannels");

  glassMat=ModelSupport::EvalMat<int>(Control,keyName+"GlassMat",
				      baseName+"GlassMat");

  return;
}
  

void
beamSlot::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces
    \param FC :: FixedComp for front/back
  */
{
  ELog::RegMethod RegA("beamSlot","createSurfaces");

  SMap.addMatch(buildIndex+1,FC.getLinkSurf(1));
  SMap.addMatch(buildIndex+2,FC.getLinkSurf(2));

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(xSize/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(xSize/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*zSize/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*zSize/2.0,Z);


  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(xSize/2.0-endThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
  			   Origin+X*(xSize/2.0-endThick),X);

  const double gap=(zSize-static_cast<double>(NChannels+1)*divideThick)/
    static_cast<double>(NChannels);
  double zPoint(-zSize/2.0);
  int surfOffset(buildIndex+10);
  for(size_t i=0;i<NChannels;i++)
    {
      // Glass: Air
      ModelSupport::buildPlane(SMap,surfOffset+5,
			     Origin+Z*(zPoint+divideThick),Z);
      // Air: Glass: 
      ModelSupport::buildPlane(SMap,surfOffset+15,
			     Origin+Z*(zPoint+divideThick+gap),Z);
      surfOffset+=20;
      zPoint+=divideThick+gap;
    }
    
  return;
}

void
beamSlot::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("beamSlot","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  addOuterSurf(Out);

  
  // End plates
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -13 5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,glassMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 14 -4 5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,glassMat,0.0,Out));

  int surfOffset(buildIndex);
  const std::string baseOut=
    ModelSupport::getComposite(SMap,buildIndex," 1 -2 13 -14 ");
  
  for(size_t i=0;i<NChannels;i++)
    {
      Out=baseOut+ModelSupport::getComposite(SMap,surfOffset," 5 -15 ");
      System.addCell(MonteCarlo::Object(cellIndex++,glassMat,0.0,Out));
      Out=baseOut+ModelSupport::getComposite(SMap,surfOffset," 15 -25 ");
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
      surfOffset+=20;
    }
  Out=baseOut+ModelSupport::getComposite(SMap,buildIndex,
					 surfOffset," 5M -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,glassMat,0.0,Out));
  
  return;
}

void
beamSlot::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("beamSlot","createLinks");

  FixedComp::setConnect(0,Origin,-Y); 
  FixedComp::setConnect(1,Origin,Y);  
  FixedComp::setConnect(2,Origin-X*xSize,X); 
  FixedComp::setConnect(3,Origin+X*xSize,X); 
  FixedComp::setConnect(4,Origin-Z*zSize/2.0,-Z); 
  FixedComp::setConnect(5,Origin+Z*zSize/2.0,Z); 

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  for(size_t i=2;i<6;i++)
    {
      const int sN(buildIndex+static_cast<int>(i+1));
      FixedComp::setLinkSurf(i,SMap.realSurf(sN));
    }

  return;
}

void
beamSlot::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add slot to
    \param FC :: BeamInsert Object
  */
{
  ELog::RegMethod RegA("beamSlot","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces(FC);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
