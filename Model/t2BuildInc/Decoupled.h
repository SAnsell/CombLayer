/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2BuildInc/Decoupled.h
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
#ifndef moderatorSystem_Decoupled_h
#define moderatorSystem_Decoupled_h

class Simulation;

namespace moderatorSystem
{

  class VanePoison;
/*!
  \class Decoupled
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief Decoupled [insert object]
*/

class Decoupled :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap
{
 protected:
  
  std::shared_ptr<VanePoison> VP;  ///< Vane poisoning

  double width;             ///< Total Width
  double height;            ///< Total height
  
  Geometry::Vec3D westCentre;  ///< Decoupled radius centre
  Geometry::Vec3D eastCentre;  ///< Decoupled 

  double westRadius;       ///< Inner West radius
  double eastRadius;       ///< Inner East radius
  double westDepth;       ///< Inner West depth
  double eastDepth;       ///< Inner East Depth

  double alCurve;      ///< Inner Curve Al thickness
  double alSides;      ///< Side of groove view
  double alUpDown;     ///< Top/Bottom of view

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  Geometry::Vec3D getDirection(const size_t) const;
  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,
			const long int) override;
  
  void createLinks();
  void createSurfaces();
  void createObjects(Simulation&);
  
 public:

  Decoupled(const std::string&);
  Decoupled(const Decoupled&);
  Decoupled& operator=(const Decoupled&);
  /// Clone function
  virtual Decoupled* clone() const { return new Decoupled(*this); }
  ~Decoupled() override;

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  int getDividePlane(const int) const;
  int viewSurf(const int) const;
  /// Need internal pipe
  virtual int needsHePipe() const { return 1; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
 
