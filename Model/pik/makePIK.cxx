/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   pik/makePIK.cxx
  *
  * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <array>
#include <algorithm>
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "inputParam.h"
#include "objectRegister.h"
#include "World.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FixedRotate.h"

#include "PIKPool.h"
#include "PIKReflector.h"
#include "PIKCore.h"
#include "makePIK.h"

namespace pikSystem
{

  makePIK::makePIK() :
    pool(new pikSystem::PIKPool("Pool")),
    refl(new pikSystem::PIKReflector("Reflector")),
    core(new pikSystem::PIKCore("Core"))
    /*!
      Constructor
    */
  {
    ModelSupport::objectRegister& OR=
      ModelSupport::objectRegister::Instance();

    OR.addObject(pool);
    OR.addObject(refl);
    OR.addObject(core);
  }

  makePIK::makePIK(const makePIK&A) :
    pool(new pikSystem::PIKPool(*A.pool)),
    refl(new pikSystem::PIKReflector(*A.refl)),
    core(new pikSystem::PIKCore(*A.core))
    /*!
      Copy constructor
      \param A :: makePIK object to copy
     */
  {
  }

  makePIK&
  makePIK::operator=(const makePIK& A)
  /*!
    Assignment operator
    \param A :: makePIK object to assign
    \return *this
   */
  {
    if (this!=&A)
      {
	*pool = *A.pool;
	*refl = *A.refl;
	*core = *A.core;
      }
    return *this;
  }

  makePIK::~makePIK()
  /*!
    Destructor
  */
  {}

  bool
  makePIK::buildFuelElements(Simulation& System,
			     const mainSystem::inputParam& IParam)
  /*!
    Builds fuel elements
    \param System :: Simulation system
    \param IParam :: Input parameters
  */
  {
    ELog::RegMethod RegA("makePIK","buildFuelElements");

    return true;
  }

  void
  makePIK::build(Simulation& System,
		 const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
  */
  {
    ELog::RegMethod RegA("makePIK","build");

    constexpr int voidCell(74123);
    const attachSystem::FixedComp& World=World::masterOrigin();

    pool->addInsertCell(voidCell);
    pool->createAll(System,World,0);

    refl->setSide(*pool,1);
    refl->setBottom(*pool,2);
    refl->setTop(*pool,3);
    refl->addInsertCell(pool->getCell("InnerShieldCentral"));
    refl->createAll(System,*pool,0);

    core->addInsertCell(pool->getCell("InnerShieldCentral"));
    core->addInsertCell(refl->getCell("MainCell"));
    core->createAll(System,*pool,0);


    //  const FuncDataBase& Control=System.getDataBase();

    if (buildFuelElements(System,IParam))
      ELog::EM<<"=Finished fuel elements="<<ELog::endDiag;
    // else if(buildR1Ring(System,IParam))
    //   ELog::EM<<"Finished 1.5GeV Ring"<<ELog::endDiag;
    // else
    //   ELog::EM<<"NO Linac/ SPF / R1 / R3 Ring built"<<ELog::endCrit;

    return;
  }


}   // NAMESPACE pikSystem
