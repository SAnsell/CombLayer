/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/R3ChokeChamberGenerator.h
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

#ifndef setVariable_R3ChokeChamberGenerator_h
#define setVariable_R3ChokeChamberGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class R3ChokeChamberGenerator
  \version 1.0
  \author S. Ansell
  \date April 2019
  \brief R3ChokeChamberGenerator for variables
*/

class R3ChokeChamberGenerator
{
 private:

  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length
  double flangeRadius;        ///< Main Flange radius
  double flangeLength;        ///< Main Flange length
  
  double inletWidth;           ///< In pipe radius
  double inletHeight;          ///< In pipe height
  double inletLength;          ///< In pipe length [inc flange]
  double inletThick;           ///< In pipe wall thickness
  double flangeInletRadius;        ///< In Flange radius
  double flangeInletLength;        ///< In Flange length

  double electronXStep;          ///< Electron X step
  double electronXYAngle;        ///< Electron Angle 
  double electronRadius;     ///< Electron out radius
  double electronLength;     ///< Electron out lenght
  double electronThick;      ///< Electron pipe wall thickness
  double flangeElectronRadius;   ///< Electron Flange radius
  double flangeElectronLength;   ///< Electron Flange length
  
  double photonXStep;          ///< Photon X step
  double photonXYAngle;        ///< Photon Angle 
  double photonRadius;         ///< Photon out radius
  double photonLength;         ///< Photon out lenght
  double photonThick;          ///< Photon  wall thickness
  double flangePhotonRadius;   ///< Photon Flange radius
  double flangePhotonLength;   ///< Photon Flange length

  double sideRadius;           ///< Side out radius
  double sideLength;           ///< Side out lenght
  double sideThick;      ///< Side pipe wall thickness
  double flangeSideRadius;   ///< Side Flange radius
  double flangeSideLength;   ///< Side Flange length
  
  std::string voidMat;                ///< void material
  std::string wallMat;                ///< Wall material 
  std::string flangeMat;              ///< Flange material 


 public:

  R3ChokeChamberGenerator();
  R3ChokeChamberGenerator(const R3ChokeChamberGenerator&);
  R3ChokeChamberGenerator& operator=(const R3ChokeChamberGenerator&);
  ~R3ChokeChamberGenerator();


  void setRadius(const double);

  void setMaterial(const std::string&,const std::string&);
  
  void generateChamber(FuncDataBase&,const std::string&) const;

};

}

#endif
 
