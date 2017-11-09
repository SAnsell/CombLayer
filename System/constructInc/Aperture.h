/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Aperture.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef constructSystem_Aperture_h
#define constructSystem_Aperture_h

class Simulation;


namespace constructSystem
{

/*!
  \class Aperture
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief Aperture system
*/

class Aperture : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::FrontBackCut

{  
 private:
 
  const int appIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double innerWidth;                ///< inner width
  double innerHeight;               ///< inner height
  double width;                     ///< full width x2
  double height;                    ///< full height
  double thick;                     ///< full thick
  size_t nLayers;                   ///< number of extra layers

  int voidMat;                  ///< void material
  int defMat;                   ///< Main material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Aperture(const std::string&);
  Aperture(const Aperture&);
  Aperture& operator=(const Aperture&);
  ~Aperture();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
