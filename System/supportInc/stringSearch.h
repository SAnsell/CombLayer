/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/stringSearch.h
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
#ifndef StrFunc_stringSearch_h
#define StrFunc_stringSearch_h

namespace StrFunc
{
 /*!
   \class stringSearch
   \brief Facilitator for finding a short string begining longer string
   \author S. Ansell
   \date November 2014
   \version 1.0
 */

class stringSearch
{
 private:
  
  const size_t shortLen;
  std::string shortName;

 public:
  
 stringSearch() : shortLen(0) {}

 stringSearch(const std::string& S) :
    shortLen(S.size()),shortName(S) {}
    
  stringSearch(const stringSearch& A) :
    shortLen(A.shortLen),shortName(A.shortName) {}
  
  
  bool operator()(const std::string& B) const
  {
    return (shortName<B.substr(0,shortLen));
  }

  bool operator()(const std::string& shortA,const std::string& B) const
  {
    return (shortA<B.substr(0,shortA.size()));
  }
};
  
}

#endif
