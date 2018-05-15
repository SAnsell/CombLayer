/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WForm.cxx
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
#include <iomanip>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "WForm.h"

namespace WeightSystem
{

WForm::WForm()  :  pType("n"),
  wupn(2.0),wsurv(0.7*wupn),maxsp(5),
  mwhere(0),mtime(0)
  /*!
    Constructor
  */
{} 

WForm::WForm(const std::string& P)  :  pType(P),
  wupn(2.0),wsurv(0.7*wupn),maxsp(5),
  mwhere(0),mtime(0)
  /*!
    Constructor
    \param c :: Particle type
  */
{}

WForm::WForm(const WForm& A) : 
  pType(A.pType),Energy(A.Energy),wupn(A.wupn),wsurv(A.wsurv),
  maxsp(A.maxsp),mwhere(A.mwhere),mtime(A.mtime)
  /*!
    Copy constructor
    \param A :: WForm to copy
  */
{}

WForm&
WForm::operator=(const WForm& A)
  /*!
    Assignment operator
    \param A :: WForm to copy
    \return *this
  */
{
  if (this!=&A)
    {
      pType=A.pType;
      Energy=A.Energy;
      wupn=A.wupn;
      wsurv=A.wsurv;
      maxsp=A.maxsp;
      mwhere=A.mwhere;
      mtime=A.mtime;
    }
  return *this;
}

bool
WForm::operator==(const WForm& A) const
  /*!
    Operator to determine if although the types
    are different the energy and the parameters
    are the same.
    \param A :: WForm object to compare
    \return A == This
  */
{
  if (A.Energy.size()!=Energy.size())
    return 0;
  if (!std::equal(Energy.begin(),Energy.end(),A.Energy.begin()))
    return 0;
  if (fabs(wupn-A.wupn)/std::max(wupn,A.wupn)>1e-6 ||
      fabs(wsurv-A.wsurv)/std::max(wsurv,A.wsurv)>1e-6 ||
      maxsp!=A.maxsp ||
      mwhere!=A.mwhere ||
      mtime!=A.mtime)
    return 0;
  // Everthing equal
  return 1;
}

void
WForm::setParam(const double WP,const double WS,
		   const int MS,const int WC,
		   const int MT)
  /*!
    Sets the Control parameters
    \param WP :: up-split weight
    \param WS :: distruction weight
    \param MS :: Max number of splits
    \param WC :: Where to check
    \param MT :: Time / Energy
    \return 0 on succes and 
  */
{
  ELog::RegMethod RegA("WForm","setParam");

  if (WP<2 || WS<1 || WS>=WP ||
      MS<=1 || WC<-1 || WC>1 || (MT!=0 && MT!=1))
    throw ColErr::ExBase(0,"Param error with WP/WS/MS/MT");

  wupn=WP;
  wsurv=WS;
  maxsp=MS;
  mwhere=WC;
  mtime=MT;
    
  return;
}

void
WForm::setEnergy(const std::vector<double>& E)
  /*!
    Sets the energy list
    \param E :: Energy vector (already sorted)
  */
{
  Energy=E;
  return;
}

}   // NAMESPACE WeightSystem
