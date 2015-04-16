/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/CoolPad.h
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
#ifndef moderatorSystem_CoolPad_h
#define moderatorSystem_CoolPad_h

class Simulation;

namespace moderatorSystem
{

  class Reflector;

/*!
  \class CoolPad
  \version 1.0
  \author S. Ansell
  \date September 2011
  \brief Cooling pads on the reflector
*/

class CoolPad :  public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int ID;                     ///< ID number
  const int padIndex;               ///< Index of surface offset
  int cellIndex;                    ///< Cell index
  int populated;                    ///< 1:var
  
  
  size_t fixIndex;                  ///< Index in Ref/Fixed system
  double xStep;                     ///< Step accross surface
  double zStep;                     ///< Step down/up
  double thick;                     ///< Thickness of system
  double height;                    ///< Height 
  double width;                     ///< Width

  int Mat;                          ///< Al (inner) material

  std::vector<Geometry::Vec3D> CPts;  ///< Channel points
  double IWidth;                      ///< Inner width
  double IDepth;                     ///< Inner height
  int IMat;                           ///< Inner material

  void populate(const Simulation&);

  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createWaterTrack(Simulation&);

 public:

  CoolPad(const std::string&,const int);
  CoolPad(const CoolPad&);
  CoolPad& operator=(const CoolPad&);
  virtual ~CoolPad();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
