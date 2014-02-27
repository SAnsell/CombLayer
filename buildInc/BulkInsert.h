/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/BulkInsert.h
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
#ifndef shutterSystem_BulkInsert_h
#define shutterSystem_BulkInsert_h

class Simulation;

namespace shutterSystem
{
  class GeneralShutter;
  
  /*!
    \class BulkInsert
    \version 1.0
    \author S. Ansell
    \date February 2011
    \brief Holds a simple basic ISIS shutter
    
    Can be specialized to ChipIR shutter or other
    Improved to not require shutter system
*/


class BulkInsert : public attachSystem::TwinComp,
    public attachSystem::ContainedGroup
{
 protected:

  const size_t shutterNumber;         ///< number of the shutter
  const int surfIndex;                ///< Index of the surface
  int cellIndex;                      ///< Index of the cells

  int populated;                      ///< populated / not

  int divideSurf;                     ///< Divider Number
  Geometry::Plane* DPlane;            ///< Divided plane [if set]

  double xyAngle;                     ///< Angle of beamline
  double innerRadius;                 ///< Inner radius
  double midRadius;                   ///< Mid radius
  double outerRadius;                 ///< Outer radius


  double zOffset;                     ///< Vertical offset
  double innerHeight;                 ///< Total height 
  double innerWidth;                  ///< Total depth
  double outerHeight;                 ///< Total height 
  double outerWidth;                  ///< Outer Width
  
  // Cells:
  int innerCell;                ///< Inner Steel section [BulkShield]
  int outerCell;                ///< Outer Steel section [BulkShield]

  int innerVoid;                ///< Inner void section [Created]
  int outerVoid;                ///< Outer void section [Created]

  std::string innerInclude;     ///< Include for the inner layer
  std::string outerInclude;     ///< Include for the outer layer
  //--------------
  // FUNCTIONS:
  //--------------

  void populate(const Simulation&,
		const shutterSystem::GeneralShutter&);
  void createUnitVector(const shutterSystem::GeneralShutter&);
  void createSurfaces();
  void createObjects(Simulation&);
  std::string divideStr() const;

  void createLinks();

 public:

  BulkInsert(const size_t,const std::string&);
  BulkInsert(const BulkInsert&);
  BulkInsert& operator=(const BulkInsert&);
  virtual ~BulkInsert();

  /// Access shutter number
  size_t getShutterNumber() const { return shutterNumber; }

  /// Access plane
  const Geometry::Plane* getDPlane() const { return DPlane; }
  /// Set inner/outer cells for exclusion
  void setLayers(const int A,const int B)
    { innerCell=A;outerCell=B; }
  void setExternal(const int,const int,const int);
  void setGlobalVariables(const double,const double,const double);

  /// Get innerVoid cell
  int getInnerVoid() const { return innerVoid; }  
  /// Get outerVoid cell
  int getOuterVoid() const { return outerVoid; }  
  /// Accessor to include
  const std::string& getInnerInc() const { return innerInclude; }
  /// Accessor to include
  const std::string& getOuterInc() const { return outerInclude; }

  virtual void createAll(Simulation&,const shutterSystem::GeneralShutter&);

};

}

#endif
 
