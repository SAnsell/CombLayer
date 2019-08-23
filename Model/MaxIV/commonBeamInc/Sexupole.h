/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/Sexupole.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_Sexupole_h
#define xraySystem_Sexupole_h

class Simulation;


namespace xraySystem
{
/*!
  \class Sexupole
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Sexupole for Max-IV
*/

class Sexupole : public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const size_t NPole; 
  const std::string baseName;   ///< Base key
  
  double length;                ///< frame length

  double frameRadius;           ///< Gap to start of frame
  double frameThick;            ///< Thikckness of frame

  double poleYAngle;            ///< Rotation of +X Pole about Y
  double poleGap;               ///< Gap from centre point
  double poleRadius;            ///< Radius of pole piece cut
  double poleWidth;             ///< width [in rotated frame]

  double coilRadius;            ///< Radius of coil start
  double coilWidth;             ///< Cross width of coil
  
  int poleMat;                     ///< pole piece of magnet
  int coreMat;                     ///< core of magnet 
  int coilMat;                     ///< coil material
  int frameMat;                    ///< Iron material

  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Sexupole(const std::string&);
  Sexupole(const std::string&,const std::string&);
  Sexupole(const Sexupole&);
  Sexupole& operator=(const Sexupole&);
  virtual ~Sexupole();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
