/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/SqrCollimator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_SqrCollimator_h
#define xraySystem_SqrCollimator_h

class Simulation;

namespace xraySystem
{
  /*!
    \class SqrCollimator
    \version 1.0
    \author S. Ansell
    \date June 2015
    \brief Variable detemine hole type
  */
  
class SqrCollimator :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< Main radius    
  double length;                ///< thickness of collimator
  
  double innerAWidth;           ///< front width
  double innerAHeight;          ///< front height
  
  double minLength;               ///< point of min closure
  double innerMinWidth;           ///< min width at closure
  double innerMinHeight;          ///< min height at closure

  double innerBWidth;           ///< back width
  double innerBHeight;          ///< back height 
  
  int mat;                      ///< material
  int voidMat;                  ///< inner material
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  SqrCollimator(const std::string&);
  SqrCollimator(const SqrCollimator&);
  SqrCollimator& operator=(const SqrCollimator&);
  virtual ~SqrCollimator() {}  ///< Destructor

  void populate(const FuncDataBase&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
