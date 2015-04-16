/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuildInc/BeamLine.h
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
#ifndef ts1System_EPBline_h
#define ts1System_EPBline_h

class Simulation;

namespace ts1System
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

  double innerRad;             ///< Inner radius 
  double wallThick;            ///< Wall thick
  size_t nSeg;                 ///< N Segment

  std::vector<Geometry::Ved3D> Pts;   ///< Point of ben
  std::vector<Geometry::Ved3D> YVec;  ///< Y Vector
  
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
 
