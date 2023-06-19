/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/Torpedo.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef shutterSystem_Torpedo_h
#define shutterSystem_Torpedo_h

class Simulation;

namespace shutterSystem
{
  /*!
    \class Torpedo
    \version 1.0
    \author S. Ansell
    \date February 2011
    \brief Holds a simple basic ISIS torpedo
    
    Can be specialized a vacuum/air torpedo 
  */

class Torpedo :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::ExternalCut
{
 private:

  const std::string baseName;         ///< Base Keyname 
  const size_t shutterNumber;         ///< number of the shutter

  Geometry::Convex vBox;              ///< Convex box of torpedo

  double voidXoffset;                 ///< Main void vessel offset 
  double xyAngle;                     ///< Angle of beamline
  double innerRadius;                 ///< Inner radius

  double Height;                      ///< Total height 
  double Width;                       ///< Total depth

  //--------------
  // FUNCTIONS:
  //--------------

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void calcVoidIntercept();
  void calcConvex(Simulation&);
  void createLinks();

 public:

  Torpedo(const size_t,const std::string&);
  Torpedo(const Torpedo&);
  Torpedo& operator=(const Torpedo&);
  ~Torpedo() override;

  
  void addCrossingIntersect(Simulation&,const Torpedo&);
  int findPlane(const Geometry::Face&) const;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
