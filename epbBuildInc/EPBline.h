/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuildInc/EPBline.h
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
#ifndef epbSystem_EPBline_h
#define epbSystem_EPBline_h

class Simulation;

namespace epbSystem
{

/*!
  \class EPBline
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief EPBline [insert object]
*/

class EPBline : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int epbIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< X step
  double yStep;                 ///< Y step
  double zStep;                 ///< Z Step
  double xyAngle;
  double zAngle;

  double innerRad;             ///< Inner radius 
  double wallThick;            ///< Wall thick
  int wallMat;                 ///< Wall Mat
  size_t nSeg;                 ///< N Segment

  std::vector<Geometry::Vec3D> Pts;   ///< Point of ben
  std::vector<Geometry::Vec3D> YVec;  ///< Y Vector
  std::vector<Geometry::Vec3D> Cent;  ///< Calcualble centre!
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  EPBline(const std::string&);
  EPBline(const EPBline&);
  EPBline& operator=(const EPBline&);
  ~EPBline();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
