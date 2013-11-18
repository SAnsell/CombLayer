/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/ReadFunctions.h
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

namespace physicsSystem
{
  class PhysicsCards;
}
class FuncDataBase;

/*!
  \namespace ReadFunc
  \brief Controls reading mcnpx files
  \version 1.0
  \author S. Ansell
  \date June 2010
*/
namespace ReadFunc
{
  typedef std::map<int,MonteCarlo::Qhull*> OTYPE;      ///< Object type

  void removeDollarComment(std::string&);
  void processDollarString(FuncDataBase&,std::string&);
  int processSurface(const std::string&,const int);
  int readSurfaces(FuncDataBase&,std::istream&,const int);

  int checkInsert(const MonteCarlo::Qhull&,const int,OTYPE&);
  int mapInsert(const OTYPE&,OTYPE&);

  int readCells(FuncDataBase&,std::istream&,const int,OTYPE&);
  int readMaterial(std::istream&);
  int readPhysics(FuncDataBase&,std::istream&,
		  physicsSystem::PhysicsCards*);
  
}
