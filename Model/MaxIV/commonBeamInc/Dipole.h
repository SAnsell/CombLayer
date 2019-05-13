/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/Dipole.h
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
#ifndef xraySystem_Dipole_h
#define xraySystem_Dipole_h

class Simulation;


namespace xraySystem
{
/*!
  \class Dipole
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Dipole for Max-IV
*/

class Dipole : public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const std::string baseName;   ///< Base key


  double length;                 ///< frame length
  double height;                 ///< height of frame
  
  double poleAngle;              ///< Arc of cylinder angle
  double poleRadius;             ///< radius of cylinder angle
  double poleGap;                ///< Gap from centre point
  double poleWidth;              ///< width [in rotated frame]

  double coilGap;                ///< Separation of coils
  double coilLength;             ///< Length of coil
  double coilWidth;              ///< Width of coil
  
  int poleMat;                     ///< pole piece of magnet
  int coilMat;                     ///< coil material

  HeadRule innerTube;              ///< Inner tube 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Dipole(const std::string&);
  Dipole(const std::string&,const std::string&);
  Dipole(const Dipole&);
  Dipole& operator=(const Dipole&);
  virtual ~Dipole();

  /// set innner tube
  void setInnerTube(const HeadRule& HR) { innerTube=HR; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
