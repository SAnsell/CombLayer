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
namespace ModelSupport
{
  class LineTrack;
}

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
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:

  size_t statusFlag;         ///< Flag to check object correct
  int portIndex;             ///< port surface offset
  int cellIndex;             ///< cell number

  double externalLength;     ///< Length of item 
  double radius;             ///< radius of pipe
  double wall;               ///< wall thick
  double flangeRadius;       ///< flange radius
  double flangeLength;       ///< flange thick(length)
  double plateThick;         ///< Plate on flange [if thick>0]
  
  int voidMat;               ///< Void material
  int wallMat;               ///< Wall material
  int plateMat;              ///< plate Material

  std::string refComp;       ///< Name of reference object
  Geometry::Vec3D exitPoint; ///< exit point of object

  
  void createSurfaces();
  void createLinks(const ModelSupport::LineTrack&,
		   const size_t,const size_t);

  void constructOuterFlange(Simulation&,
			    const ModelSupport::LineTrack&,
			    const size_t,const size_t);
  void calcBoundaryCrossing(const ModelSupport::LineTrack&,
			    size_t&,size_t&) const;
  
 public:

  portItem(const std::string&);
  portItem(const portItem&);
  portItem& operator=(const portItem&);
  ~portItem();

  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void setCentLine(const attachSystem::FixedComp&,
		   const Geometry::Vec3D&,const Geometry::Vec3D&);

  void setMain(const double,const double,const double);
  void setMaterial(const int,const int);
  void setFlange(const double,const double);
  void setCoverPlate(const double,const int= -1);

  void constructTrack(Simulation&);
  
  void calcMaxCut(const std::string&,const size_t);
    
				       
};
  

}

#endif
 
