/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/Token.h
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
#ifndef Token_h
#define Token_h

/*!
  \struct Token
  \author S. Ansell
  \version 1.0
  \date March 2010
  \brief tokenize object definition from MCNPX
*/
class Token
{
 public:

  int type;       ///< type 0 : separator / 1 : surf / 2 :other
  char unit;      ///< unit if separator
  int num;        ///< number if surf

  /// Constructor for separator
  explicit Token(const char C) : type(0),unit(C),num(0) {}
  /// Constructor for surface
  explicit Token(const int N) : type(1),unit(0),num(N) {}
  explicit Token(const int,const char,const int);
  Token(const Token&);
  Token& operator=(const Token&);
  ~Token() {}    ///< Destructor

  bool operator==(const Token&) const;

  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const Token&);

#endif
