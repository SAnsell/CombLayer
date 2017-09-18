/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1EngineerInc/bulletTarget.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Goran Skoro
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
#ifndef ts1System_bulletTarget_h
#define ts1System_bulletTarget_h

class Simulation;

namespace ts1System
{

  class BulletVessel;
  class BulletPlates;
  class BulletDivider;

/*!
  \class BulletTarget
  \version 1.0
  \author S. Ansell
  \date February 2015
  \brief BulletTarget [insert object]
*/

class BulletTarget : 
  public constructSystem::TargetBase
{
 private:

  const int tIndex;                                  ///< Base number
  std::shared_ptr<BulletPlates> PlateTarObj;          ///< Plate Target
  std::shared_ptr<BulletVessel> PressVObj;            ///< Pressure vessel
  std::vector<std::shared_ptr<BulletDivider> > DObj;  ///< Internal dividers

 public:

  BulletTarget(const std::string&);
  BulletTarget(const BulletTarget&);
  BulletTarget& operator=(const BulletTarget&);
  virtual ~BulletTarget();
  virtual BulletTarget* clone() const;
  

  void addProtonLine(Simulation&,const attachSystem::FixedComp&,
		     const long int);

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
