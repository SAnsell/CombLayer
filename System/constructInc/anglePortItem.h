/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/anglePortItem.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef constructSystem_anglePortItem_h
#define constructSystem_anglePortItem_h

class Simulation;

namespace constructSystem
{

  /*!
    \class anglePortItem
    \brief Calculates the intersection port with an object
    \author S. Ansell
    \date January 2018
    \version 1.0
    
    This is NOT a standard FixedComp  because it 
    is an adjoint to an existing FixedComp. 
    The problem is the that this item MUST establish
    a full basis set or createUnitVector cannot work 
    when called from this object.
  */

class anglePortItem :
  public portItem
{

protected:

  Geometry::Vec3D bAxis;     ///< Secondary direction
  double lengthA;            ///< length to centre of turn
  double lengthB;            ///< length to cap from centre of turn
 
  virtual void createSurfaces();
  virtual void createLinks();
  
 public:

  anglePortItem(const std::string&);
  anglePortItem(const std::string&,const std::string&);
  anglePortItem(const anglePortItem&);
  anglePortItem& operator=(const anglePortItem&);
  ~anglePortItem();

  // make public as accessor function:
  virtual void populate(const FuncDataBase&);
      
  virtual void constructObject(Simulation&,
		      const HeadRule&,const HeadRule&);
  
  virtual void addFlangeCut(MonteCarlo::Object*) const;
  
  using FixedComp::createAll;
  virtual void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
				       
};
  

}

#endif
 
