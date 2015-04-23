/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/InsertObject.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef attachSystem_InsertObject_h
#define attachSystem_InsertObject_h

class Simulation;

namespace attachSystem
{
  
/*!
  \class InsertObject
  \version 1.0
  \author S. Ansell
  \date May 2010
  \brief InsertObject [insert object]
*/

class InsertObject : public ContainedComp,
    public FixedComp
{
 private:
  
  const int surfIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var

  double xStep;                 ///< Offset on X 
  double yStep;                 ///< Offset on Y 
  double zStep;                 ///< Offset on Z 
  double xyAngle;               ///< x-y Rotation angle
  double zAngle;                ///< z Rotation angle
  
  double xMinus;                ///< X-Minus distance
  double xPlus;                 ///< X-Plus distance
  double yMinus;                ///< Y-Minus distance                 
  double yPlus;                 ///< Y-Plus distance
  double zMinus;                ///< Z-Minus distance 
  double zPlus;                 ///< Z-Plus distance

  Geometry::Matrix<double> RBase;  ///< ReBase matrix

  void populate(const Simulation&);

  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects();

  int readFile(Simulation&,const std::string&);
  void reMapSurf(ReadFunc::OTYPE&) const;

 public:

  InsertObject(const std::string&);
  InsertObject(const InsertObject&);
  InsertObject& operator=(const InsertObject&);
  ~InsertObject();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const std::string&);

};


}  

#endif
 
