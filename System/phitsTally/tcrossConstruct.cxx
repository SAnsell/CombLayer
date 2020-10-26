/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/tcrossConstruct.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "support.h"
#include "stringCombine.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "inputParam.h"

#include "SimPHITS.h"
#include "particleConv.h"
#include "TallySelector.h"
#include "meshConstruct.h"
#include "MeshXYZ.h"
#include "eType.h"
#include "aType.h"
#include "phitsTally.h"
#include "TCross.h"
#include "tcrossConstruct.h" 

namespace phitsSystem
{

void 
tcrossConstruct::createTally(SimPHITS& System,
			     const int ID)
/*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimPHITS to add tallies
    \param ID :: output stream
  */
{
  ELog::RegMethod RegA("tcrossConstruct","createTally");

  TCross UB(ID);

  
  System.addTally(UB);

  return;
}

  
      

void
tcrossConstruct::processSurface(SimPHITS& System,
				const mainSystem::inputParam& IParam,
				const size_t Index) 
  /*!
    Add t-cross tally (s) as needed
    \param System :: SimPHITS to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("tcrossConstruct","processSurface");

  const int nextId=10; // System.getNextFTape();
  
  const std::string particleType=
    IParam.getValueError<std::string>("tally",Index,1,"tally:ParticleType");
  const std::string FCname=
    IParam.getValueError<std::string>("tally",Index,2,"tally:Object/Cell");
  const std::string FCindex=
    IParam.getValueError<std::string>("tally",Index,3,"tally:linkPt/Cell");

  size_t itemIndex(4);
  int cellA(0);
  int cellB(0);
  if (
      (!StrFunc::convert(FCname,cellA) ||
       !StrFunc::convert(FCindex,cellB) ||
       !checkLinkCells(System,cellA,cellB) ) &&
      !constructCellMapPair(System,FCname,FCindex,cellA,cellB) &&
      !constructLinkRegion(System,FCname,FCindex,cellA,cellB)
      )
    {
      throw ColErr::CommandError(FCname+" "+FCindex,"Surf Tally conversion");
    }
  ELog::EM<<"Regions connected from "<<cellA<<" to "<<cellB<<ELog::endDiag;  

  const double EA=IParam.getDefValue<double>(1e-9,"tally",Index,itemIndex++);
  const double EB=IParam.getDefValue<double>(1000.0,"tally",Index,itemIndex++);
  const size_t NE=IParam.getDefValue<size_t>(200,"tally",Index,itemIndex++); 

  const double AA=IParam.getDefValue<double>(0.0,"tally",Index,itemIndex++);
  const double AB=IParam.getDefValue<double>(2*M_PI,"tally",Index,itemIndex++);
  const size_t NA=IParam.getDefValue<size_t>(1,"tally",Index,itemIndex++);


  creatTally(System,ID);
  
  return;      
}
  
void
tcrossConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "-T surface \n"
    "  particleType : name of particle / energy to tally\n"
    "  Option A : \n"
    "       FixedComp + linkPoint (using % to mean -ve)\n"
    "  Option B : \n"
    "       FixedComp:CellMap:Index (index optional) \n"
    "  Emin EMax NE \n"
    "  AMin AngleMax NA (all optional) \n"
    <<std::enld;

  return;
}

}  // NAMESPACE phitsSystem
