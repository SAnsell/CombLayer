/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/heatConstruct.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "inputParam.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"

#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "heatConstruct.h" 

namespace tallySystem
{

heatConstruct::heatConstruct() 
  /// Constructor
{}

heatConstruct::heatConstruct(const heatConstruct&) 
  /// Copy Constructor
{}

heatConstruct&
heatConstruct::operator=(const heatConstruct&) 
  /// Assignment operator
{
  return *this;
}

void
heatConstruct::processHeat(Simulation& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index) const
  /*!
    Add heat tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("heatConstruct","processHeat");

  const size_t NItems=IParam.itemCnt("tally",Index);
  ELog::EM<<"NItem == "<<NItems<<ELog::endDiag;
  if (NItems<3)
    throw ColErr::IndexError<size_t>(NItems,3,
				     "Insufficient items for tally");

  const std::string pType(IParam.getValue<std::string>("tally",Index,1)); 
  const std::string MType(IParam.getValue<std::string>("tally",Index,2)); 
  
  // Process a Ranged Heat:

  boost::format Cmt("tally: %d Mat %d Range(%d,%d)");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  // Get Material number:
  int matN(0);
  if (DB.hasKey(MType))
    matN=DB.getIndex(MType);
  else if (!StrFunc::convert(MType,matN))
    {
      ELog::EM<<"No material number for :"<<MType<<ELog::endErr;
      return;
    }

  if (matN<0) matN=-2;
    
  int RA,RB;
  // if flag true : then valid range
  const bool flag=
    convertRegion(IParam,"tally",Index,3,RA,RB);
  if (flag)
    {
      const int nTally=System.nextTallyNum(6);
      const std::vector<int> cells=getCellSelection(System,matN,RA,RB);
      
      
      tallySystem::addF6Tally(System,nTally,pType,cells);
      tallySystem::Tally* TX=System.getTally(nTally); 
      TX->setPrintField("e f");
      const std::string Comment=(Cmt % nTally % matN % RA % RB ).str();
      TX->setComment(Comment);
    }
  return;
}


void
heatConstruct::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<"Heat tally options:\n"
    "Particle MatN range ANumber BNumber\n"
    "Particle MatN cellRange";
  return;
}
  
  
}  // NAMESPACE tallySystem
