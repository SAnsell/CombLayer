/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/sourceDataBase.cxx
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "support.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "stringCombine.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SourceBase.h"
#include "sourceDataBase.h"

namespace SDef
{

sourceDataBase::sourceDataBase() 
  /*!
    Constructor
  */
{}

sourceDataBase::~sourceDataBase() 
  /*!
    Destructor
  */
{}
  
sourceDataBase& 
sourceDataBase::Instance() 
  /*!
    Effective this object			
    \return sourceDataBase object
  */
{
  static sourceDataBase A;
  return A;
}

void
sourceDataBase::reset()
  /*!
    Delete all the references to the shared_ptr register
  */
{  
  Components.erase(Components.begin(),Components.end());
  return;
}

void
sourceDataBase::addSource(const std::string& Name,
			  const SourceBase& SObj)
  /*!
    Add a source and manage the memory
    \param Name :: Name of source
    \param SObj :: source Ptr
  */
{
  ELog::RegMethod RegA("sourceDataBase","addSource");
  
  SMAP::iterator mc=Components.find(Name);
  if (mc != Components.end())
    {
      if (mc->second.get()== &SObj) return;
      Components.erase(mc);
    }
  Components.emplace(Name,STYPE(SObj.clone()));
  return;
}



} // NAMESPACE SDef
