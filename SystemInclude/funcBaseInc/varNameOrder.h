/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   funcBaseInc/varNameOrder.h
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
#ifndef varNameOrder_h
#define varNameOrder_h

class varNameOrder 
{
 public:

  varNameOrder();
  ~varNameOrder() {}

  template<typename T> bool
    operator()(const std::pair<std::string,T>& A,
	       const std::pair<std::string,T>& B) const
    {
      return operator()(A.first,B.first);
    }

  bool operator()(const std::string& A,
		  const std::string& B) const;
};

#endif
