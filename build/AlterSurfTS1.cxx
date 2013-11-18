/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/AlterSurfTS1.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "Exception.h"
#include "MapSupport.h"
#include "Triple.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "AlterSurfBase.h"
#include "AlterSurfTS1.h"

AlterSurfTS1::AlterSurfTS1(FuncDataBase& A) : AlterSurfBase(A)
  /*!
    Constructor. Requires that the FuncDataBase object
    has been fully populated.
    \param A :: Reference to FuncDataBase object 
  */
{
  setMap();
  setMapZero();
}

AlterSurfTS1::AlterSurfTS1(const AlterSurfTS1& A) :
  AlterSurfBase(A)
  /*!
    Basic copy constructor
    \param A :: AlterSurfTS1 object to copy
  */
{}

AlterSurfTS1&
AlterSurfTS1::operator=(const AlterSurfTS1& A) 
  /*!
    Assignment operator 
    \param A :: AlterSurfTS1 object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      AlterSurfBase::operator=(A);
    }
  return *this;
}

AlterSurfTS1::~AlterSurfTS1()
  /*!
    Destructor
  */
{}

void
AlterSurfTS1::setMapZero()
  /*!
    Given a set of strings set the exchange on the string
    The principle is that if the variable equal 0, then
    the first surface is substituted for the second.
    A cascade is run for each.
  */
{
  std::vector<DTriple<std::string,int,int> >DX;

  // Liquid Methane pre-mod:
  DX.push_back(DTriple<std::string,int,int>("mPreLWidth",32003,10003));
  DX.push_back(DTriple<std::string,int,int>("mPreRWidth",32004,10004));
  DX.push_back(DTriple<std::string,int,int>("mPreTop",32005,10005));
  DX.push_back(DTriple<std::string,int,int>("mPreBase",32006,10006));


      std::vector<DTriple<std::string,int,int> >::const_iterator vc;
      for(vc=DX.begin();vc!=DX.end();vc++)
        {
	  if (ConVar.EvalVar<double>(vc->first)<=0.0)
	    setChange(vc->second,vc->third);
	}
      return; 
}


void
AlterSurfTS1::setMap()
  /*!
    Creates the altered plane list
  */

{
  const double methPoison=ConVar.EvalVar<double>("methPoison");

  if (methPoison<=0.0)
    {
      setChange(31101,0);
      setChange(31102,0);
    }

  return;
}
