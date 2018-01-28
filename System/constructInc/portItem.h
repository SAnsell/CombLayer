/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/portItem.h
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
#ifndef constructSystem_portItem_h
#define constructSystem_portItem_h

class Simulation;

namespace constructSystem
{
  /*!
    \class portItem
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

class portItem :
  public attachSystem::FixedComp,
  public attachSystem::ContainedComp
{
 private:

  size_t statusFlag;         ///< Flag to check object correct
  int portIndex;             ///< port surface offset
  int cellIndex;             ///< cell number

  double externalLength;     ///< Length of item 
  double radius;             ///< radius of pipe
  double wall;               ///< wall thick
  double flangeRadius;       ///< flange radius
  double flangeThick;        ///< flange thick

  int voidMat;               ///< Void material
  int wallMat;               ///< Wall material

  std::string refComp;       ///< Name of reference object
  Geometry::Vec3D exitPoint; ///< exit point of object

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:

  portItem(const std::string&);
  portItem(const portItem&);
  portItem& operator=(const portItem&);
  ~portItem();

  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void setCentLine(const Geometry::Vec3D&,const Geometry::Vec3D&);

  void setMain(const double,const double,const double);
  void setFlange(const double,const double);

  void constructTrack(Simulation&);
  
  void calcMaxCut(const std::string&,const size_t);
    
				       
};
  

}

#endif
 
