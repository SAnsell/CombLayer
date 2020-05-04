/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/objectRegister.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef ModelSupport_objectRegister_h
#define ModelSupport_objectRegister_h

namespace attachSystem
{
  class FixedComp;
}

class objectGroups;

namespace MonteCarlo
{
  class Object;
}

namespace ModelSupport
{


/*!
  \class objectRegister 
  \version 2.0
  \author S. Ansell
  \date September 2018
  \brief Holds the objectGroup pointer

  Thie holds the objectGroup point to 
  allow objects to be register without excessive re-write
  and without excessive passing of a state variable.

*/

class objectRegister
{
 private:
 
  objectGroups* GPtr;    ///< The official object group pointer

  objectRegister();
  ///\cond SINGLETON
  objectRegister(const objectRegister&);
  objectRegister& operator=(const objectRegister&);
  ///\endcond SINGLETON
  
 public:
  
  ~objectRegister();

  static objectRegister& Instance();

  void setObjectGroup(objectGroups&);
  
  int cell(const std::string&,const size_t = 10000);
  void removeCell(const std::string&);

  // to be removed:
    /// Storage of component pointers
  typedef std::shared_ptr<attachSystem::FixedComp> CTYPE;
  void addObject(const std::string&,const CTYPE&);
  void addObject(const CTYPE&);

};

}

#endif
