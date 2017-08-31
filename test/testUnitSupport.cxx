/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testUnitSupport.cxx
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
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <iterator>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLvector.h"
#include "XMLgrid.h"
#include "XMLgroup.h"
#include "XMLcomp.h"
#include "XMLread.h"
#include "XMLcollect.h"
#include "XMLnamespace.h"

#include "testUnitSupport.h"

namespace UnitTest
{

void
Lower::procXML(XML::XMLcollect& XOut) const
  /*!
    Write out of the basic schema
    \param XOut :: Output stream
  */
{
  XOut.addNumComp("C",c);
  XOut.addNumComp("C",a);
  XOut.addNumComp("C",b);
  return;
}


void
Lower::write() const
  /// Write information
{
  //  std::cout<<"Lower == "<<a<<" "<<b<<" "<<c<<std::endl; 
  return;
}

// ------------------------------------------------
//          MID
// ------------------------------------------------

void
Mid::procXML(XML::XMLcollect& XOut) const
  /*!
    Write out of the basic schema
    \param XOut :: Output stream
   */
{
  XOut.addComp("A",a);
  XOut.addComp("B",b);
  XOut.addGrp("Lower");
  Hold.procXML(XOut);
  XOut.closeGrp();
  return;
}


void
Mid::write() const
  /// Write information
{
  // std::cout<<"Mid == "<<a<<" "<<b<<std::endl; 
  // std::cout<<"Sub == ";
  Hold.write();
  return;
}


// ------------------------------------------------
//          TOP
// ------------------------------------------------

void
Top::procXML(XML::XMLcollect& XOut) const
  /*!
    Write out of the basic schema
    \param XOut :: Output stream
   */
{
  XOut.addComp("A",a);
  XOut.addGrp("Mid");
  Hold.procXML(XOut);
  XOut.closeGrp();
  XOut.addComp("B",b);
  return;
}



void
Top::write() const
  /*!
    Write Out top level stuff
  */
{
  //  std::cout<<"Top == "<<a<<" "<<b<<std::endl; 
  //  std::cout<<"Sub == ";
  Hold.write();
  return;
}

}   // NAMESPACE UnitSupport
