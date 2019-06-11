/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/MagnetM1.h
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
#ifndef xraySystem_MagnetM1_h
#define xraySystem_MagnetM1_h

class Simulation;


namespace xraySystem
{
  class Octupole;
  class Quadrupole;
  class Dipole;
  class EPCombine;
  
/*!
  \class MagnetM1
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Magnet block M1 

  This builds the M1 Magnet block around the EPCombine/EPSeparator

*/

class MagnetM1 :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double blockYStep;            ///< Step forward
  double length;                ///< frame length

  double outerVoid;             ///< Size of outer void gap
  double ringVoid;              ///< Size of outer void gap
  double topVoid;               ///< Size of outer void gap
  double baseVoid;              ///< Size of outer void gap

  double baseThick;             ///< base thickness
  double wallThick;             ///< side wall thickness
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material

  attachSystem::InnerZone buildZone;   ///<Main outerVoid
  /// dipole connection pipe
  std::shared_ptr<xraySystem::PreBendPipe> preDipole;
  /// Electron/photon combined track
  std::shared_ptr<xraySystem::EPCombine> epCombine;

  /// Octupole [first]
  std::shared_ptr<xraySystem::Octupole> Oxx;
  /// Quad [first]
  std::shared_ptr<xraySystem::Quadrupole> QFend;
  /// Octupole [second]
  std::shared_ptr<xraySystem::Octupole> Oxy;
  /// Quad [Second]
  std::shared_ptr<xraySystem::Quadrupole> QDend;
  /// Dipole
  std::shared_ptr<xraySystem::Dipole> DIPm;
  /// Octupole [third]
  std::shared_ptr<xraySystem::Octupole> Oyy;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createEndPieces();

 public:

  MagnetM1(const std::string&);
  MagnetM1(const MagnetM1&);
  MagnetM1& operator=(const MagnetM1&);
  virtual ~MagnetM1();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
