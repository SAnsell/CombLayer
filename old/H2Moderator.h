/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/H2Moderator.h
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
#ifndef delftSystem_H2Moderator_h
#define delftSystem_H2Moderator_h

class Simulation;

namespace delftSystem
{

/*!
  \class H2Moderator
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief H2Moderator [insert object]
*/

class H2Moderator : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int hydIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;             ///< X Step
  double yStep;             ///< Y Step
  double zStep;             ///< Z Step

  double depth;             ///< Total depth [Max]
  double radius;            ///< Total depth
  double sideRadius;        ///< Side (height+width) radius
  double innerXShift;       ///< Shift of inner space

  int backDirection;        ///< Back rotation
  double backRadius;        ///< Rear radius [-ve to be concave]

  double alDivide;          ///< Divider wall thickness
  double alFront;           ///< Front surface
  double alTop;             ///< Top [away from target]
  double alBase;            ///< Base [near target]
  double alSide;            ///< Side walls

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  Geometry::Vec3D HCentre;  ///< Centre of Radius  
  Geometry::Vec3D GCentre;  ///< Centre of Radius [Back Plane]
  int HCell;                ///< Main H2 cell

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::SecondTrack&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  H2Moderator(const std::string&);
  H2Moderator(const H2Moderator&);
  H2Moderator& operator=(const H2Moderator&);
  ~H2Moderator();

  int getDividePlane() const;
  int viewSurf() const;
  int getMainBody() const { return HCell; }

  void createAll(Simulation&,const attachSystem::TwinComp&);

};

}

#endif
 
