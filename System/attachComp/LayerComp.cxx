/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/LayerComp.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <limits>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Vec3D.h"
#include "interPoint.h"
#include "HeadRule.h"
#include "LayerComp.h"


namespace attachSystem
{

LayerComp::LayerComp(const size_t NL,const size_t INL) :
  nLayers(NL),nInnerLayers(INL)
  /*!
    Constructor 
    \param NL :: Number of layers
    \param INL :: Number of inner layers [def =0]
   */
{}

LayerComp::LayerComp(const LayerComp& A) : 
  nLayers(A.nLayers),nInnerLayers(A.nInnerLayers)
  /*!
    Copy constructor
    \param A :: LayerComp to copy
  */
{}

LayerComp&
LayerComp::operator=(const LayerComp& A)
  /*!
    Assignment operator
    \param A :: LayerComp to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nLayers=A.nLayers;
      nInnerLayers=A.nInnerLayers;
    }
  return *this;
}


double
LayerComp::intersectDist(const size_t layerIndex,
			 const long int sideIndex,
			 const Geometry::Vec3D& Origin,
			 const Geometry::Vec3D& Unit) const
 /*!
    Calculate the distance to the interect point
    \param layerIndex :: layer number
    \param sideIndex :: side number
    \param Origin :: Origin point
    \param Unit :: Unit distance
    \return distance [-ve max if no intersect]
  */
{
  ELog::RegMethod RegA("LayerComp","intersectDist");
  
  HeadRule HR=getLayerHR(layerIndex,sideIndex);
  if (HR.isEmpty())
    return -std::numeric_limits<double>::max();

  HR.populateSurf();
  const Geometry::interPoint inter=
    HR.trackSurfIntersect(Origin,Unit);
  
  return (inter.SNum) ? inter.D : -std::numeric_limits<double>::max();
}

}  // NAMESPACE attachSystem 
