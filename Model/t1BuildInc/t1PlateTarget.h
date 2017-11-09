/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/t1PlateTarget.h
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
#ifndef moderatorSystem_t1PlateTarget_h
#define moderatorSystem_t1PlateTarget_h

class Simulation;

namespace ts1System
{

  class PressVessel;
  class PlateTarget;
  class WaterDividers;

/*!
  \class t1PlateTarget
  \version 1.0
  \author S. Ansell
  \date January 2013
  \brief t1PlateTarget [insert object]
*/

class t1PlateTarget : 
  public constructSystem::TargetBase
{
 private:

  const int tIndex;                                  ///< Base number
  std::shared_ptr<PressVessel> PressVObj;            ///< Pressure vessel
  std::shared_ptr<PlateTarget> PlateTarObj;          ///< Plate Target
  std::shared_ptr<WaterDividers> DivObj;             ///< Internal divider

 public:

  t1PlateTarget(const std::string&);
  t1PlateTarget(const t1PlateTarget&);
  t1PlateTarget& operator=(const t1PlateTarget&);
  virtual ~t1PlateTarget();
  virtual t1PlateTarget* clone() const;
  

  void addProtonLine(Simulation&,const attachSystem::FixedComp&,
		     const long int);
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
