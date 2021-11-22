/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeSurround.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BeSurround.h"

namespace delftSystem
{

BeSurround::BeSurround(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,3),
  attachSystem::ExternalCut(),
  active(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeSurround::BeSurround(const BeSurround& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::ExternalCut(A),
  active(A.active),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  length(A.length),mat(A.mat)
  /*!
    Copy constructor
    \param A :: BeSurround to copy
  */
{}

BeSurround&
BeSurround::operator=(const BeSurround& A)
  /*!
    Assignment operator
    \param A :: BeSurround to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      active=A.active;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}

BeSurround::~BeSurround() 
 /*!
   Destructor
 */
{}

void
BeSurround::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Data base to use
 */
{
  ELog::RegMethod RegA("BeSurround","populate");

  active=Control.EvalDefVar<int>(keyName+"Active",1);
  if (active)
    {
      FixedOffset::populate(Control);
      
      length=Control.EvalVar<double>(keyName+"Length");
      innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
      outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
      frontThick=Control.EvalDefVar<double>(keyName+"FrontThick",0.0);

      mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
      frontMat=ModelSupport::EvalDefMat(Control,keyName+"FrontMat",mat);
    }

  return;
}
  
void
BeSurround::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeSurround","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin,Y,outerRadius);

  // front surface
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*frontThick,Y);
    
  return;
}

void
BeSurround::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
    \param ExcludeString :: internal structure
  */
{
  ELog::RegMethod RegA("BeSurround","createObjects");

  const std::string ExcludeString=
    ExternalCut::getRuleStr("FlightCut");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -2 -17 7 ");
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+ExcludeString));
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 -17 ");
  System.addCell(MonteCarlo::Object(cellIndex++,frontMat,0.0,Out+ExcludeString));

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -17 (7:-11) ");
  addOuterSurf(Out);
    
  return;
}

void
BeSurround::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin+X*outerRadius+Y*length/2.0,X);

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));

  return;
}

void
BeSurround::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: link point for construction
    \param ExcludeStr :: Inner area
  */
{
  ELog::RegMethod RegA("BeSurround","createAll");

  populate(System.getDataBase());
  if (!active) return;

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE delftSystem
