/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/EArrivalMon.h
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
#ifndef tdcSystem_EArrivalMon_h
#define tdcSystem_EArrivalMon_h

class Simulation;


namespace tdcSystem
{
/*!
  \class EArrivalMon
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief EArrivalMon for Max-IV
*/

class EArrivalMon :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;               ///< inner radius
  double length;               ///< inner length
  double thick;                ///< radius thicknees
  double faceThick;            ///< front/back wall thickness

  double frontPipeILen;        ///< Front pipe inner step
  double frontPipeLen;         ///< Front pipe length (to end of flange)
  double frontPipeRadius;      ///< Front pipe radius 
  double frontPipeThick;       ///< Front pipe thickness 

  double backPipeILen;         ///< Back pipe inner step
  double backPipeLen;          ///< Back pipe length (to end of flange)
  double backPipeRadius;       ///< Back pipe radius 
  double backPipeThick;        ///< Back pipe thickness 

  double flangeRadius;         ///< Joining Flange radius
  double flangeLength;         ///< Joining Flange length

  double windowRotAngle;       ///< Window angle relative to Z 
  double windowRadius;         ///< Window radius
  double windowThick;          ///< window thickness
  
  int voidMat;                  ///< void material
  int mainMat;                  ///< main material
  int windowMat;                ///< window material
  int flangeMat;                ///< flange material  


  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  EArrivalMon(const std::string&);
  EArrivalMon(const std::string&,const std::string&);
  EArrivalMon(const EArrivalMon&);
  EArrivalMon& operator=(const EArrivalMon&);
  virtual ~EArrivalMon();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
