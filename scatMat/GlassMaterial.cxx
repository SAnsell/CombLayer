/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   scatMat/GlassMaterial.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/multi_array.hpp>

#include "MersenneTwister.h"
#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h" 
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "Simpson.h"
#include "RefCon.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "neutron.h"
#include "neutMaterial.h"
#include "GlassMaterial.h"

extern MTRand RNG;

namespace scatterSystem
{


GlassMaterial::GlassMaterial() : neutMaterial()
 /*!
    Constructor
  */
{}

GlassMaterial::GlassMaterial(const std::string& N,const double M,
			     const double D,const double B,
			     const double S,const double I,
			     const double A) : 
  neutMaterial(N,M,D,B,S,I,A),refIndex(1.33)
  /*!
    Constructor for values: Sets the refractive index to 1.33
    \param N :: GlassMaterial name
    \param M :: Mean atomic mass
    \param D :: density [atom/angtrom^3]
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

GlassMaterial::GlassMaterial(const double D,const double M,const double B,
			     const double S,const double I,const double A) : 
  neutMaterial(D,M,B,S,I,A),refIndex(1.33)
  /*!
    Constructor for values : sets the refractive index to 1.33
    \param D :: density [atom/angtrom^3]
    \param M :: Mean atomic mass
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

GlassMaterial::GlassMaterial(const GlassMaterial& A) : 
  neutMaterial(A),refIndex(A.refIndex)
  /*!
    Copy constructor
    \param A :: Crystal to copy
  */
{}

GlassMaterial&
GlassMaterial::operator=(const GlassMaterial& A)
  /*!
    Assignment operator
    \param A :: GlassMaterial to copy
    \return *this
  */
{
  if (this!=&A)
    {
      neutMaterial::operator=(A);
      refIndex=A.refIndex;
    }
  return *this;
}

GlassMaterial*
GlassMaterial::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new GlassMaterial(*this);
}

GlassMaterial::~GlassMaterial() 
  /*!
    Destructor
  */
{}
  

} // NAMESPACE scatterSystem

