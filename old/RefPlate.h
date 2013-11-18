/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/RefPlate.h
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
#ifndef moderatorSystem_Reflector_h
#define moderatorSystem_Reflector_h

class Simulation;


namespace ts1System
{

/*!
  \class RefItem
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief TS1 RefItem [insert object]
*/

class RefItem : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  

  std::vector<int> RBoundary;   ///< Reflector boundary
  


  int defMat;                   ///< Default material
  

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createLinks();

 public:

  RefItem(const std::string&);
  RefItem(const RefItem&);
  RefItem& operator=(const RefItem&);
  virtual ~RefItem();

  std::string getComposite(const std::string&) const;
  virtual void addToInsertChain(attachSystem::ContainedComp&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 
