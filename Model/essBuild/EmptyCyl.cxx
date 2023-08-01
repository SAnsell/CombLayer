/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/EmptyCyl.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "EmptyCyl.h"

namespace essSystem
{

EmptyCyl::EmptyCyl(const std::string& Key)  :
  attachSystem::FixedRotateUnit(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

EmptyCyl::EmptyCyl(const EmptyCyl& A) : 
  attachSystem::FixedRotateUnit(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  height(A.height),mat(A.mat)
  /*!
    Copy constructor
    \param A :: EmptyCyl to copy
  */
{}

EmptyCyl&
EmptyCyl::operator=(const EmptyCyl& A)
  /*!
    Assignment operator
    \param A :: EmptyCyl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
	  
      height=A.height;
      mat=A.mat;
    }
  return *this;
}

EmptyCyl*
EmptyCyl::clone() const
/*!
  Clone self
  \return new (this)
 */
{
  return new EmptyCyl(*this);
}
  
EmptyCyl::~EmptyCyl() 
  /*!
    Destructor
  */
{}

void
EmptyCyl::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("EmptyCyl","populate");

  FixedRotate::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
 
  return;
}
  
  
void
EmptyCyl::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EmptyCyl","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin+Y*(height),Y);

  // This is insane -- + 1cm in z direction?
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z,Z);

  return;
}
  
void
EmptyCyl::createObjects(Simulation& System,const attachSystem::FixedComp& FC,
			const long int floor,
			const long int side,
			const long int inner,
			const attachSystem::FixedComp& BC,
			const long int bulk,
			const attachSystem::FixedComp& GB1,
			const long int gb1lp,
			const attachSystem::FixedComp& GB2,
			const long int gb2lp)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC     :: FC to connect to
    \param floor  :: bottom link point
    \param side   :: outer side link point
    \param inner  :: shaft 1st step to exclude
    \param BC     :: Bulk/Twister component
    \param bulk   :: bulk/twister lp to exclude
    \param GB1     :: GuideBay1 object
    \param gb1lp   :: GB1 link point
    \param GB2     :: GuideBay2 object
    \param gb2lp   :: GB2 link point
  */
{
  ELog::RegMethod RegA("EmptyCyl","createObjects");

  HeadRule HR;

  if (Origin[2]>Geometry::zeroTol)
    ELog::EM << "Target inner lp not needed. "
      "Now this cylinder cuts EmptyCyl, but I have to increase the "
      "angle of triangle (adjust GuideBay)" << ELog::endDiag;

  const HeadRule topBotHR(HeadRule(SMap,buildIndex,-5)*FC.getFullRule(floor));
  const HeadRule commonHR(topBotHR*BC.getFullRule(bulk)*
			  FC.getMainRule(inner));

  // main clearance
  HR=GB1.getFullRule(gb1lp)+GB1.getFullRule(gb2lp);
  HR*=commonHR*FC.getMainRule(-side);  
  System.addCell(cellIndex++,mat,0.0,HR);

  if (Origin[2]<Geometry::zeroTol)
    {
      // triangle cut below target
      HR=commonHR*HeadRule(SMap,buildIndex,-6)*
	GB1.getFullRule(-gb1lp)*GB2.getFullRule(-gb2lp);
	
      System.addCell(cellIndex++,mat,0.0,HR);

      HR=GB1.getFullRule(gb1lp)+GB2.getFullRule(gb2lp)+
	HeadRule(SMap,buildIndex,-6);
      HR*=commonHR*FC.getMainRule(-side);
    }
  addOuterSurf(HR);
  return;
}

  
void
EmptyCyl::createLinks(const attachSystem::FixedComp&FC,
		      const long int floor,
		      const long int side)
  /*!
    Create all the links
    \param FC :: Line piont
  */
{
  ELog::RegMethod RegA("EmptyCyl","createLinks");

  FixedComp::setLinkCopy(0,FC,floor); // ??? how to invert surf sign?
  FixedComp::setLinkCopy(1,FC,side);
  FixedComp::setLinkCopy(2,FC,side+1);
  
  FixedComp::setConnect(3,Origin+Y*(height),Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+6));

  // for (int i=0; i<4; i++)
  //   ELog::EM << getLinkPt(i+1) << ":\t" << getLinkString(i+1) << ELog::endDiag;
  
  return;
}
  
void
EmptyCyl::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int floor,
		    const long int side,
		    const long int inner,
		    const attachSystem::FixedComp& BC,
		    const long int bulk,
		    const attachSystem::FixedComp& GB1,
		    const long int gb1lp,
		    const attachSystem::FixedComp& GB2,
		    const long int gb2lp)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param floor :: link point for origin (botom surf)
    \param side :: outer side link point
    \param inner :: shaft 1st step to exclude
    \param BC :: Bulk/Twister component
    \param bulk   :: bulk/twister lp to exclude
    \param GB1     :: GuideBay1 object
    \param gb1lp   :: GB1 link point
    \param GB2     :: GuideBay2 object
    \param gb2lp   :: GB2 link point
  */
{
  ELog::RegMethod RegA("EmptyCyl","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,floor);
  createSurfaces();
  ELog::EM<<"EARLY RETURN "<<ELog::endCrit;
  return;

  createObjects(System,FC,floor,side,inner,
		BC,bulk,
		GB1,gb1lp,GB2,gb2lp);
  createLinks(FC,floor,side);
  insertObjects(System);              

  return;
}

}  // essSystem
