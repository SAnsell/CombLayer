/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModelInc/layers.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef LensSource_layers_h
#define LensSource_layers_h

class Simulation;

namespace lensSystem
{

class siModerator;

/*!
  \class layers
  \version 1.0
  \author S. Ansell
  \date November 2009
  \brief Layers surrounding the LENS moderator space
*/

 class layers :public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int surIndex;           ///< Index of surface offset
  int cellIndex;                ///< Index of surface offset
  int populated;                ///< populated or not
  int innerCompSurf;            ///< InnerCompSurf;

  ProtonFlight PA;              ///< Proton flight line
  FlightCluster FC;             ///< Outgoing Flight Cluster

  // Master radii
  double waterRad;            ///< Water radius
  double DCRad;               ///< Decoupler radius
  double leadRad;             ///< lead
  double bPolyRad;            ///< Boral Poly Radius
  double epoxyRad;            ///< epoxyRad
  double outPolyRad;          ///< outPolyRad
  double outVoidRad;          ///< outer void radius [for tallies]

  double waterAlThick;        ///< Base Width (beamdirection)

  double waterHeight;         ///< Height of the water [no Al]
  double waterDepth;          ///< Depth of the water [no Al]

  double DCHeight;            ///< Height of the inner decoupler
  double DCDepth;             ///< Depth of the inner decoupler

  double leadHeight;          ///< A-Height
  double leadDepth;           ///< A-Depth

  double bPolyHeight;         ///< Height of the boral-poly
  double bPolyDepth;          ///< Depth of the boral-poly

  double epoxyHeight;         ///< epoxy+lead-Height
  double epoxyDepth;          ///< epoyx+lead-Depth

  double outPolyHeight;       ///< outerPoly-Height
  double outPolyDepth;        ///< outerPoly-Depth

  double wedgeWidth;          ///< Width of the wedge
  double wedgeHeight;         ///< Height
  double wedgeAngleXY[2];     ///< Angle
  
  size_t nWedge;                   ///< number of wedge units
  std::vector<double> wedgeLiner;  ///< Thicknesses for liners
  std::vector<int> wedgeLinerMat;  ///< Materials

  int waterMat;               ///< Material for support
  int alMat;                  ///< Material for Al surround
  int DCMat;                  ///< Material for Decoupler
  int leadMat;                ///< Material for lead layer
  int bPolyMat;               ///< Material for boron-poly
  int epoxyMat;               ///< Material for epoxy
  int outPolyMat;             ///< Material for outer layer

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&,const candleStick&);
  void constructFlightLines(Simulation&,const attachSystem::FixedComp&);
  void createLinks(const attachSystem::FixedComp&);

 public:

  layers(const std::string&);
  layers(const layers&);
  layers& operator=(const layers&);
  ~layers() {}  ///< Destructor

  /// Accessor to Proton flight linex
  const ProtonFlight& getPF() const { return PA; }
  /// Set inner valued
  void setInnerComp(const int N) { innerCompSurf=N; }
  void createAll(Simulation&,const candleStick&);

  /// Access flight cluster
  const FlightCluster& getFC() const { return FC; }

};

}


#endif
 
