/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/Hydrogen.h
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
#ifndef moderatorSystem_Hydrogen_h
#define moderatorSystem_Hydrogen_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class Hydrogen
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Hydrogen [insert object]
*/

class Hydrogen : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int hydIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  
  double width;             ///< Total Width
  double height;            ///< Total height
  double depth;             ///< Total depth [Max]
  double radius;            ///< Total depth
  double innerXShift;       ///< Shift of inner space

  double alDivide;          ///< Divider wall thickness
  double alFront;           ///< Front surface
  double alTop;             ///< Top [away from target]
  double alBase;            ///< Base [near target]
  double alSide;            ///< Side walls

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  Geometry::Vec3D HCentre;  ///< Centre of Radius  
  int HCell;                ///< Main H2 cell

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void createSurfaces(const attachSystem::LinkUnit&);
  void createObjects(Simulation&);

 public:

  Hydrogen(const std::string&);
  Hydrogen(const Hydrogen&);
  Hydrogen& operator=(const Hydrogen&);
  ~Hydrogen();

  int getDividePlane() const;
  int viewSurf() const;
  /// Access Centre
  const Geometry::Vec3D& getHCentre() const { return HCentre; }
  /// Access Radius
  double getRadius() const { return radius; }
  int getMainBody() const { return HCell; }
  /// Access divide width
  double getAlDivide() const { return alDivide; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);

};

}

#endif
 
