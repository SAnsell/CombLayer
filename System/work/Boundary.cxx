/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   work/Boundary.cxx
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
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>
#include <climits>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <algorithm>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "doubleErr.h"
#include "BUnit.h"
#include "Boundary.h"


std::ostream&
operator<<(std::ostream& OX,const BItems& A)
  /*!
    Write out the a standard stream
    \param OX :: Output stream
    \param A :: BItems to write
    \return Stream 
  */
{
  A.write(OX);
  return OX;
}

BItems::BItems()
  /// Constructor
{}

BItems::BItems(const BItems& A) :
  FList(A.FList)
  /*!
    Copy Constructor
    \param A :: BItem to copy
  */
{}

BItems&
BItems::operator=(const BItems& A) 
  /*!
    Assignment operator
    \param A :: BItems to copy
    \return *this
   */
{
  if (this!=&A)
    {
      FList=A.FList;
    }
  return *this;
}

void
BItems::addItem(const size_t Index,const double F)
  /*!
    Adds an item to the list
    \param Index :: Index of old component
    \param F :: Fraction
  */
{
  if (F>0.0)
    {
      std::vector<PTYPE>::iterator vc;
      vc=std::lower_bound(FList.begin(),FList.end(),Index,
		     mathSupport::PairSndLess<size_t,double>());
      if (vc!=FList.end() && vc->first==Index)
	{
	  vc->second=F;
	}
      else
        {
	  FList.insert(vc,PTYPE(Index,F));
	}
    }
  return;
}

BItems::PTYPE
BItems::getItem(const size_t Index) const
  /*!
    Get an individual component
    \param Index :: Index value
    \return original_Index / frac [-ve : 0 on Index out of range]
   */
{
  return (Index>=FList.size()) ? 
    PTYPE(ULONG_MAX,0) :
    FList[Index];
} 

void
BItems::write(std::ostream& OX) const
  /*!
    Write out the item to the stream
    \param OX :: Output stream
  */
{
  std::vector<PTYPE >::const_iterator vc;
  for(vc=FList.begin();vc!=FList.end();vc++)
    OX<<vc->first<<"("<<vc->second<<") ";
  return;
}

//---------------------------------------------------------
//             Boundary
//---------------------------------------------------------

Boundary::Boundary() : nonEmpty(0),backEmpty(0)
  /// Constructor
{}

Boundary::Boundary(const Boundary& A) : 
  Components(A.Components),nonEmpty(A.nonEmpty),
  backEmpty(A.backEmpty)
  /*!
    Copy Constructor
    \param A :: Object to copy
  */
{}

Boundary&
Boundary::operator=(const Boundary& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this;
  */
{
  if (this!=&A)
    {
      Components=A.Components;
      nonEmpty=A.nonEmpty;
      backEmpty=A.backEmpty;
    }
  return *this;
}

void
Boundary::setBoundary(const std::vector<double>& OData,
		      const std::vector<double>& NRegion)
  /*!
    Given a vector OData which is edge limited boundary
    region, remap onto NRegion 
    \param OData :: Old boundary values
    \param NRegion :: New boundary regions
  */
{
  if (OData.size()<2 || NRegion.size()<2)
    {
      if (OData.size()<2)
	throw ColErr::RangeError<int>(static_cast<int>(OData.size())
				      ,2,1000,"Boundary::setBoundary OData");
      else
	throw ColErr::RangeError<int>(static_cast<int>(NRegion.size())
				      ,2,1000,"Boundary::setBoundary NRegion");
    }
  Components.clear();
  Components.resize(NRegion.size());
  double NA(NRegion.front());
  double NB(NRegion[1]);
  double OA(OData[0]);
  double OB(OData[1]);
  
  // Initial setup:
  size_t nI(1);
  size_t oI(1);
  size_t index(0);
  while(NB<OA)
    {
      nI++;
      NA=NB;
      NB=NRegion[nI];
      index++;
    }
  for(;;)
    {
      double frac=getFrac(OA,OB,NA,NB);
      Components[nI-1].addItem(oI-1,frac);
      // now choose which one to increase
      if (NB>OB)      // Increase Old
        {
	  oI++;
	  if (oI==OData.size())
	    break;
	  OA=OB;
	  OB=OData[oI];
	}
      else
        {
	  nI++;
	  if (nI==NRegion.size())
	    break;
	  NA=NB;
	  NB=NRegion[nI];
	}
    }
  setEmpty();
 
  return;
}

void
Boundary::setBoundary(const std::vector<BUnit>& OData,
		      const std::vector<BUnit>& NRegion)
  /*!
    Given a vector OData which is edge limited boundary
    region, remap onto NRegion 
    \param OData :: Old boundary values
    \param NRegion :: New boundary regions
  */
{
  ELog::RegMethod RegA("Boundary","setBoundary<BUnit>");

  // Assume that both units are ordered:
  if (OData.empty() || NRegion.empty())
    {
      if (OData.size()<1)
	throw ColErr::IndexError<size_t>(OData.size(),
					 1000,"OData");
      else
	throw ColErr::IndexError<size_t>(NRegion.size(),
					 1000,"OData");
    }

  Components.clear();
  Components.resize(NRegion.size());

  
  size_t initPt(0);
  for(size_t nI=0;nI<NRegion.size();nI++)
    {
      const double& NA(NRegion[nI].xA);
      const double& NB(NRegion[nI].xB);
      for(size_t oJ=initPt;oJ<OData.size();oJ++)
	{
	  const double& OA(OData[oJ].xA);
	  const double& OB(OData[oJ].xB);
	  if (OB<NA) 
	    initPt++;
	  else if (OA>NB)
	    break;
	  else         // REAL work here:
	    {
	      const double frac=getFrac(OA,OB,NA,NB);
	      Components[nI].addItem(oJ,frac);
	    }
	}
    }
  setEmpty(); 
  return;
}
      
void
Boundary::setEmpty()
  /*!
    Sets the Empty variables based on
    the Empty components
   */
{
  ELog::RegMethod RegA("Boundary","setEmpty");

  nonEmpty=0;
  for(;nonEmpty<Components.size() &&
	Components[nonEmpty].isEmpty();nonEmpty++) ;
          
  // Special case Comp is empty 	
  // Points to the begin+1 (end)
  if (Components.empty())
    {
      backEmpty=1;
      return;
    }
  // Work from back to front (handles empty Components):
  backEmpty=Components.size();
  for(;backEmpty>0 && 
	Components[backEmpty-1].isEmpty();backEmpty--) ;

  return;  
}

double
Boundary::getFrac(const double OA,const double OB,
		  const double NA,const double NB) const
  /*!
    calcuate the fraction that NA:NB overlap 
    OA:OB (fraction of O-range)
    \param OA :: Low Old Range
    \param OB :: High Old Range
    \param NA :: Low New Range
    \param NB :: High New Range
    \return Fraction of Old in new
   */
{
  const double A((OA>NA) ? OA : NA);
  const double B((OB<NB) ? OB : NB);
  const double NR=B-A;
  return (NR>0) ? NR/(OB-OA): 0.0;
}

BItems::PTYPE
Boundary::getItem(const size_t cell,const size_t Index) const
  /*!
    Get an individual component [composed of items]
    \param cell :: Cell value [index of new set]
    \param Index :: Index value of the component
    \return original_Index / frac [-ve : 0 on Index out of range]
  */
{
  return (cell>=Components.size()) ? 
    BItems::PTYPE(ULONG_MAX,0) :
    Components[cell].getItem(Index);
}


