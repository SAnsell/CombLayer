/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/EQDetector.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef photonSystem_EQDetector_h
#define photonSystem_EQDetector_h

class Simulation;

namespace photonSystem
{
  
/*!
  \class EQDetector
  \author S. Ansell
  \version 1.0
  \date July 2015
  \brief Simple B4c Round collimator with rectangular apperature
*/

class EQDetector : public attachSystem::ContainedComp,
   public attachSystem::FixedOffset
{
 private:

  const int detIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double yOffset;               ///< Outer volume
  double radius;                ///< Outer volume
  double length;                ///< Length of object

  double boxLength;             ///< Length of box
  double boxHeight;             ///< Height of box
  double boxWidth;              ///< Width of box

  double boxLead;               ///< Lead thickness
  double boxPlastic;            ///< Plastic thickness

  double viewRadius;            ///< RAdius for view port

  
  int plasticMat;               ///< Plastic material
  int detMat;                   ///< Detector material
  int leadMat;                  ///< Lead material

  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  EQDetector(const std::string&);
  EQDetector(const EQDetector&);
  EQDetector& operator=(const EQDetector&);
  virtual ~EQDetector();
  virtual EQDetector* clone() const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
