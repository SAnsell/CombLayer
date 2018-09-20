/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/TGShow.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "MeshXYZ.h"

#include "phitsTally.h"
#include "TGShow.h"

namespace phitsSystem
{

TGShow::TGShow(const int ID) :
  phitsTally(ID)
  /*!
    Constructor
    \param ID :: Identity number of tally 
  */
{}
  
TGShow*
TGShow::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new TGShow(*this);
}

TGShow::~TGShow()
  /*!
    Destructor
  */
{}
  

  
void
TGShow::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  return;
}

}  // NAMESPACE phitsSystem

