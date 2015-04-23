/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/InsertComp.h
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
#ifndef InsertComp_h
#define InsertComp_h

class Rule;

namespace attachSystem
{
/*!
  \class InsertComp
  \version 1.0
  \author S. Ansell
  \date August 2010
  \brief Allows an object to be wrapped

  Contains storage for the outer surface rule of an object.
  This storage can be access via a string for inclusion/exclusion
  of a particular object.
*/

class InsertComp  
{
 private:

  Rule* outerSurf;       ///< Outer surfaces
  
 public:

  InsertComp();
  InsertComp(const InsertComp&);
  InsertComp& operator=(const InsertComp&);
  virtual ~InsertComp();
  
  virtual std::string getExclude() const;

  void setInterSurf(const std::string&);
  void addInterSurf(const int);
  void addInterSurf(const std::string&);

};

}

#endif
 
