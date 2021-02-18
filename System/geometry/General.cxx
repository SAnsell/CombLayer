/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/General.cxx
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
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "General.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const General& A)
  /*!
    Output stream
    \param OX :: output stream
    \param A :: General surface to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}

General::General() : Quadratic()
  /*!
    Standard Constructor
  */
{
  setBaseEqn();
}

General::General(const int N,const int T) : 
  Quadratic(N,T)
  /*!
    Standard Constructor creates a object
    \param N :: Name
    \param T :: Transform
  */
{
  // Called after it has been sized by Quadratic
  setBaseEqn();
}



General::General(const General& A) : 
  Quadratic(A)
  /*!
    Standard Copy Constructor
    \param A :: General Object to copy
  */
{}

General*
General::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return General(this)
  */
{
  return new General(*this);
}

General&
General::operator=(const General& A)
  /*!
    Standard assignment operator
    \param A :: General Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Quadratic::operator=(A);
    }
  return *this;
}

General::~General()
  /*!
    Destructor
  */
{}


int 
General::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX general string (GQ/SQ types)
    Despite type, moves both to the general equation.
    \param Pstr :: String to process (with name and transform)
    \return 0 on success, neg of failure
  */
{
  std::string Line=Pstr;
  std::string item;
  if (!StrFunc::section(Line,item) || item.length()!=2 ||
      (tolower(item[0])!='g' &&  tolower(item[0]!='s')) ||
      tolower(item[1])!='q')
    return -1;
      
  double num[10];
  int index;
  for(index=0;index<10 && 
	StrFunc::section(Line,num[index]);index++) ;
  if (index!=10)
    return -2;

  if (tolower(item[0])=='g')
    {
      // Note swapped values since MCNPX
      // uses xy yz xz [
      for(size_t i=0;i<10;i++)
	Quadratic::BaseEqn[i]=num[i];
      std::swap(Quadratic::BaseEqn[4],Quadratic::BaseEqn[5]);
    }
  else
    {
      Quadratic::BaseEqn[0]=num[0];
      Quadratic::BaseEqn[1]=num[1];
      Quadratic::BaseEqn[2]=num[2];
      Quadratic::BaseEqn[3]=0.0;;
      Quadratic::BaseEqn[4]=0.0;;
      Quadratic::BaseEqn[5]=0.0;;
      Quadratic::BaseEqn[6]=2*(num[3]-num[7]*num[0]);  
      Quadratic::BaseEqn[7]=2*(num[4]-num[8]*num[1]);
      Quadratic::BaseEqn[8]=2*(num[5]-num[9]*num[2]);
      Quadratic::BaseEqn[9]=num[0]*num[7]*num[7]+
	num[1]*num[8]*num[8]+num[2]*num[9]*num[9]-
	2.0*(num[3]*num[7]+num[4]*num[8]+num[5]*num[9])+
	num[6];
    }
  return 0;
  
}

void 
General::setBaseEqn()
  /*!
    Set baseEqn (nothing to do) as it is 
    already a baseEqn driven system
  */
{
  return;
}

}  // NAMESPACE Geometry
