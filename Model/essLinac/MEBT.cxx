/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essLinac/MEBT.cxx
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#include <memory>
#include <numeric>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "CopiedComp.h"
#include "AttachSupport.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "MEBT.h"

#include "VacuumPipe.h"
#include "MEBTQuad.h"
#include "MagRoundVacuumPipe.h"


namespace essSystem
{

MEBT::MEBT(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MEBT::MEBT(const MEBT& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  baseName(A.baseName),
  vacPipe(A.vacPipe),
  mebtquad(A.mebtquad),
  magRoundVacuumPipe(A.magRoundVacuumPipe)
  /*!
    Copy constructor
    \param A :: MEBT to copy
  */
{}

MEBT&
MEBT::operator=(const MEBT& A)
  /*!
    Assignment operator
    \param A :: MEBT to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      vacPipe=A.vacPipe;
      mebtquad=A.mebtquad;
      magRoundVacuumPipe=A.magRoundVacuumPipe;
    }
  return *this;
}

MEBT*
MEBT::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new MEBT(*this);
}
  
MEBT::~MEBT() 
  /*!
    Destructor
  */
{}

void
MEBT::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MEBT","populate");

  FixedOffset::populate(Control);

  return;
}

void
MEBT::createLinks()
/*!
  Construct all the links
 */
{
  const int N = std::accumulate
    (vacPipe.begin(),vacPipe.end(),0,
     [](size_t total,std::shared_ptr<constructSystem::VacuumPipe> d)
     {
       return total+d->NConnect();
     });
  FixedComp::setNConnect(static_cast<size_t>(N));

  size_t i(0);
  for (const std::shared_ptr<constructSystem::VacuumPipe> d: vacPipe)
    {
      for (size_t j=0; j<d->NConnect(); j++)
	setUSLinkCopy(i++, *d, j);
    }
  return;
}
  
void
MEBT::createAll(Simulation& System,
		const attachSystem::FixedComp& FC,
		const long int sideIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MEBT","createAll");

  populate(System.getDataBase());

  //if (nDTL<1)
  //  return;

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  HeadRule HR;
  
  int counter = 0;
  
  std::shared_ptr<constructSystem::VacuumPipe> d
    (new constructSystem::VacuumPipe(baseName+"MEBTSection0"));
  OR.addObject(d);
  d->createAll(System, FC, sideIndex);
  HR=d->getFullRule("#front")*
    d->getFullRule("#pipeRadius")*
    d->getFullRule("#back");
  vacPipe.push_back(d);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p(new MagRoundVacuumPipe
					(baseName,"MEBTVacuumPipe",0));
  OR.addObject(p);
  p->addInsertCell(d->getCells("Void"));
  p->createAll(System, FC, sideIndex);
  magRoundVacuumPipe.push_back(p);

  counter++;

  for(int i=1;i<2;i++){

    std::shared_ptr<constructSystem::VacuumPipe> d1
      (new constructSystem::VacuumPipe(baseName+"MEBTSection"+std::to_string(i)));
    OR.addObject(d1);
    
    d1->createAll(System, *vacPipe[counter-1], 2);
    HR=d1->getFullRule("#front")*
      d1->getFullRule("#pipeRadius")*
      d1->getFullRule("#back");
    vacPipe.push_back(d1);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<MEBTQuad> q1(new MEBTQuad(baseName,"MEBTQuad",i));
    OR.addObject(q1);
    q1->addInsertCell(d1->getCells("Void"));
    q1->createAll(System, *vacPipe[counter-1], 2);
    mebtquad.push_back(q1);
    counter++;
    // ----
  }

  createLinks();

  return;
}

}  // essSystem
