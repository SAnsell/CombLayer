/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/portItem.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
    \brief This is a half pipe added to a component
    \author S. Ansell
    \date January 2018
    \version 1.0

    This is a standard object except that it can find the surface
    of the object it is linked to. The intention is to add a port
    to an object e.g. a pipe or a box etc.
  */

class portItem :
  public attachSystem::FixedComp,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 protected:

  const std::string portBase;  ///< Base key name
  bool statusFlag;             ///< Flag to check object correct
  bool outerFlag;              ///< Make outer void

  Geometry::Vec3D centreOffset;  ///< Centre axis
  Geometry::Vec3D axisOffset;    ///< axis Offset
  
  double length;             ///< Length of port (to flange end)
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
  void createLinks();

  virtual void constructObject(Simulation&,
			       const HeadRule&,
			       const HeadRule&);
  
  
 public:

  portItem(const std::string&);
  portItem(std::string ,const std::string&);
  portItem(const portItem&);
  portItem& operator=(const portItem&);
  ~portItem() override;

  // make public as accessor function:
  virtual void populate(const FuncDataBase&);
  
  double getLength() const { return length; }
  double getCapLength() const
    { return std::max(capThick,0.0); }
  
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;
  void setCentLine(const attachSystem::FixedComp&,
		   const Geometry::Vec3D&,const Geometry::Vec3D&);
  void reNormZ(const Geometry::Vec3D&);

  void addOuterCell(const int);
  void setMain(const double,const double,const double);
  void setMaterial(const int,const int,const int = -3);
  void setFlange(const double,const double);
  void setCoverPlate(const double,const int= -1);
  /// surround the object
  void setWrapVolume() { outerFlag=1; }
  
  void constructTrack(Simulation&,MonteCarlo::Object*,
		      const HeadRule&,const HeadRule&);
  
  void intersectPair(Simulation&,portItem&) const;
  void intersectVoidPair(Simulation&,const portItem&) const;


  void constructAxis(Simulation&,
		     const attachSystem::FixedComp&,
		     const long int);

  void addPortCut(MonteCarlo::Object*) const;
  void addFlangeCut(MonteCarlo::Object*) const;
  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
				       
};
  

}

#endif
 
