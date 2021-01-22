/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1CommonInc/DipoleSndBend.h
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
#ifndef xraySystem_DipoleSndBend_h
#define xraySystem_DipoleSndBend_h

class Simulation;


namespace xraySystem
{
/*!
  \class DipoleSndBend
  \version 1.0
  \author S. Ansell
  \date October 2020

  \brief DipoleSndBend for Max-IV 

  This is the extacttion section in the dipole chamber of R1 
  between dipole magnets
*/

class DipoleSndBend :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double arcAngle;             ///< full arc angle [deg]

  double curveRadius;          ///< Radius of centre line
  double flatWidth;            ///< Step to outer surf
  double outerFlat;            ///< straight line from beam
  
  double tipHeight;                ///< Beam height
  double height;                ///< Beam height
  double outerHeight;           ///< Outer height

  double wallThick;             ///< Wall thickness
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  int outerMat;                 ///< inner material
  
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DipoleSndBend(const std::string&);
  DipoleSndBend(const DipoleSndBend&);
  DipoleSndBend& operator=(const DipoleSndBend&);
  virtual ~DipoleSndBend();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
