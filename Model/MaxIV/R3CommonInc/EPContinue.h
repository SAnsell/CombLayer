/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/EPContinue.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_EPContinue_h
#define xraySystem_EPContinue_h

class Simulation;


namespace xraySystem
{
  class EPCombine;
/*!
  \class EPContinue
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief EPContinue for Max-IV 

  This is built relative to the proton channel
*/

class EPContinue :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  bool epPairSet;                 ///< Assignment of e-p position
  double length;                  ///< frame length

  double electronRadius;          ///< Electron radius
  
  double photonAGap;              ///< Photon radius at start [5mm]
  double photonBGap;              ///< Photon radius at end [14mm]

  double width;                   ///< Main width
  double height;                  ///< Main height

  double outerRadius;             ///< outer 
  
  int voidMat;                    ///< void material
  int wallMat;                    ///< wall material

  Geometry::Vec3D photOffset;     ///< Photon origin [offset]
  Geometry::Vec3D elecOffset;     ///< Electron origin [offset]
  Geometry::Vec3D elecXAxis;      ///< Electron X-axis
  Geometry::Vec3D elecYAxis;      ///< Electron beam axis
  
  void populate(const FuncDataBase&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  EPContinue(const std::string&);
  EPContinue(const EPContinue&);
  EPContinue& operator=(const EPContinue&);
  virtual ~EPContinue();

  void setEPOriginPair(const EPCombine&);
  void setEPOriginPair(const attachSystem::FixedComp&,
		       const std::string&,const std::string&);
  void setEPOriginPair(const attachSystem::FixedComp&,
		       const long int,const long int);
  
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
