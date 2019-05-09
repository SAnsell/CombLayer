/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/Quadrupole.h
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
#ifndef xraySystem_Quadrupole_h
#define xraySystem_Quadrupole_h

class Simulation;


namespace xraySystem
{
/*!
  \class Quadrupole
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Quadrupole for Max-IV
*/

class Quadrupole : public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const std::string baseName;   ///< Base key

  double vertGap;               ///< Inner vertical space

  double length;                ///< frame length
  double width;                 ///< horrizontal width [inner]
  double height;                ///< vertical width [inner]

  double coilLength;            ///< Length of coil
  double coilCornerRad;         ///< Corner radius of coil
  double coilWidth;             ///< Width of coil
  
  double frameThick;            ///< Thikckness of frame

  double poleLength;            ///< Lenght in y axis
  double poleRadius;            ///< Radius of pole piece cut
  double poleZStep;             ///< Step down/up [NON-rotated frame]
  double poleYAngle;            ///< Rotation about Y axis
  double poleStep;              ///< Step down/up [rotated frame]
  double poleWidth;             ///< width [in rotated frame]
  
  int poleMat;                     ///< pole piece of magnet
  int coreMat;                     ///< core of magnet 
  int coilMat;                     ///< coil material
  int frameMat;                    ///< Iron material

  HeadRule innerTube;              ///< Inner tube 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Quadrupole(const std::string&);
  Quadrupole(const std::string&,const std::string&);
  Quadrupole(const Quadrupole&);
  Quadrupole& operator=(const Quadrupole&);
  virtual ~Quadrupole();

  /// set innner tube
  void setInnerTube(const HeadRule& HR) { innerTube=HR; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
