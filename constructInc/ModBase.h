/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/ModBase.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef constructSystem_ModBase_h
#define constructSystem_ModBase_h

class Simulation;

namespace constructSystem
{

/*!
  \class ModBase
  \version 1.0
  \author S. Ansell
  \date July 2013
  \brief General ModBase unit
*/

class ModBase : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 protected:
  
  const int modIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy rotation angle
  double zAngle;                  ///< z rotation angle
  
 public:

  ModBase(const std::string&,const size_t);
  ModBase(const ModBase&);
  ModBase& operator=(const ModBase&);
  virtual ModBase* clone() const =0;
  virtual ~ModBase();

  std::string getComposite(const std::string&) const;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&) =0;

};

}

#endif
 
