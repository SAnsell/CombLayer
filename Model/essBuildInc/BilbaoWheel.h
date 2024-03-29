/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/BilbaoWheel.h
 *
 * Copyright (c) 2015-2022 Konstantin Batkov
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
#ifndef essSystem_BilbaoWheel_h
#define essSystem_BilbaoWheel_h

class Simulation;

namespace essSystem
{

/*!
  \class BilbaoWheel
  \author Konstantin Batkov
  \version 2.0
  \date September 2015
  \brief Bilbao-type wheel for ESS
*/

class BilbaoWheel : public WheelBase
{
 private:

  double targetHeight;           ///< Total height of target
  double targetInnerHeight;      ///< Inner height of target wheel (R<Tungsten)
  double targetInnerHeightRadius; ///< Radius of the inner height of target wheel (R<Tungsten)
  double voidTungstenThick;      ///< Void thickness below/above Tungsten
  double steelTungstenThick;     ///< Steel thickness below/above Tungsten
  double steelTungstenInnerThick; ///< Steel thickness below/above Tungsten in the inner part
  double coolantThick;           ///< Thickness of coolant (outer wheel)
  double caseThick;              ///< Case Thickness
  double voidThick;              ///< void surrounding thickness

  double innerRadius;            ///< Inner core
  double innerHoleHeight;        ///< Vent hole height at the inner radius
  double innerHoleSize;          ///< Relative angular size of the hole with respect to hole+steel (<1)
  double innerHoleXYangle;       ///< XY angle offset of inner holes
  double coolantRadiusIn;        ///< Inner coolant radius
  double coolantRadiusOut;       ///< Outer coolant radius
  double caseRadius;             ///< Outer case radius
  double voidRadius;             ///< Final outer radius
  double aspectRatio;            ///< Defines curvature in the yz view

  double mainTemp;                  ///< Main temperature

  size_t nSectors;                  ///< number of sectors for LayerDivide3D
  size_t nLayers;                   ///< number of radial layers
  std::vector<double> radius;       ///< cylinder radii
  std::vector<int> matTYPE;         ///< Material type

  // shaft
  double shaftHeight;               ///< Shaft Height (above origin)
  size_t nShaftLayers;              ///< Number of shaft layers
  std::vector<double> shaftRadius;  ///< shaft radii
  std::vector<int> shaftMat;        ///< shaft materials
  double shaft2StepHeight;          ///< height of the 2nd step
  double shaft2StepConnectionHeight;///< height of the 2nd step connection thickness
  double shaft2StepConnectionDist;  ///< vertical distance of the 2nd step connection with shaft
  double shaft2StepConnectionRadius;///< radius of the 2nd step connection with shaft
  double shaftCFRingHeight; ///< connection flange ring height
  double shaftCFRingRadius; ///< connection flange ring radius
  double shaftUpperBigStiffLength; ///< connection flange stiffener length
  double shaftUpperBigStiffHeight; ///< connection flange stiffener height
  double shaftUpperBigStiffThick;  ///< connection flange stiffener thickness
  /// Homogenised material of the upper large stiffener cell (used if engActive=0)
  int shaftUpperBigStiffHomoMat;
  double shaftLowerBigStiffShortLength; ///< lower big stiffener short (=lower) length
  double shaftLowerBigStiffLongLength; ///< lower big stiffeners long (=upper) length
  double shaftLowerBigStiffHeight; ///< lower big stiffeners height
  double shaftLowerBigStiffThick; ///< lower big stiffeners thickness
  /// Homogenised material of the lower large stiffener cell (used if engActive=0)
  int shaftLowerBigStiffHomoMat;

  double shaftHoleHeight;        ///< Vent hole height at the shaft radius
  double shaftHoleSize;          ///< Relative angular size of the hole with respect to hole+steel (<1)
  double shaftHoleXYangle;       ///< XY angle offset of shaft holes
  double shaftBaseDepth;            ///< shaft depth (below origin)

  double catcherTopSteelThick; ///< thickness of top steel plate
  double catcherRadius;    ///< catcher rotal radius
  double catcherBaseHeight;///< catcher base truncated cone height
  double catcherBaseRadius;///< catchr base truncated cone lower radius
  double catcherBaseAngle; ///< catcher base truncated cone angle
  double catcherNotchRadius; ///< catcher notch radius
  double catcherNotchBaseThick; ///< catcher notch lower non-inclined part vertical thickness

  double circlePipesBigRad;   /// Big radius of circle of pipes]
  double circlePipesRad;      /// Radius of pipes in the circle of pipes
  double circlePipesWallThick; /// Thickness of pipes in the circle of pipes

  int homoWMat;                     ///< homogenized W material
  int heMat;                        ///< He material
  int steelMat;                     ///< Steel mat
  int ssVoidMat;                    ///< Mixture of SS316L and void

  int innerMat;                     ///< Inner Material block

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createShaftSurfaces();
  void createShaftObjects(Simulation&);
  std::string getSQSurface(const double,const double);

  void createRadialSurfaces(const int,const size_t,const double w=0.0);
  void divideRadial(Simulation&,const HeadRule&,const int);
  void buildStiffeners(Simulation&,const HeadRule,const int);
  void buildHoles(Simulation&,
		  const HeadRule&,const HeadRule&,const HeadRule&,
		  const int, const double, const double,const double,
		  const double,const int);

  void buildCirclePipes(Simulation&,const HeadRule&,
			const HeadRule&,const HeadRule&,
			const HeadRule&,const HeadRule&);
  void buildSectors(Simulation&) const;

  public:

  BilbaoWheel(const std::string&);
  BilbaoWheel(const BilbaoWheel&);
  BilbaoWheel& operator=(const BilbaoWheel&);
  BilbaoWheel* clone() const override;
  ~BilbaoWheel() override;

  /// total wheel void size
  double wheelHeight() const override
  {
    return targetHeight+
      2.0*(voidTungstenThick+steelTungstenThick+coolantThick+
	   caseThick+voidThick);
  }

  using FixedComp::createAll;
  void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif

