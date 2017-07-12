/******************************************************************** 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/reactorTallyConstruct.cxx
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
#include <boost/multi_array.hpp>
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
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "Simulation.h"
#include "inputParam.h"
#include "objectSupport.h"
#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "fissionConstruct.h" 
#include "reactorTallyConstruct.h" 


namespace tallySystem
{

reactorTallyConstruct::reactorTallyConstruct() :
  fissionConstruct()
  /*!
    Constructor
  */
{}

reactorTallyConstruct::reactorTallyConstruct(const reactorTallyConstruct& A) :
  fissionConstruct(A)
  /*!
    Copy Constructor
    \param A :: reactorTallyConstructor to copy
  */
{}

reactorTallyConstruct&
reactorTallyConstruct::operator=(const reactorTallyConstruct& A) 
  /*!
    Assignment operator
    \param A :: Tally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      fissionConstruct::operator=(A);
    }
  return *this;
}

int
reactorTallyConstruct::processPower
        (Simulation& System,const mainSystem::inputParam& IParam,
	 const size_t Index) const
  /*!
    Add point tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: Index of tally
    \return 0 on success
   */
{
  ELog::RegMethod RegA("reactorTallyConstruct","processPower");

  const ModelSupport::objectRegister& OR= 
    ModelSupport::objectRegister::Instance();

				     
  const std::string PType=IParam.getValueError<std::string>
    ("tally",Index,1,"Insufficient items for tally");
  
  const delftSystem::ReactorGrid* GPtr=
    dynamic_cast<const delftSystem::ReactorGrid*>
    (OR.getObject<attachSystem::FixedComp>(PType));
  
  if (!GPtr)
    throw ColErr::InContainerError<std::string>
      (PType,"ReactorGrid type not found");
  
  const int nTally=System.nextTallyNum(7);
  const std::vector<int> allCells(GPtr->getAllCells(System));
  const std::vector<int> nonVoidCells=
    objectSupport::cellSelection(System,"AllNonVoid",allCells);
  tallySystem::addF7Tally(System,nTally,nonVoidCells);

  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  boost::format Cmt("tally: %d Cell %s ");
  const std::string Comment=(Cmt % nTally % "ReactorGrid" ).str();
  TX->setComment(Comment);

  return 0;
}

void
reactorTallyConstruct::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<"-- Reactor Name";
  return;
}

  
}  // NAMESPACE tallySystem
