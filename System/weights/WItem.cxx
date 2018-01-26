/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WItem.cxx
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
#include <boost/format.hpp>

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
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "WForm.h"
#include "WItem.h"

namespace WeightSystem
{

std::ostream&
operator<<(std::ostream& OX,const WItem& A)
  /*!
    Output stream
    \param OX :: output stream
    \param A :: WItem to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}


WItem::WItem(const int N) :
  cellN(N),Density(0),Tmp(300)
  /*!
    Constructor  (sets default temperature)
    \param N :: Cell number 
  */
{}

WItem::WItem(const int N,const double D,const double T) :
  cellN(N),Density(D),Tmp(T)
  /*!
    Constructor 
    \param N :: Cell number 
    \param D :: Density [atom/A^3]
    \param T :: Temperature [kelvin]
  */
{}

WItem::WItem(const WItem& A) :
  cellN(A.cellN),Density(A.Density),Tmp(A.Tmp),
  Val(A.Val)
  /*!
    Copy Constructor
    \param A :: Item to copy
  */
{}

WItem&
WItem::operator=(const WItem& A) 
  /*!
    Copy Constructor
    \param A :: Item to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellN=A.cellN;
      Density=A.Density;
      Tmp=A.Tmp;
      Val=A.Val;
    }
  return *this;
}

WItem::~WItem()
  /*!
    Destructor
  */
{}

int
WItem::isMasked() const
  /*!
    Check that the cell is completely masked
    \return 1 if all masked 0 otherwize
  */
{
  return (find_if(Val.begin(),Val.end(),
		  std::bind2nd(std::greater<double>(),0.0))) == Val.end() ?
    1 : 0;
}

void
WItem::mask()
  /*!
    Mask out a cell (assumes that it has already
    had the size of Val set.
  */
{
  ELog::RegMethod RegA("WItem","mask");

  fill(Val.begin(),Val.end(),-1);
  return;
}

void
WItem::mask(const size_t Index)
  /*!
    Mask out particluar component on a a cell.
    \param Index :: Item to use on mask
  */
{
  if (Index>=Val.size())
    throw ColErr::IndexError<size_t>(Index,Val.size(),"WItem::mask");
  Val[Index]=-1;
  return;
}

void
WItem::setWeight(const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight
    \param WT :: New weights (size is already checked)
  */
{
  Val=WT;
  return;
}

void
WItem::scaleWeight(const std::vector<double>& WT)
  /*!
    Scals all the cells to a specific weight
    \param WT :: New weights (size is already checked)
  */
{
  std::transform(Val.begin(),Val.end(),WT.begin(),
		 Val.begin(),std::multiplies<double>());
  
  return;
}

void
WItem::scaleWeight(const double& WT)
  /*!
    Scals all the cells to a specific weight
    \param WT :: New weights (size is already checked)
  */
{
  for(double& V : Val)
    V*=WT;
  return;
}

void
WItem::setWeight(const size_t Index,const double V)
  /*!
    Sets all the cells to a specific weight
    \param Index :: Index of the weight to set
    \param V :: Value to set 
  */
{
  if (Index>=Val.size())
    throw ColErr::IndexError<size_t>(Index,Val.size(),"WItem::setWeight");

  Val[Index]=V;
  return;
}

void
WItem::rescale(const double Tscale,const double SF)
  /*!
    Sets all the cells to a specific weight.
    Divides the weight by SF if temperature below Tscale
    \param Tscale :: Temperature to scale 
    \param SF :: Scale factor
  */
{
  // Most cell Tmp == 0 [assume 293.0K]
  
  if (Tmp<Tscale && (Tscale>293.0 || Tmp>1.0))
    transform(Val.begin(),Val.end(),Val.begin(),
	      std::bind2nd(std::divides<double>(),SF));
  return;
}
  
void
WItem::rescale(const int cA,const int cB,const double SF)
  /*!
    Scales all the cells by a scale factor.
    If the cell nubmer is between cA and cB
    \param cA :: Start cell number 
    \param cB :: End cell number 
    \param SF :: Scale factor
  */
{
  if (cellN>=cA && cellN<=cB) 
    {
      for(double& V : Val)
	V/=SF;
    }
  return;
}

bool
WItem::isNegative() const
  /*!
    Return true is V are negative
    \return 1 if negative
   */
{
  for(const double& V : Val)
    if (V<-Geometry::zeroTol)
      return 1;
  return 0;
}
  
void
WItem::write(std::ostream& OX) const
  /*!
    Writes out an individual cell weight item
    \param OX :: Output stream
  */
{
  boost::format FMT("%1$5.2e ");
  std::ostringstream cx;
  cx<<cellN<<"   ";

  std::vector<double>::const_iterator vc;
  for(vc=Val.begin();vc!=Val.end();vc++)
    cx<<FMT % (*vc);

  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

}   // NAMESPACE WeightSystem
