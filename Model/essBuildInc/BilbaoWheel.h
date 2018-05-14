/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BilbaoWheel.h
 *
 * Copyright (c) 2015-2017 Konstantin Batkov
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

class BilbaoWheelInnerStructure;

/*!
  \class BilbaoWheel
  \author Konstantin Batkov
  \version 1.0
  \date September 2015
  \brief Bilbao-type wheel for ESS
*/

class BilbaoWheelInnerStructure;

class BilbaoWheel : public WheelBase
{
 private:
  
  std::shared_ptr<BilbaoWheelInnerStructure> InnerComp; ///< Inner components
  
  double targetHeight;           ///< Total height of target
  double targetInnerHeight;      ///< Inner height of target wheel (R<Tungsten)
  double targetInnerHeightRadius; ///< Radius of the inner height of target wheel (R<Tungsten)
  double voidTungstenThick;      ///< Void thickness below/above Tungsten
  double steelTungstenThick;     ///< Steel thickness below/above Tungsten
  double steelTungstenInnerThick; ///< Steel thickness below/above Tungsten in the inner part
  double caseThickIn;            ///< Thickness of coolant (inner wheel)
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

  double shaftHoleHeight;        ///< Vent hole height at the shaft radius
  double shaftHoleSize;          ///< Relative angular size of the hole with respect to hole+steel (<1)
  double shaftHoleXYangle;       ///< XY angle offset of shaft holes

  
  double shaftBaseDepth;            ///< shaft depth (below origin)
  double catcherTopSteelThick; ///< thickness of top steel plate
  double catcherHeight;    ///< catcher total height
  double catcherRadius;    ///< catcher rotal radius
  double catcherMiddleHeight;///< catcher mid height
  double catcherMiddleRadius;///< catchr mid radius
  double catcherNotchDepth; ///< catcher notch depth
  double catcherNotchRadius; ///< catcher notch radius
  double catcherRingRadius; ///< catcher ring radius
  double catcherRingDepth; ///< catcher ring depth (below origin)
  double catcherRingThick; ///< catcher ring thickness

  double circlePipesBigRad;   /// Big radius of circle of pipes]
  double circlePipesRad;      /// Radius of pipes in the circle of pipes
  double circlePipesWallThick; /// Thickness of pipes in the circle of pipes
  
  int wMat;                         ///< W material
  int heMat;                        ///< He material
  int steelMat;                     ///< Steel mat
  int ssVoidMat;                    ///< Mixture of SS316L and void 
  
  int innerMat;                     ///< Inner Material block

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void makeShaftSurfaces();
  void makeShaftObjects(Simulation&);
  std::string getSQSurface(const double,const double);

  void createRadialSurfaces();
  void divideRadial(Simulation&,const std::string&,const int);

  void buildHoles(Simulation&,
		  const std::string&,const std::string&,const std::string&,
		  const int, const double, const double,const double,
		  const double,const int);

  void buildCirclePipes(Simulation&,const std::string&,const std::string&,
			const int);

  public:

  BilbaoWheel(const std::string&);
  BilbaoWheel(const BilbaoWheel&);
  BilbaoWheel& operator=(const BilbaoWheel&);
  virtual BilbaoWheel* clone() const;
  virtual ~BilbaoWheel();

  /// total wheel void size
  virtual double wheelHeight() const
  {
    return targetHeight+
      2.0*(voidTungstenThick+steelTungstenThick+coolantThick+
	   caseThick+voidThick);
  }

  //  virtual int getCell() const { return mainShaftCell; }
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);
  
};

}

#endif
 
