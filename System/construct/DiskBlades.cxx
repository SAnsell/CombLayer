/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/DiskBlades.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "mathSupport.h"

#include "DiskBlades.h"

namespace constructSystem
{

DiskBlades::DiskBlades() : 
  nBlades(0),thick(0.0),innerThick(0.0),
  innerMat(0),outerMat(0)
  /*!
    Constructor
  */
{}
  
DiskBlades::DiskBlades(const DiskBlades& A) : 
  nBlades(A.nBlades),phaseAngle(A.phaseAngle),
  openAngle(A.openAngle),thick(A.thick),
  innerThick(A.innerThick),innerMat(A.innerMat),
  outerMat(A.outerMat)
  /*!
    Copy constructor
    \param A :: DiskBlades to copy
  */
{}

DiskBlades&
DiskBlades::operator=(const DiskBlades& A)
  /*!
    Assignment operator
    \param A :: DiskBlades to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nBlades=A.nBlades;
      phaseAngle=A.phaseAngle;
      openAngle=A.openAngle;
      thick=A.thick;
      innerThick=A.innerThick;
      innerMat=A.innerMat;
      outerMat=A.outerMat;
    }
  return *this;
}

void
DiskBlades::addOpen(const double pA,const double oA)
  /*
    Adds a single phase opening to the system
    \param pA :: Phase angle [mid]
    \param oA :: opening value (half either side)
  */
{
  ELog::RegMethod RegA("DiskBlades","addOpen");

  for(size_t i=0;i<nBlades;i++)
    {
      if (pA<(phaseAngle[i]+openAngle[i]) &&
	  (pA+oA)>(phaseAngle[i]))
	throw ColErr::NumericalAbort("phase angles overlap");
    }
  const long int Index=
    mathFunc::binSearch(phaseAngle.begin(),phaseAngle.end(),pA);

  phaseAngle.insert(phaseAngle.begin()+Index,pA);
  openAngle.insert(openAngle.begin()+Index,oA);
  nBlades++;
  return;
}

double
DiskBlades::getOpenAngle(const size_t index) const
  /*!
    Return a phase correct number
    \param index :: index value
    \return angle [deg]
  */
{
  if (!nBlades) return 0.0;
  return (index>=nBlades) ? phaseAngle[index % nBlades]+360.0 :
    phaseAngle[index];
}

double
DiskBlades::getCloseAngle(const size_t index) const
  /*!
    Return a phase corrected closing angle [actual]
    \param index :: index value
    \return angle [deg]
  */
{
  if (!nBlades) return 0.0;
  return (index>=nBlades) ?
    openAngle[index % nBlades]+phaseAngle[index % nBlades]+360.0 :
    phaseAngle[index]+openAngle[index];
}
  
 
}  // NAMESPACE constructSystem
