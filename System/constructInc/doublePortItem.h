/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/doublePortItem.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_doublePortItem_h
#define constructSystem_doublePortItem_h

class Simulation;
namespace ModelSupport
{
  class LineTrack;
}

namespace constructSystem
{
  
  /*!
    \class doublePortItem
    \brief This is double radius pipe attachement 
    \author S. Ansell
    \date January 2018
    \version 1.0

    This is a portItem which has two radii the first and the
    second and a closure between them e.g. for a telescopic pipe.
  */

class doublePortItem :
  public portItem
{
 private:
  
  double lengthA;     ///< Length of larger item
  double lengthB;     ///< Length of larger item
  double radiusB;             ///< larger radius of pipe
  double wallB;               ///< Wall thickness of B

  void createSurfaces();
  
  virtual void constructObject(Simulation&,const HeadRule&,const HeadRule&);
  void constructObjectIncreasing(Simulation&,const HeadRule&,const HeadRule&);
  void constructObjectReducing(Simulation&,const HeadRule&,const HeadRule&);
  
 public:

  doublePortItem(const std::string&);
  doublePortItem(const std::string&,const std::string&);
  doublePortItem(const doublePortItem&);
  doublePortItem& operator=(const doublePortItem&);
  virtual ~doublePortItem();

  virtual void populate(const FuncDataBase&);
  
};
  

}

#endif
 
