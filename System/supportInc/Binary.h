/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/Binary.h
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
#ifndef Binary_h
#define Binary_h

/*!
  \class Binary
  \brief display class for binary numbers
  \version 1.0
  \date Jan 2006 
  \author S. Ansell
*/

class Binary
{
private:

  unsigned long int num;      ///< Number stored
  int pad;                    ///< number of padding zeros

public:
  
  Binary();
  Binary(const unsigned long int&,const int =0);
  Binary(const std::vector<int>&,const int =0);
  Binary(const std::string&,const int =0);
  Binary(const std::string&,const unsigned int,const int =0);
  Binary(const char*,const unsigned int,const int =0);
  Binary(const unsigned char*,const unsigned int,const int =0);
  Binary(const Binary&);
  Binary& operator=(const Binary&);
  ~Binary();

  /// Conversion operator
  operator unsigned long int () const { return num; }
  /// Set Padding
  void setPad(const int P) { pad=P; }
  /// Accessor to the number
  unsigned long int getNumber() const { return num; }
  
  void setNum(const std::string&);
  void setNum(const unsigned long int&);

  void write(std::ostream&) const;
  void write(std::ostream&,const int) const;
};

std::ostream&
operator<<(std::ostream&,const Binary&);

#endif

