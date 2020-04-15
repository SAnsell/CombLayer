/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1CommonInc/MagnetBlock.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef xraySystem_MagnetBlock_h
#define xraySystem_MagnetBlock_h

class Simulation;


namespace xraySystem
{
  class QuadUnit;
  class DipoleChamber;
  class VacuumPipe;
  
/*!
  \class MagnetBlock
  \version 1.0
  \author S. Ansell
  \date April 2020

  \brief Steel Magnet block for R1 [container]

  This builds the R1 Magnet block and associated units
*/

class MagnetBlock :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double blockYStep;            ///< Step forward of main block
  double aLength;               ///< first length  (x2)
  double bLength;               ///< second length (x2)
  double midLength;             ///< Mid length [full]

  double sectorAngle;            ///< Angle of sections 

  double height;                ///< full height
  double frontWidth;            ///< width across front
  
  int voidMat;                  ///< void material
  int outerMat;                 ///< wall material

  /// Quad unit
  std::shared_ptr<xraySystem::QuadUnit> quadUnit;

  /// Dipole chamber
  std::shared_ptr<xraySystem::DipoleChamber> dipoleChamber;

  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createEndPieces();

  void buildInner(Simulation&);
  
 public:

  MagnetBlock(const std::string&);
  MagnetBlock(const MagnetBlock&);
  MagnetBlock& operator=(const MagnetBlock&);
  virtual ~MagnetBlock();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
