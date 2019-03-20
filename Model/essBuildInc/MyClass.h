/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/MyClass.h
 *
 * Copyright (c) 2019-2019 by Konstantin Batkov
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
#ifndef essSystem_MyClass_h
#define essSystem_MyClass_h

class Simulation;

namespace essSystem
{

/*!
  \class MyClass
  \version 1.0
  \author Konstantin Batkov
  \date 20 Mar 2019
  \brief Description
*/

class MyClass : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< height

  int mainMat;                   ///< main material
  double mainTemp; ///< main temperature

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  MyClass(const std::string&);
  MyClass(const MyClass&);
  MyClass& operator=(const MyClass&);
  virtual MyClass* clone() const;
  virtual ~MyClass();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


