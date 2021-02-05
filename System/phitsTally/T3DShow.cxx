/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/T3DShow.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "MeshXYZ.h"

#include "phitsTally.h"
#include "T3DShow.h"

namespace phitsSystem
{

T3DShow::T3DShow(const int ID) :
  phitsTally("T3DShow",ID)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

T3DShow::T3DShow(const T3DShow& A) : 
  phitsTally(A),
  xyz(A.xyz)
  /*!
    Copy constructor
    \param A :: T3DShow to copy
  */
{}

T3DShow&
T3DShow::operator=(const T3DShow& A)
  /*!
    Assignment operator
    \param A :: T3DShow to copy
    \return *this
  */
{
  if (this!=&A)
    {
      phitsTally::operator=(A);
      xyz=A.xyz;
    }
  return *this;
}

  
T3DShow*
T3DShow::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new T3DShow(*this);
}

T3DShow::~T3DShow()
  /*!
    Destructor
  */
{}
  
void
T3DShow::write(std::ostream& OX,const std::string& fileHead) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  
  return;
}

}  // NAMESPACE phitsSystem

