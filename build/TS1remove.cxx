/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/TS1remove.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "RemoveCell.h"
#include "TS1remove.h"

TS1remove::TS1remove() : RemoveCell()
 /*! 
   Default constructor
 */		       
{}

TS1remove::TS1remove(FuncDataBase& FDB) :
  RemoveCell(FDB)
  /*!
    Constructor
    \param FDB :: Reference function database
  */
{
  populateZero();
}

TS1remove::TS1remove(const TS1remove& A) : 
  RemoveCell(A)
  /*!
    Copy constructor 
    \param A :: object to copy
  */
{}

TS1remove&
TS1remove::operator=(const TS1remove& A) 
  /*!
    Assignement operator
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RemoveCell::operator=(A);
    }
  return *this;
}

void
TS1remove::populateZero()
  /*!
    Populate ZerCellComp list
    IF value is zero then remove cell
  */
{
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(4111,"methPoison"));         // methane poison
  RemoveCell::ZeroCellComp.insert(ZCCType::value_type(8103,"watPoisDepth"));         // methane poison
  return;
}


bool
TS1remove::toBeRemoved(const int ICN,MonteCarlo::Qhull&)
  /*!
    Function to used QHull and cell to determine if
    a cell is valid
    \param ICN :: Cell number (from MCNPX file)
    \return true if the cell should be removed
  */
{
  if (!RemoveCell::ConVar)
    {
      ELog::EM.error("RemoveCell: not initialized");
      exit(1);
    }
  
  ZCCType::const_iterator vc;
  vc=RemoveCell::ZeroCellComp.find(ICN);
  while (vc!=RemoveCell::ZeroCellComp.end() && vc->first==ICN)
    {
      if (RemoveCell::ConVar->EvalVar<double>(vc->second)<=0.0)
	return 1;
      vc++;
    }

  return 0;
}
