/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/RemoveCell.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "RemoveCell.h"


RemoveCell::RemoveCell() :
  ConVar(0)
  /*!
    Constructor
  */
{}

RemoveCell::RemoveCell(FuncDataBase& FDB) :
  ConVar(&FDB)
  /*!
    Constructor
    \param FDB :: Reference function database
  */
{}

RemoveCell::RemoveCell(const RemoveCell& A) :
  ZeroCellComp(A.ZeroCellComp),ConVar(A.ConVar)
  /*!
    Copy constructor 
    \param A :: object to copy
  */
{}

RemoveCell&
RemoveCell::operator=(const RemoveCell& A) 
  /*!
    Assignement operator
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ZeroCellComp=A.ZeroCellComp;
      ConVar=A.ConVar;
    }
  return *this;
}

void
RemoveCell::substituteSurf(MonteCarlo::Qhull& Cell,const int OS,
			   const int NS) const
  /*!
    Substitute a particular surface on a cell
    The sign is preserved on the item unless
    NS is negative in which case the sign is reversed
    \param Cell :: Object ot substitue
    \param OS :: Old surface name
    \param NS :: new surface name
  */
{
  const int surfN=(NS>0) ? NS : -NS;
  Geometry::Surface* sptr=ModelSupport::surfIndex::Instance().getSurf(surfN);
  Cell.substituteSurf(OS,NS,sptr);
  return;  
}

void 
RemoveCell::reconfigure(FuncDataBase& FDB)
  /*!
    Reconfigure with different/new FDB and surface
    \param FDB :: Reference function database
  */
{
  ConVar=&FDB;
  this->populateZero();
  return;
}
