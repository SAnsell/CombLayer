/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/BulkInsert.h
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
    \brief Simple void after the ISIS shutter
*/


class BulkInsert :
  public attachSystem::FixedGroup,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 protected:

  const std::string baseName;         ///< Base name
  const size_t shutterNumber;         ///< number of the shutter

  double zOffset;                     ///< Vertical offset
  double innerHeight;                 ///< Total height 
  double innerWidth;                  ///< Total depth
  double outerHeight;                 ///< Total height 
  double outerWidth;                  ///< Outer Width

  int innerMat;                       ///< Fill in material
  int outerMat;                       ///< Fill in material

  int impZero;                        ///< Make importance zero
  
  // Cells:
  int innerCell;                ///< Inner Steel section [BulkShield]
  int outerCell;                ///< Outer Steel section [BulkShield]
  
  std::string innerInclude;     ///< Include for the inner layer
  std::string outerInclude;     ///< Include for the outer layer
  //--------------
  // FUNCTIONS:
  //--------------

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);

  void createUnitVector(const FixedComp&,const long int);
  void createLinks();

 public:

  BulkInsert(const size_t,const std::string&);
  BulkInsert(const BulkInsert&);
  BulkInsert& operator=(const BulkInsert&);
  virtual ~BulkInsert();

  /// Access shutter number
  size_t getShutterNumber() const { return shutterNumber; }

  /// Set inner/outer cells for exclusion
  void setLayers(const int A,const int B)
    { innerCell=A;outerCell=B; }
  void setExternal(const int,const int,const int);

  /// Accessor to include
  const std::string& getInnerInc() const { return innerInclude; }
  /// Accessor to include
  const std::string& getOuterInc() const { return outerInclude; }

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
