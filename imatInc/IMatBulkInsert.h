/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/IMatBulkInsert.h
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
#ifndef ShutterSystem_IMatBulkInsert_h
#define ShutterSystem_IMatBulkInsert_h

class Simulation;

namespace shutterSystem
{
  class shutterInfo;
  class GeneralShutter;

/*!
  \class IMatBulkInsert
  \author S. Ansell
  \version 1.0
  \date October 2009
  \brief Specialized for for the IMatBulkInsert
*/

class IMatBulkInsert : public BulkInsert
{
 private:
  
  const std::string keyName;  ///< KeyName
  const int insIndex;         ///< Insert element
  int cellIndex;              ///< Cell index

  double xStep;              ///< X-Step
  double yStep;              ///< X-Step
  double zStep;              ///< X-Step
  double xyAngle;            ///< XY angle rotation
  double zAngle;             ///< Z angle rotation

  double frontGap;           ///< Distance to miss at the front
  double width;              ///< Main width
  double height;             ///< Main height

  int defMat;                ///< Default material
  
  // Function:

  void populate(const Simulation&);
  void createUnitVector();

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  IMatBulkInsert(const size_t,const std::string&,const std::string&);
  IMatBulkInsert(const IMatBulkInsert&);
  IMatBulkInsert& operator=(const IMatBulkInsert&);
  virtual ~IMatBulkInsert();

  void createAll(Simulation&,const shutterSystem::GeneralShutter&);
  
};

}

#endif
 
