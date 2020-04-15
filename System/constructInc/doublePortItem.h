/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/doublePortItem.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

class doublePortItem :
  public portItem
{
 private:
  
  double externPartLen;     ///< Length of larger item
  double radiusB;           ///< larger radius of pipe

  void createSurfaces();
  
  virtual void constructOuterFlange(Simulation&,
				    const ModelSupport::LineTrack&,
				    const size_t,const size_t);
  
 public:

  doublePortItem(const std::string&);
  doublePortItem(const std::string&,const std::string&);
  doublePortItem(const doublePortItem&);
  doublePortItem& operator=(const doublePortItem&);
  virtual ~doublePortItem();

  virtual void populate(const FuncDataBase&);
  
  void setLarge(const double,const double);
  
};
  

}

#endif
 
