/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/Reflector.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef moderatorSystem_Reflector_h
#define moderatorSystem_Reflector_h

class Simulation;

namespace TMRSystem
{
  class TS2target;
  class TS2ModifyTarget;
}

namespace moderatorSystem
{

/*!
  \class Reflector
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Reflector [insert object]
*/

class Reflector : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:
  
  double cornerAngle;           ///< Angle of corners
  double xySize;                ///< Left/Right size
  double zSize;                 ///< Vertical size
  double cutSize;               ///< End cuts
  int defMat;                   ///< Default material


  // The pads
  std::vector<moderatorSystem::CoolPad> Pads;

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks(const Geometry::Vec3D&,const Geometry::Vec3D&);

 public:

  Reflector(const std::string&);
  Reflector(const Reflector&);
  Reflector& operator=(const Reflector&);
  virtual ~Reflector();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);


};

}

#endif
 
