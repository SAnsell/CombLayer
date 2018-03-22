/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/flukaPhysics.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>
#include <tuple>


#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "MapRange.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "particleConv.h"
#include "cellValueSet.h"
#include "flukaPhysics.h"

namespace flukaSystem
{
		       
flukaPhysics::flukaPhysics() :
  impValue({
      { "all",      cellValueSet("all","BIAS",0.0) },
      { "hadron",   cellValueSet("hadron","BIAS",1.0) },
      { "electron", cellValueSet("electron","BIAS",2.0) },
      { "low",      cellValueSet("low","BIAS",3.0) }
    })
  /*!
    Constructor
  */
{}

flukaPhysics::flukaPhysics(const flukaPhysics& A) : 
  cellN(A.cellN),impValue(A.impValue)
  /*!
    Copy constructor
    \param A :: flukaPhysics to copy
  */
{}

flukaPhysics&
flukaPhysics::operator=(const flukaPhysics& A)
  /*!
    Assignment operator
    \param A :: flukaPhysics to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellN=A.cellN;
      impValue=A.impValue;
    }
  return *this;
}



flukaPhysics::~flukaPhysics()
  /*!
    Destructor
  */
{}

  
void
flukaPhysics::clearAll()
  /*!
    The big reset
  */
{
  cellN.clear();
  for(std::map<std::string,cellValueSet>::value_type& mc : impValue)
    mc.second.clearAll();
  
  return;
}


void
flukaPhysics::setCellNumbers(const std::vector<int>& cellInfo)
  /*!
    Process the list of the valid cells 
    over each importance group.
    \param cellInfo :: list of cells
  */

{
  ELog::RegMethod RegA("flukaPhysics","setCellNumbers");

  cellN=cellInfo;
  return;
}
  
void
flukaPhysics::setImp(const std::string& keyName,
		     const int cellN,
		     const double value)
  /*!
    Set the importance list
    \param keyName :: all/hadron/electron/low
    \param cellN :: Cell number
    \param value :: Value to use
  */
{
  ELog::RegMethod RegA("flukaPhysics","setImp");
  
  std::map<std::string,cellValueSet>::iterator mc=
    impValue.find(keyName);
  if (mc==impValue.end())
    throw ColErr::InContainerError<std::string>(keyName,"impValue");

  mc->second.setValue(cellN,value);
  return;
}

void
flukaPhysics::writeFLUKA(std::ostream& OX) const
  /*!
    Write out all the FLUKA physics
    \param OX :: Output stream
 */
{
  ELog::EM<<"Cn == "<<cellN.size()<<ELog::endDiag;
  for(const std::map<std::string,cellValueSet>::value_type& mc : impValue)
    mc.second.writeFLUKA(OX,cellN," %0 1.0 %3 %1 %2 1.0 ");
  ELog::EM<<"Finish "<<ELog::endDiag;
  return;
}

  
} // NAMESPACE flukaSystem
      
   
