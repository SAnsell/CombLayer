/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BilbaoWheel.h
 *
 * Copyright (c) 2015 Konstantin Batkov
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
  \version 1.0
  \date September 2015
  \brief Bilbao-type wheel for ESS
*/

class BilbaoWheelInnerStructure;

class BilbaoWheel : public WheelBase,
  public attachSystem::CellMap
{
 private:
  
  double xStep;                   ///< X step
  double yStep;                   ///< y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy angle
  double zAngle;                  ///< zAngle step

  int engActive;                 ///< Engineering active flag
  std::shared_ptr<BilbaoWheelInnerStructure> InnerComp; ///< Inner components
  
  double targetHeight;           ///< Total height of target
  double voidTungstenThick;      ///< Void thickness below/above Tungsten
  double steelTungstenThick;     ///< Steel thickness below/above Tungsten
  double caseThickIn;            ///< Thickness of coolant (inner wheel)
  double coolantThick;           ///< Thickness of coolant (outer wheel)
  double caseThick;              ///< Case Thickness
  double voidThick;              ///< void surrounding thickness
  
  double innerRadius;            ///< Inner core
  double coolantRadiusIn;        ///< Inner coolant radius
  double coolantRadiusOut;       ///< Outer coolant radius
  double caseRadius;             ///< Outer case radius
  double voidRadius;             ///< Final outer radius
  double aspectRatio;            ///< Defines curvature in the yz view

  double mainTemp;               ///< Main temperature 
  
  size_t nSectors;               ///< number of sectors for LayerDivide3D
  size_t nLayers;                ///< number of radial layers
  std::vector<double> radius;    ///< cylinder radii
  std::vector<int> matTYPE;      ///< Material type

  // shaft
  double shaftHeight;               ///< Shaft Height (above origin)
  size_t nShaftLayers;           ///< Number of shaft layers
  std::vector<double> shaftRadius; ///< shaft radii
  std::vector<int> shaftMat;     ///< shaft materials
  
  int wMat;                         ///< W material
  int heMat;                        ///< He material
  int steelMat;                     ///< Steel mat
  int ssVoidMat;                    ///< Mixture of SS316L and void (for the simplified version)
  
  int innerMat;                     ///< Inner Material block

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void makeShaftSurfaces();
  void makeShaftObjects(Simulation&);
  std::string getSQSurface(const double R, const double e);

  void createRadialSurfaces();
  void divideRadial(Simulation&, std::string&, int);

  public:

  BilbaoWheel(const std::string&);
  BilbaoWheel(const BilbaoWheel&);
  BilbaoWheel& operator=(const BilbaoWheel&);
  virtual BilbaoWheel* clone() const;
  virtual ~BilbaoWheel();

  /// total wheel void size
  virtual double wheelHeight() const
  { return targetHeight+
      2.0*(voidTungstenThick+steelTungstenThick+coolantThick+caseThickIn+voidThick); }

  //  virtual int getCell() const { return mainShaftCell; }
  virtual void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
