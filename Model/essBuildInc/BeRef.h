/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BeRef.h
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
#ifndef essSystem_BeRef_h
#define essSystem_BeRef_h

class Simulation;

namespace essSystem
{
  class BeRefInnerStructure;

/*!
  \class BeRef
  \author S. Ansell
  \version 1.0
  \date February 2013
  \brief Reflector object 
*/

class BeRef : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int refIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  int engActive;                  ///< Engineering active flag
  /// Inner components inside Reflector (eng detail)
  std::shared_ptr<BeRefInnerStructure> InnerComp;   

  double radius;                  ///< Radius
  double height;                  ///< Height
  double wallThick;               ///< Wall thickness
  double wallThickLow;            ///< Wall thickness of the side near the target wheel. Separated from wallThick in order to optimise wrapping with CapMod

  double lowVoidThick;            ///< Low void segment
  double topVoidThick;            ///< Top void segment

  double targSepThick;            ///< Steel seperator at target level

  int topRefMat;                  ///< reflector material (upper Be tier)
  int lowRefMat;                  ///< reflector material (lower Be tier)
  int topWallMat;                 ///< wall Material (upper tier) 
  int lowWallMat;                 ///< wall Material (lower tier)
  int targSepMat;                 ///< Separator Mat
  
  // Functions:

  void populateWithDef(const FuncDataBase&,const double,const double,
		       const double);
  
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeRef(const std::string&);
  BeRef(const BeRef&);
  BeRef& operator=(const BeRef&);
  virtual ~BeRef();

  void globalPopulate(const FuncDataBase&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const double,
		 const double,const double);
  
  /// Access to radius
  double getRadius() const { return radius+wallThick; }
};

}

#endif
 
