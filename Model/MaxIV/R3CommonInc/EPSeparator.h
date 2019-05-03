/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/EPSeparator.h
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
#ifndef xraySystem_EPSeparator_h
#define xraySystem_EPSeparator_h

class Simulation;


namespace xraySystem
{
/*!
  \class EPSeparator
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief EPSeparator for Max-IV 

  This is built relative to the proton channel
*/

class EPSeparator : public attachSystem::FixedOffset,
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

  double wallXStep;              ///< Outer wall step
  double wallWidth;              ///< Outer wall box
  double wallHeight;             ///< Outer wall box

  double flangeRadius;           ///< flange  radius
  double flangeLength;           ///< flange length
  
  int voidMat;                    ///< void material
  int wallMat;                    ///< wall material
  int flangeMat;                  ///< Port material

  bool epPairSet;                 ///< Setting of phot/elec Origin.
  Geometry::Vec3D photOrg;        ///< Photon origin
  Geometry::Vec3D elecOrg;        ///< Electron origin
  
  Geometry::Vec3D elecXAxis;      ///< Electron X-axis
  Geometry::Vec3D elecYAxis;      ///< Electron beam axis

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  EPSeparator(const std::string&);
  EPSeparator(const EPSeparator&);
  EPSeparator& operator=(const EPSeparator&);
  virtual ~EPSeparator();

  void setEPOriginPair(const attachSystem::FixedComp&,const long int,
		       const long int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
