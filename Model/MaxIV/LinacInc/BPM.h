/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/BPM.h
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
#ifndef tdcSystem_BPM_h
#define tdcSystem_BPM_h

class Simulation;


namespace tdcSystem
{
/*!
  \class BPM
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief BPM for Max-IV
*/

class BPM :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius   
  double length;                ///< void length [total]

  double outerThick;            ///< pipe thickness
  double claddingThick;         ///< cladding thickness

  double innerRadius;           ///< inner radius
  double innerThick;            ///< Inner electorn thickness
  double innerAngle;            ///< Angle of electrode
  double innerAngleOffset;      ///< Offset angle of inner electron

  double flangeARadius;         ///< Joining Flange radius 
  double flangeALength;         ///< Joining Flange length

  double flangeBRadius;         ///< Joining Flange radius 
  double flangeBLength;         ///< Joining Flange length

  double electrodeRadius;       ///< Electrode distance [support]
  double electrodeThick;        ///< Electrode thickness [support]
  double electrodeYStep;        ///< Electrode YStep
  double electrodeEnd;          ///< Electrode end piece
  
  int voidMat;                  ///< void material
  int electrodeMat;             ///< electrode material  
  int outerMat;                 ///< pipe material

  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BPM(const std::string&);
  BPM(const std::string&,const std::string&);
  BPM(const BPM&);
  BPM& operator=(const BPM&);
  virtual ~BPM();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
