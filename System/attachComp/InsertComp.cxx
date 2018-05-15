/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/InsertComp.cxx
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
#include <deque>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "InsertComp.h"

namespace attachSystem
{

InsertComp::InsertComp() 
  /*!
    Constructor 
  */
{}

InsertComp::InsertComp(const InsertComp& A) : 
  outerSurf(A.outerSurf)
  /*!
    Copy constructor
    \param A :: InsertComp to copy
  */
{}

InsertComp&
InsertComp::operator=(const InsertComp& A)
  /*!
    Assignment operator
    \param A :: InsertComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      outerSurf=A.outerSurf;
    }
  return *this;
}


InsertComp::~InsertComp()
  /*!
    Deletion operator
  */
{}

void
InsertComp::setInterSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("InsertComp","addInterSurf(std::string)");
  outerSurf.procString(SList);
  return;
}

void
InsertComp::addInterSurf(const int SN) 
  /*!
    Add a surface to the output
    \param SN :: Surface number [inward looking]
  */
{
  ELog::RegMethod RegA("InsertComp","addInterSurf");

  outerSurf.addIntersection(SN);
  return;
}

void
InsertComp::addInterSurf(const std::string& SList) 
  /*!
    Add a set of surfaces to the output
    \param SList ::  Surface string [fully decomposed]
  */
{
  ELog::RegMethod RegA("InsertComp","addInterSurf(std::string)");
  outerSurf.addIntersection(SList);
  return;
}

std::string
InsertComp::getExclude() const
  /*!
    Calculate the write out the excluded surface
    \return Exclude string [union]
  */
{
  ELog::RegMethod RegA("InsertComp","getExclude");
  return outerSurf.complement().display();
}



  
  
}  // NAMESPACE attachSystem
