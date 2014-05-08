/******************************************************************** 
  CombLayer : MNCPX Input builder
 
 * File:   build/reactorTallyConstruct.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
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
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "Simulation.h"
#include "inputParam.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "fissionConstruct.h" 
#include "reactorTallyConstruct.h" 


namespace tallySystem
{

reactorTallyConstruct::reactorTallyConstruct() :
  fissionConstruct()
  /// Constructor
{}

reactorTallyConstruct::reactorTallyConstruct(const reactorTallyConstruct& A) :
  fissionConstruct(A)
  /*!
    Copy Constructor
  */
{}

reactorTallyConstruct&
reactorTallyConstruct::operator=(const reactorTallyConstruct& A) 
  /// Assignment operator
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
	 const size_t Index,const bool renumberFlag) const
/*!
    Add point tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: Index of tally
    \param renumberFlag :: Renubmer call
   */
{
  ELog::RegMethod RegA("reactorTallyConstruct","processPower");

  const ModelSupport::objectRegister& OR= 
    ModelSupport::objectRegister::Instance();

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");

  const std::string PType(IParam.getCompValue<std::string>("tally",Index,1)); 

  if (PType=="help")  // 
    {
      ELog::EM<<"-- Reactor Name"<<ELog::endDiag;
      return 0;
    }
  const delftSystem::ReactorGrid* GPtr=
    dynamic_cast<const delftSystem::ReactorGrid*>
      (OR.getObject<attachSystem::FixedComp>(PType));

  if (!GPtr)
    throw ColErr::InContainerError<std::string>(PType,"ReactorGrid object");

  const int nTally=System.nextTallyNum(7);
  tallySystem::addF7Tally(System,nTally,GPtr->getAllCells(System));
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  boost::format Cmt("tally: %d Cell %s ");
  const std::string Comment=(Cmt % nTally % "ReactorGrid" ).str();
  TX->setComment(Comment);

  
  return 0;
}

}  // NAMESPACE tallySystem
