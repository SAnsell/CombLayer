/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/VacuumBox.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell & Konstantin Batkov
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
#ifndef constructSystem_VacuumBox_h
#define constructSystem_VacuumBox_h

class Simulation;

namespace constructSystem
{
  class portSet;

/*!
  \class VacuumBox
  \version 1.1
  \author S. Ansell & K. Batkov
  \date March 2026
  \brief VacuumBox unit
*/

class VacuumBox :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  const bool centreOrigin;      ///< Construct on the first port

  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]

  double feHeight;            ///< fe height [top only]
  double feDepth;             ///< fe depth [low only]
  double feWidth;             ///< fe width [total]
  double feFront;             ///< fe front
  double feBack;              ///< fe back

  double portAXStep;          ///< XStep of port
  double portAZStep;          ///< ZStep of port
  double portAWallThick;      ///< Flange wall thickness
  double portATubeLength;     ///< Port tube
  double portATubeRadius;     ///< Port tube length

  double portBXStep;          ///< XStep of port
  double portBZStep;          ///< ZStep of port
  double portBXAngle;          ///< XAngle of port
  double portBZAngle;          ///< ZAngle of port
  double portBWallThick;      ///< Flange wall thickness
  double portBTubeLength;     ///< Port tube
  double portBTubeRadius;     ///< Port tube length

  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  constructSystem::portSet PSet; ///< Port set

  int voidMat;                ///< Void material
  int wallMat;                ///< Wall material
  int pipeMat;                ///< Pipe (port/flangeA/B) material

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createPorts(Simulation&);

 public:

  VacuumBox(const std::string&,const bool =0);
  VacuumBox(const VacuumBox&);
  VacuumBox& operator=(const VacuumBox&);
  ~VacuumBox() override;

  const portItem& getPort(const size_t index) const {return PSet.getPort(index);};
  void insertInCell(MonteCarlo::Object&) const override;
  void insertInCell(Simulation&,const int) const override;

  virtual void insertMainInCell(Simulation&,const int) const;
  virtual void insertMainInCell(Simulation&,const std::vector<int>&) const;
  virtual void insertPortInCell(Simulation&,const size_t,const int) const;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
