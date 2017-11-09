/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/Wheel.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_Wheel_h
#define essSystem_Wheel_h

class Simulation;

namespace essSystem
{

/*!
  \class Wheel
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder object
*/

class Wheel : public WheelBase
{
 private:
    
  double targetHeight;           ///< Total height of target
  double coolantThickIn;         ///< Thickness of coolant (inner wheel)
  double coolantThickOut;        ///< Thickness of coolant (outer wheel)
  double caseThick;              ///< Case Thickness
  double voidThick;              ///< void surrounding thickness
  
  double innerRadius;            ///< Inner core
  double coolantRadiusIn;        ///< Inner coolant radius
  double coolantRadiusOut;       ///< Outer coolant radius
  double caseRadius;             ///< Outer case radius
  double voidRadius;             ///< Final outer radius

  double mainTemp;               ///< Main temperature 
  
  size_t nLayers;                ///< number of layers
  std::vector<double> radius;    ///< cylinder radii
  std::vector<int> matTYPE;      ///< Material type

  double shaftRadius;               ///< Main shaft radius
  double shaftCoolThick;            ///< coolant 
  double shaftCladThick;            ///< cladding
  double shaftVoidThick;            ///< void
  double shaftHeight;               ///< Shaft Height (above origin)
  
  int wMat;                         ///< W material
  int heMat;                        ///< He material
  int steelMat;                     ///< Steel mat
  
  int innerMat;                     ///< Inner Material block
  int mainShaftMat;                 ///< Main shaft material
  int cladShaftMat;                 ///< Cladding on shaft;

  // Functions:

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void makeShaftSurfaces();
  void makeShaftObjects(Simulation&);

  public:

  Wheel(const std::string&);
  Wheel(const Wheel&);
  Wheel& operator=(const Wheel&);
  virtual Wheel* clone() const;
  virtual ~Wheel();

  /// total wheel void size
  virtual double wheelHeight() const
  { return targetHeight+
      2.0*(coolantThickIn+caseThick+voidThick); }

  //  virtual int getCell() const { return mainShaftCell; }
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  
};

}

#endif
 
