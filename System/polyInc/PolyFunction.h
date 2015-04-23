/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   polyInc/PolyFunction.h
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
#ifndef mathLevel_PolyFunction_h
#define mathLevel_PolyFunction_h

namespace mathLevel
{

  /*!
    \class PolyFunction
    \version 1.0
    \author S. Ansell
    \date December 2007
    \brief Holds a polynominal base type
  */

class PolyFunction
{
 protected:

  double Eaccuracy;               ///< Polynomic accuracy

 public:

  static size_t getMaxSize(const std::string&,const char);

  PolyFunction();
  explicit PolyFunction(const double);
  PolyFunction(const PolyFunction&);
  PolyFunction& operator=(const PolyFunction&);
  virtual ~PolyFunction();

  ///\cond Abstract
  virtual PolyFunction& operator+=(const double) =0;  
  virtual PolyFunction& operator-=(const double) =0;
  virtual PolyFunction& operator*=(const double) =0;
  virtual PolyFunction& operator/=(const double) =0;

  virtual int read(const std::string&) =0;
  virtual int write(std::ostream&,const int= 0) const =0;
  ///\endcond Abstract

};

std::ostream& operator<<(std::ostream&,const PolyFunction&);


}  // NAMESPACE mathlevel

#endif
