/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/EPSeparator.h
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
  double photonOuterGap;        ///< Gap to outer radius
  double photonRadius;          ///< photon inner radius
  double photonAGap;            ///< Initial photon gap
  double photonBGap;            ///< Final photon gap  
  double electronRadius;        ///< Electron radius

  double initEPSeparation;      ///< initial separation between e / p

  double electronAngle;         ///< XY angle of electron beam to proton

  double wallPhoton;            ///< Extra on photon side 
  double wallElectron;          ///< Extra on elextron side
  double wallHeight;            ///< Extra above/below 

  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material

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

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
