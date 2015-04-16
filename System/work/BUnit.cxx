/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   work/BUnit.cxx
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
#include <cmath>

#include "doubleErr.h"
#include "BUnit.h"

std::ostream&
operator<<(std::ostream& OX,const BUnit& A)
  /*!
    Write out Operator 
    \param OX :: Output stream
    \param A :: BUnit to write
   */
{
  A.write(OX);
  return OX;
}


BUnit::BUnit(const double& ixA,const double& ixB,
	     const DError::doubleErr& iY) :
  W(-1.0),xA(ixA),xB(ixB),Y(iY) 
  /*!
    Constructor
    \param ixA :: Start value [x]
    \param ixB :: End value [x]
    \param iY :: Data value 
  */
{}

BUnit::BUnit(const double& weight,const double& ixA,
	     const double& ixB,const DError::doubleErr& iY) :
  W(weight),xA(ixA),xB(ixB),Y(iY) 
  /*!
    Constructor
    \param weight :: Weight of bin
    \param ixA :: Start value [x]
    \param ixB :: End value [x]
    \param iY :: Data value 
  */
{}

BUnit::BUnit(const BUnit& A) :
  W(A.W),xA(A.xA),xB(A.xB),Y(A.Y)
  /*!
    Copy constructor
    \param A :: BUnit 
  */
{}


/// Assignment operator
BUnit& 
BUnit::operator=(const BUnit& A) 
  /*!
    Assignment operator
    \param A :: BUnit to copy
    \return *this
   */
{
  if (this!=&A)
    {
      W=A.W;
      xA=A.xA;
      xB=A.xB;
      Y=A.Y;
    }
  return *this;
}

BUnit&
BUnit::operator+=(const BUnit& A)
  /*!
    Plus 
    \param A :: Object to add
    \return *this
  */
{
  if (W>0.0 && A.W>0.0)
    {
      Y+=A.Y;
      W+=A.W;
    }
  else if (A.W>0.0)
    {
      Y+=A.Y;
      W=A.W;
    }
  else
    Y+=A.Y;

  return *this;
}

BUnit&
BUnit::operator-=(const BUnit& A)
  /*!
    Subtract
    \param A :: Object to multiply by
    \return *this
  */
{
  if (W>0.0 && A.W>0.0)
    {
      Y-=A.Y;
      W-=A.W;
    }
  else if (A.W>0.0)
    {
      Y-=A.Y;
      W=A.W;
    }
  else
    Y-=A.Y;
  return *this;
}


BUnit&
BUnit::operator*=(const BUnit& A)
  /*!
    Multiply
    \param A :: Object to multiply by
    \return *this
  */
{
  if (W>0.0)
    W*=A.Y;
  Y*=A.Y;
  return *this;
}


BUnit&
BUnit::operator/=(const BUnit& A)
  /*!
    Divide
    \param A :: Object to divide by
    \return *this
  */
{
  if (W>0.0)
    W/=A.Y;
  Y/=A.Y;
  return *this;
}


BUnit&
BUnit::operator+=(const DError::doubleErr& DE)
  /*!
    Addition
    \param DE :: Object to multiply by
    \return *this
  */
{
  Y+=DE;
  return *this;
}

BUnit&
BUnit::operator-=(const DError::doubleErr& DE)
  /*!
    Multiply
    \param DE :: Object to multiply by
    \return *this
  */
{
  Y-=DE;
  return *this;
}

BUnit&
BUnit::operator*=(const DError::doubleErr& DE)
  /*!
    Multiply
    \param DE :: Object to multiply by
    \return *this
  */
{
  if (W>0.0)
    {
      W*=DE;
    }
  Y*=DE;
  return *this;
}

BUnit&
BUnit::operator/=(const DError::doubleErr& DE)
  /*!
    Divide
    \param DE :: Object to divide by
    \return *this
  */
{
  if (W>0.0)
    W/=DE;
  Y/=DE;
  return *this;
}

BUnit
BUnit::operator+(const DError::doubleErr& DE) const
  /*!
    Addition
    \param DE :: Object to addition by
    \return *this + DE
  */
{
  BUnit A(*this);
  A+=DE;
  return A;
}


BUnit
BUnit::operator-(const DError::doubleErr& DE) const
  /*!
    Subtraction
    \param DE :: Object to subtraction by
    \return *this - DE
  */
{
  BUnit A(*this);
  A-=DE;
  return A;
}

BUnit
BUnit::operator*(const DError::doubleErr& DE) const
  /*!
    Multiply
    \param DE :: Object to multiply by
    \return *this * DE
  */
{
  BUnit A(*this);
  A*=DE;
  return A;
}

BUnit
BUnit::operator/(const DError::doubleErr& DE) const
  /*!
    Division
    \param DE :: Object to divide by
    \return *this / DE
  */
{
  BUnit A(*this);
  A/=DE;
  return A;
}

void
BUnit::norm()
  /*!
    Normalize to the weight
  */
{
  if (W>0.0)
    {
      const double ww=W;
      W=-1.0;
      Y/=ww;
    }
  return;
}

void
BUnit::write(std::ostream& OX) const
  /*!
    Write out the Unit to a stream
    \param OX :: Output stream
   */
{
  OX<<"["<<xA<<":"<<xB<<"] "<<Y;
  return;
}
