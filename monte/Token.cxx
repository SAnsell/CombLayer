/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/Token.cxx
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Token.h"

std::ostream&
operator<<(std::ostream& OX,const Token& A)
  /*!
    Standard write out 
    \param OX :: Output stream
    \param A :: Token to write
    \return Stream
   */
{
  A.write(OX);
  return OX;
}

Token::Token(const int T,const char C,const int N) :
  type(T),unit(C),num(N)
  /*!
    Constructor
    \param T :: Type
    \param C :: Character unit
    \param N :: Number of surface / cell
  */
{}

Token::Token(const Token& A) :
  type(A.type),unit(A.unit),num(A.num)
  /*!
    Copy constructor
    \param A :: Token to copy
  */
{}

Token&
Token::operator=(const Token& A) 
  /*!
    Assignment operator
    \param A :: Token to copy
    \return *this
  */
{
  if (this!=&A)
    {
      type=A.type;
      unit=A.unit;
      num=A.num;
    }
  return *this;
}

bool
Token::operator==(const Token& A) const
  /*!
    Simple equality operator
    \param A :: Object to compare
    \return true/ false
  */
{
  return (type==A.type && unit==A.unit && num==A.num);
}

void
Token::write(std::ostream& OX) const
  /*!
    Write out to a stream
    \param OX :: Output stream
   */
{
  switch (type)
    {
    case 0:
      OX<<" "<<unit;
      return;
    case 1:
      OX<<" "<<num;
      return;
    case 2:
      OX<<" "<<unit<<num;
      return;
    }
  return;
}

