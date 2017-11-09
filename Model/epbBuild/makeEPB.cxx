/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   epbBuild/makeEPB.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <algorithm>
#include <iterator>
#include <memory>

#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "World.h"
#include "AttachSupport.h"

#include "Building.h"
#include "EPBline.h"
#include "Magnet.h"
#include "makeEPB.h"

namespace epbSystem
{

makeEPB::makeEPB() :
  Hall(new Building("Hall")),
  LineVoid(new EPBline("LineVoid"))
 /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeEPB","makeEPB");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(Hall);
  OR.addObject(LineVoid);
}


makeEPB::~makeEPB()
  /*!
    Destructor
  */
{}

void
makeEPB::makeMagnets(Simulation& System)
  /*!
    Build all the magnets 
    \param System :: Simulation to add to 
   */
{
  ELog::RegMethod RegA("makeEPB","makeMagnets");

  const FuncDataBase& Control=System.getDataBase();
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t nItems=Control.EvalVar<size_t>("EPBNMagnets");
  for(size_t i=0;i<nItems;i++)
    {
      std::shared_ptr<Magnet> GA(new Magnet("Magnet",i+1));
      GA->createAll(System,*LineVoid);
      attachSystem::addToInsertSurfCtrl(System,*Hall,*GA);
      OR.addObject(GA);
      BendMags.push_back(GA);
    }

  const size_t nFocus=Control.EvalVar<size_t>("EPBNFocus");
  for(size_t i=0;i<nFocus;i++)
    {
      std::shared_ptr<Magnet> GA(new Magnet("Focus",i+1));
      GA->createAll(System,*LineVoid);
      attachSystem::addToInsertSurfCtrl(System,*Hall,*GA);
      OR.addObject(GA);
      FocusMags.push_back(GA);
    }
  return;
}


void 
makeEPB::build(Simulation* SimPtr,
	       const mainSystem::inputParam&)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeEPB","build");

  int voidCell(74123);
  Hall->addInsertCell(voidCell);
  Hall->createAll(*SimPtr,World::masterOrigin());

  LineVoid->createAll(*SimPtr,World::masterOrigin());
  attachSystem::addToInsertSurfCtrl(*SimPtr,*Hall,*LineVoid);
  
  makeMagnets(*SimPtr);
  return;
}


}   // NAMESPACE ts1System

