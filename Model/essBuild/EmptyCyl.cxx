/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/EmptyCyl.cxx
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "EmptyCyl.h"

namespace essSystem
{

EmptyCyl::EmptyCyl(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

EmptyCyl::EmptyCyl(const EmptyCyl& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
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
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
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

  FixedOffset::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
 
  return;
}
  
void
EmptyCyl::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("EmptyCyl","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
EmptyCyl::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EmptyCyl","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Y*(height),Y);

  return;
}
  
void
EmptyCyl::createObjects(Simulation& System,const attachSystem::FixedComp& FC,
			const long int floor,const long int side,
			const long int inner,
			const attachSystem::FixedComp& BC,
			const long int bulk)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC     :: FC to connect to
    \param floor  :: bottom link point
    \param side   :: outer side link point
    \param inner  :: shaft 1st step to exclude
    \param BC     :: Bulk/Twister component
    \param bulk   :: bulk/twister lp to exclude
  */
{
  ELog::RegMethod RegA("EmptyCyl","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,surfIndex," -6 ");
  Out += std::to_string(FC.getLinkSurf(-side)) + " " +
    std::to_string(FC.getLinkSurf(floor)) + " " +
    std::to_string(FC.getLinkSurf(inner)) + " " +
    BC.getLinkString(bulk);

  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  addOuterSurf(Out);

  return;
}

  
void
EmptyCyl::createLinks(const attachSystem::FixedComp&FC,
		      const long int floor,
		      const long int side)
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("EmptyCyl","createLinks");

  FixedComp::setLinkCopy(0,FC,static_cast<size_t>(floor-1)); // ??? how to invert surf sign?
  FixedComp::setLinkCopy(1,FC,static_cast<size_t>(side-1));
  FixedComp::setLinkCopy(2,FC,static_cast<size_t>(side));
  
  FixedComp::setConnect(3,Origin+Y*(height),Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+6));

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
		    const long int bulk)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param floor :: link point for origin (botom surf)
    \param side :: outer side link point
    \param inner :: shaft 1st step to exclude
    \param BC :: Bulk/Twister component
    \param bulk   :: bulk/twister lp to exclude
  */
{
  ELog::RegMethod RegA("EmptyCyl","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,floor);
  createSurfaces();
  createObjects(System,FC,floor,side,inner,BC,bulk);
  createLinks(FC,floor,side);
  insertObjects(System);              

  return;
}

}  // essSystem
