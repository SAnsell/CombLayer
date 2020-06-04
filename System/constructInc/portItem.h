/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/portItem.h
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
 protected:

  const std::string portBase;  ///< Base key name
  bool statusFlag;             ///< Flag to check object correct
  bool outerFlag;              ///< Make outer void

  Geometry::Vec3D centreOffset;
  Geometry::Vec3D axisOffset;
  
  double externalLength;     ///< Length of item 
  double radius;             ///< radius of pipe
  double wall;               ///< wall thick
  double flangeRadius;       ///< flange radius
  double flangeLength;       ///< flange thick(length)
  double capThick;           ///< Plate on flange [if thick>0]
  double windowRadius;       ///< window radius in plate flange [if thick>0]
  double windowThick;        ///< window thick in plate flange [if thick>0]

  int voidMat;               ///< Void material
  int wallMat;               ///< Wall material
  int capMat;                ///< plate Material
  int windowMat;             ///< window Material 
  int outerVoidMat;          ///< outer void Material [window outer/surround]

  std::set<int> outerCell;   ///< Extra cell to add outer to
  std::string refComp;       ///< Name of reference object
  Geometry::Vec3D exitPoint; ///< exit point of object
 
  virtual void createSurfaces();
  void createLinks(const ModelSupport::LineTrack&,
		   const size_t,const size_t);

  virtual void constructOuterFlange(Simulation&,
				    const ModelSupport::LineTrack&,
				    const size_t,const size_t);
  void calcBoundaryCrossing(const objectGroups&,
			    const ModelSupport::LineTrack&,
			    size_t&,size_t&) const;
  
 public:

  portItem(const std::string&);
  portItem(const std::string&,const std::string&);
  portItem(const portItem&);
  portItem& operator=(const portItem&);
  ~portItem();

  // make public as accessor function:
  virtual void populate(const FuncDataBase&);
  
  double getExternalLength() const { return externalLength; }
  double getCapLength() const
    { return std::max(capThick,0.0); }
  
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void setCentLine(const attachSystem::FixedComp&,
		   const Geometry::Vec3D&,const Geometry::Vec3D&);

  void addOuterCell(const int);
  void setMain(const double,const double,const double);
  void setMaterial(const int,const int,const int = -3);
  void setFlange(const double,const double);
  void setCoverPlate(const double,const int= -1);
  /// surround the object
  void setWrapVolume() { outerFlag=1; }
  
  void constructTrack(Simulation&);
  
  void intersectPair(Simulation&,portItem&) const;
  void intersectVoidPair(Simulation&,const portItem&) const;

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
				       
};
  

}

#endif
 
