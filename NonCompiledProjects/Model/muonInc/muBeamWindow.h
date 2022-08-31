/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muonInc/muBeamWindow.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_muBeamWindow_h
#define muSystem_muBeamWindow_h

class Simulation;

namespace muSystem
{

/*!
  \class muBeamWindow
  \author G. Skoro
  \version 1.0
  \date February 2014
  \brief Muon beam window object
*/

class muBeamWindow : public attachSystem::FixedRotate,
    public attachSystem::ContainedComp
{
 private:
  
     // flange cylinder
  double flCylOutRadius;          ///< Flange outer radius
  double flCylInRadius;           ///< Flange inner radius
  double flCylThick;              ///< Flange thickness
     // window
  double windowThick;             ///< Window thickness
     // small cylinder
  double smCylOutRadius;
  double smCylThick;  
     // small cone               
  double smRadStartCone;       
  double smRadStopCone;  
  double smLengthCone;     
     // big cone out               
  double bgOutRadStartCone;        ///< Big cone Start
  double bgOutRadStopCone;         ///< Big cone End
  double bgOutLengthCone;          ///< Big cone length
     // big cone in               
  double bgInRadStartCone;       
  double bgInRadStopCone;  
  double bgInLengthCone;     

  double endRadCyl;
  double endRadStartCone;       
  double endRadStopCone;  
  double endLength;   

  double bigCylOutRadius;
  double bigCylInRadius; 
  double bigCylThick;  

  // materials
  int vessMat;                   ///< Main body material
  int windowMat;                 ///< Window mat

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  muBeamWindow(const std::string&);
  muBeamWindow(const muBeamWindow&);
  muBeamWindow& operator=(const muBeamWindow&);
  virtual ~muBeamWindow();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);  
};

}

#endif
 
