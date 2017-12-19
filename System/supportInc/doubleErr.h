/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/doubleErr.h
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
#ifndef doubleErr_h
#define doubleErr_h

/*!
  \namespace DError
  \version 1.0
  \author S. Ansell
  \brief Numerical error handling
  \date September 2008
*/

namespace DError 
{

/*!
  \class doubleErr
  \brief Handles mathematical operations with error
  \author S. Ansell
  \version 0.8
  \date November 2005
*/

class doubleErr 
{ 

 private:
  
  double Val;           ///< The value
  double Err;           ///< The gaussian error term
             
 public:

  // constructors
  doubleErr();                              
  doubleErr(const double);                
  doubleErr(const double,const double);  
  doubleErr(const doubleErr&);              
  doubleErr& operator=(const doubleErr&);
  ~doubleErr();                         

  /// Cast to a double
  operator double() const
    { return Val; }

  doubleErr& operator+=(const doubleErr&); 
  doubleErr& operator-=(const doubleErr&);
  doubleErr& operator*=(const doubleErr&);
  doubleErr& operator/=(const doubleErr&);

  doubleErr& operator+=(const double&);
  doubleErr& operator-=(const double&);
  doubleErr& operator*=(const double&);
  doubleErr& operator/=(const double&);
  
  doubleErr operator+(const doubleErr&) const;
  doubleErr operator-(const doubleErr&) const;
  /// Negation operator
  doubleErr operator-() const { return *this * doubleErr(-1.0,0.0); } 
  doubleErr operator*(const doubleErr&) const;
  doubleErr operator/(const doubleErr&) const;

  
  doubleErr operator+(const double&) const;
  doubleErr operator-(const double&) const;
  doubleErr operator*(const double&) const;
  doubleErr operator/(const double&) const;
  
  /// Cast double value to a doubleError
  doubleErr operator()(const double& A) const 
    { return doubleErr(A,0.0); }


  /// Access value 
  double getVal() const { return Val; }
  /// Access err-part 
  double getErr() const { return sqrt(Err); }

  doubleErr& pow(const doubleErr&);
  doubleErr& sin();
  doubleErr& cos();
  doubleErr& tan();
  doubleErr& asin();
  doubleErr& acos();
  doubleErr& atan();
  doubleErr& exp();
  doubleErr& log();
  
  void write(std::ostream&) const;
  void read(std::istream&);
};
  
doubleErr pow(const doubleErr&,const doubleErr&);
doubleErr sin(const doubleErr&);
doubleErr cos(const doubleErr&);
doubleErr tan(const doubleErr&);
doubleErr asin(const doubleErr&);
doubleErr acos(const doubleErr&);
doubleErr atan(const doubleErr&);
doubleErr exp(const doubleErr&);
doubleErr log(const doubleErr&);

std::ostream& operator<<(std::ostream&,const doubleErr&);  
std::istream& operator>>(std::istream&,doubleErr&);

}  // NAMESPACE DError

#endif

