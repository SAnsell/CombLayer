/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/CoolPad.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

  const size_t ID;                     ///< ID number
  const std::string baseName;          ///< Base name
  
  double xStep;                     ///< Step accross surface
  double zStep;                     ///< Step down/up
  double thick;                     ///< Thickness of system
  double height;                    ///< Height 
  double width;                     ///< Width

  int Mat;                          ///< Al (inner) material

  HeadRule hotSurf;                 ///< hot surface to cool
  
  std::vector<Geometry::Vec3D> CPts;  ///< Channel points
  double IWidth;                      ///< Inner width
  double IDepth;                     ///< Inner height
  int IMat;                           ///< Inner material

  void populate(const FuncDataBase&);

  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createWaterTrack(Simulation&);

 public:

  CoolPad(const std::string&,const size_t);
  CoolPad(const CoolPad&);
  CoolPad& operator=(const CoolPad&);
  virtual ~CoolPad();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
