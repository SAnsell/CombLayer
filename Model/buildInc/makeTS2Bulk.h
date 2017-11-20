/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/makeTS2Bulk.h
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
#ifndef moderatorSystem_makeTS2Bulk_h
#define moderatorSystem_makeTS2Bulk_h

namespace shutterSystem
{
  class BulkShield;
  class VoidVessel;
}

namespace moderatorSystem
{
  class Reflector;

  /*!
    \class makeTS2Bulk
    \version 1.0
    \author S. Ansell
    \date July 2012
    \brief General constructor for TS2 void+bulk system
  */

class makeTS2Bulk
{
 private:

  std::shared_ptr<moderatorSystem::Reflector> RefObj;  ///< Reflector object  
  std::shared_ptr<shutterSystem::VoidVessel> VObj;     ///< Void vessel
  std::shared_ptr<shutterSystem::BulkShield> BulkObj;  ///< Bulk shield
  
 public:
  
  makeTS2Bulk();
  makeTS2Bulk(const makeTS2Bulk&);
  makeTS2Bulk& operator=(const makeTS2Bulk&);
  ~makeTS2Bulk();
  
  void build(Simulation*,const mainSystem::inputParam&);
  void setSource(Simulation*,const mainSystem::inputParam&);
  

  /// Accessor to Reflectro
  const moderatorSystem::Reflector* getReflector() const 
    { return RefObj.get(); } 
  /// Accessor to bulk object 
  const shutterSystem::BulkShield* getBulkShield() const 
    { return BulkObj.get(); } 

  void insertPipeObjects(Simulation*,const mainSystem::inputParam&);

};

}

#endif
