/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModelInc/DetectorBank.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef instrumentSystem_DetectorBank_h
#define instrumentSystem_DetectorBank_h

class Simulation;

namespace Transport
{
  class DetGroup;
}

namespace d4cSystem
{

/*!
  \class DetectorBank
  \author S. Ansell
  \version 1.0
  \date January 2014
  \brief Grid of detector objects 
*/

class DetectorBank : 
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp
{
 private:
  
  const size_t bankNumber;        ///< Bank number
  const std::string baseName;     ///< Base name       

  double centreOffset;            ///< Main origin centre distance [Z Rot Axis]
  double centreAngle;             ///< Main origin Y Axis Rotatoin

  double detDepth;              ///< Radius of detector
  double detLength;             ///< Height/depth
  double detHeight;             ///< Height/depth
  double wallThick;             ///< Outer wall thickness

  int wallMat;                  ///< Wall material
  int detMat;                   ///< Detector material

  size_t nDet;                  ///< Nmber of detectors
  // Functions:

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,
			const long int) override;

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DetectorBank(const size_t,const std::string&);
  DetectorBank(const DetectorBank&);
  DetectorBank& operator=(const DetectorBank&);
  ~DetectorBank() override;

  void createTally(Simulation&) const;
  void createTally(Transport::DetGroup&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

  
};

}

#endif
 
