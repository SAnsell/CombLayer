/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/SegWheel.h
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
#ifndef essSystem_SegWheel_h
#define essSystem_SegWheel_h

class Simulation;

namespace essSystem
{

/*!
  \class SegWheel
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder object
*/

class SegWheel : public WheelBase
{
 private:
  
  int lh2Index;
  int mainShaftCell;

  double xStep;                   ///< X step
  double yStep;                   ///< y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy angle
  double zAngle;                  ///< zAngle step
  
  double targetHeight;           ///< Total height of target
  double targetSectorOffsetX;
  double targetSectorOffsetY;
  double targetSectorOffsetZ;
  double targetSectorAngleXY;
  double targetSectorAngleZ;  
  double targetSectorApertureXY;  
  double targetSectorApertureZ;
  double targetSectorNumber;

  double coolantThickOut;           ///< Outer Thickness of coolant
  double coolantThickIn;           ///< Inner Thickness of coolant 

  double caseThickZ;              ///< Case Thickness
  double caseThickX;              ///< Case Thickness between sectors

  double voidThick;              ///< void surrounding thickness
  
  double innerRadius;            ///< Inner core
  double coolantRadiusOut;          ///< Outer coolant radius
  double coolantRadiusIn;          ///< Outer coolant radius 
  double caseRadius;             ///< Outer case radius
  double voidRadius;             ///< Final outer radius

  size_t nLayers;                ///< number of layers
  std::vector<double> radius;    ///< cylinder radii
  std::vector<int> matTYPE;      ///< Material type
 
  double shaftTopHeight;               ///< Shaft Height (above origin)
  double shaftBaseHeight;               ///< Shaft Height (below origin)

  // innerRadius is starting shaft radius!
  // double shaftRadius;               ///< Main shaft radius
  double shaftJointThick;            ///< coolant 
  double shaftCoolThick;            ///< coolant 
  double shaftCladThick;            ///< cladding
  double shaftVoidThick;            ///< void, but it's cooling ext 
  double shaftSupportRadius;         // between target and inner radius
  double shaftSupportThick;         // between target and inner radius
  double shaftBaseThick;            // steel bottom layer 
  double shaftBaseFootThick;        // where shaft ends at the bottom

  int wMat;                      ///< W material
  int heMat;                     ///< He material
  int steelMat;                  ///< Steel mat
  
  int innerMat;                     ///< Inner Material block
  int mainShaftMat;                 ///< Main shaft material
  int cladShaftMat;                 ///< Cladding on shaft;
  int coolingShaftMatInt;
  int coolingShaftMatExt;

  // std::string wheelQuarter[4];

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void makeShaftSurfaces();
  void makeShaftObjects(Simulation&);

  public:

  SegWheel(const std::string&);
  SegWheel(const SegWheel&);
  SegWheel& operator=(const SegWheel&);
  virtual SegWheel* clone() const;
  virtual ~SegWheel();

  int getCell() const { return mainShaftCell; }
  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 

