/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/B4CCollimator.h
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
#ifndef photonSystem_B4CCollimator_h
#define photonSystem_B4CCollimator_h

class Simulation;

namespace photonSystem
{
  
/*!
  \class B4CCollimator
  \author S. Ansell
  \version 1.0
  \date July 2015
  \brief Simple B4c Round collimator with rectangular apperature
*/

class B4CCollimator : public attachSystem::ContainedComp,
   public attachSystem::FixedOffset
{
 private:

  const int colIndex;         ///< Index of surface offset
  int cellIndex;              ///< Cell index

  double radius;              ///< Outer volume
  double length;              ///< Length of object

  double viewWidth;           ///< Inner width
  double viewHeight;          ///< Inner Height
  
  int outerMat;               ///< Outer material

  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  B4CCollimator(const std::string&);
  B4CCollimator(const B4CCollimator&);
  B4CCollimator& operator=(const B4CCollimator&);
  virtual ~B4CCollimator();
  virtual B4CCollimator* clone() const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
