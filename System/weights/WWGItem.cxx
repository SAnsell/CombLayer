/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/WWGWeight.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "WWGItem.h"

namespace WeightSystem
{

WWGItem::WWGItem(const Geometry::Vec3D& CPt) :
  Pt(CPt)
  /*!
    Constructor
    \param CPt :: Centre point
  */
{}


WWGItem::WWGItem(const WWGItem& A) : 
  Pt(A.Pt),weight(A.weight)
  /*!
    Copy constructor
    \param A :: WWGItem to copy
  */
{}

WWGItem&
WWGItem::operator=(const WWGItem& A)
  /*!
    Assignment operator
    \param A :: WWGItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      weight=A.weight;
    }
  return *this;
}

void
WWGItem::write(std::ostream& OX) const
  /*!
    Debug write output 
    \param OX :: Output stream
   */
{
  
  OX<<Pt<<" :: ";
  for(const double& W : weight)
    OX<<W<<" "<<std::endl;
  return;
}


  
}  // NAMESPACE WeightSystem
