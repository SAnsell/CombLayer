/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/BeamScrapper.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef xraySystem_BeamScrapper_h
#define xraySystem_BeamScrapper_h

class Simulation;

namespace xraySystem
{

/*!
  \class BeamScrapper
  \version 2.0
  \author Stuart Ansell
  \date October 2021
  \brief Cooled Beam Viewer screen
*/

class BeamScrapper :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double tubeRadius;        ///< inner water radius
  double tubeOffset;        ///< +/- beam offset of each pipe
  double tubeWall;          ///< wall thickness of pipe

  double plateOffset;       ///< Screen plate offset
  double plateYStep;         ///< Step in Y around pipe centre
  double plateAngle;        ///< Z rotation of screen (y is tube dir)
  double plateLength;       ///< Length along beam (angled)
  double plateHeight;       ///< Height of plate
  double plateThick;        ///< Thickness of plate
  
  double tubeWidth;         ///< in-beam copper block width
  double tubeHeight;        ///< in-beam copper block height 

  double inletZOffset;      ///< Variable for Z displacement of pipe inlet
  
  int voidMat;              ///< void material

  int waterMat;             ///< water material
  int copperMat;            ///< pipe outer material

  
  
  // Geometry points for object / surface building :
  
  Geometry::Vec3D beamCentre;        ///< central axis with beam point
  Geometry::Vec3D plateCentre;       ///< central point of plate

  /// Norminal line to get screen centre 
  Geometry::Line beamAxis;

  void calcImpactVector();

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeamScrapper(const std::string&);
  BeamScrapper(const BeamScrapper&);
  BeamScrapper& operator=(const BeamScrapper&);
  virtual ~BeamScrapper();

  void setBeamAxis(const attachSystem::FixedComp&,const long int);
  void setBeamAxis(const Geometry::Vec3D&,const Geometry::Vec3D&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
