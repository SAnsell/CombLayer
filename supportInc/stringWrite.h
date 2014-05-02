/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/stringWrite.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef StrFunc_stringWrite_h
#define StrFunc_stringWrite_h

namespace StrFunc
{

template<typename T>
std::string stringWrite(const T& A) 
  /*!
    Process to convert an object into a string 
    using its write operator.
    \param A :: Object
    \return sting
   */
{
  std::ostringstream cx;
  cx<<A;
  return cx.str();
}

}

#endif
