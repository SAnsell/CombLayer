/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/CopiedComp.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef attachSystem_CopiedComp_h
#define attachSystem_CopiedComp_h

class Simulation;

namespace attachSystem
{
/*!
  \class CopiedComp
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Creates a copy of an instrument based on a name/name system
*/

class CopiedComp  
{
 protected:

  std::string baseName;        ///< Base name 
  std::string newName;         ///< New name
  
 public:

  CopiedComp();
  CopiedComp(const std::string&,const std::string&);         
  CopiedComp(const CopiedComp&);
  CopiedComp& operator=(const CopiedComp&);
  virtual ~CopiedComp() {}     ///< Destructor

  void process(FuncDataBase&) const;
};

}

#endif
 
