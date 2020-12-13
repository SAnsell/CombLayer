/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/MagnetU1.h
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
#ifndef xraySystem_MagnetU1_h
#define xraySystem_MagnetU1_h

class Simulation;


namespace xraySystem
{
  class Quadrupole;
  class Sexupole;
  class Dipole;
  class CorrectorMag;
  
/*!
  \class MagnetU1
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Magnet block U1 

  This builds the U1 Magnet block around primary pipe. Allows the 
  dipole view pipe to cut the left outside corner (from MagnetM1)

*/

class MagnetU1 :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double blockXStep;            ///< X Centre offset
  double blockXYAngle;          ///< XY angle of block
  double blockYStep;            ///< Step forward
  double blockWidth;            ///< block width
  double blockLength;           ///< block length
  double blockHeight;           ///< block length
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  
  /// Quad [first]
  std::shared_ptr<xraySystem::Quadrupole> QFm1;
  /// Sextupole [first]
  std::shared_ptr<xraySystem::Sexupole> SFm;
  /// Quad [first]
  std::shared_ptr<xraySystem::Quadrupole> QFm2;

  /// vertial corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;
  /// horizontal corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;
  
  /// Sextupole [small]
  std::shared_ptr<xraySystem::Sexupole> SD1;
  /// Dipole
  std::shared_ptr<xraySystem::Dipole> DIPm;
  /// Sextupole [small]
  std::shared_ptr<xraySystem::Sexupole> SD2;
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createEndPieces();

 public:

  MagnetU1(const std::string&);
  MagnetU1(const MagnetU1&);
  MagnetU1& operator=(const MagnetU1&);
  virtual ~MagnetU1();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
