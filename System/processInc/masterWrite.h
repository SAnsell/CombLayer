/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/masterWrite.h
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
#ifndef masterWrite_h
#define masterWrite_h

/*!
  \class masterWrite
  \version 1.0
  \date February 2011
  \author S. Ansell
  \brief Controls the tolerance of output
*/

class masterWrite
{
 private:

  double zeroTol;         ///< All numbers below this value are zero
  int sigFig;             ///< Number of significant figures
  
  boost::format FMTdouble;       ///< Format statement for double output
  boost::format FMTinteger;      ///< Format statement for integer

  masterWrite();

  ///\cond SINGLETON
  masterWrite(const masterWrite&);
  masterWrite& operator=(const masterWrite&);
  ///\endcond SINGLETON

 public:
  
  /// Desctructor
  ~masterWrite() {} 
  
  static masterWrite& Instance();

  void setSigFig(const int);
  void setZero(const double);


  template<typename T>
  std::string padNum(const T&,const size_t);

  std::string NameNoDot(std::string);
  std::string NumComma(const Geometry::Vec3D&);
  std::string Num(const Geometry::Vec3D&);
  std::string Num(const double&);
  std::string Num(const int&);
  std::string Num(const size_t&);
    
};

#endif
