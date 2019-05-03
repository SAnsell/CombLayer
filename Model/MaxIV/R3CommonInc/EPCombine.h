/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/EPCombine.h
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
#ifndef xraySystem_EPCombine_h
#define xraySystem_EPCombine_h

class Simulation;


namespace xraySystem
{
/*!
  \class EPCombine
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief EPCombine for Max-IV 

  This is built relative to the proton channel
*/

class EPCombine : public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double length;                ///< frame length

  double photonXStep;            ///< Initial photon gap
  double electronXStep;          ///< Initial electorn gap  
  double electronXYAngle;        ///< XY angle of electron beam to proton

  double electronRadius;         ///< Electron radius
  double photonRadius;           ///< Photon radius

  double skinThick;              ///< skin thickness

  double wallXStep;              ///< X step for wall
  double wallStartLen;           ///< Outer wall start length
  double wallWidth;              ///< Outer wall box
  double wallHeight;             ///< Outer wall box

  double flangeAXStep;            ///< flange x step
  double flangeARadius;           ///< flange radius
  double flangeALength;           ///< flange length

  double flangeBXStep;            ///< back flange xstep
  double flangeBRadius;           ///< back flange radius
  double flangeBLength;           ///< back flange length
  
  int voidMat;                    ///< void material
  int wallMat;                    ///< wall material
  int flangeMat;                  ///< Port material

  Geometry::Vec3D elecOrg;        ///< Electron origin
  Geometry::Vec3D elecXAxis;       ///< Electron X-axis
  Geometry::Vec3D elecYAxis;       ///< Electron beam axis
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  EPCombine(const std::string&);
  EPCombine(const EPCombine&);
  EPCombine& operator=(const EPCombine&);
  virtual ~EPCombine();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
