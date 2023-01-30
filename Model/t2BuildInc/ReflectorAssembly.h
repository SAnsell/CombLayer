/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2BuildInc/ReflectorAssembly.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef moderatorSystem_ReflectorAssembly_h
#define moderatorSystem_ReflectorAssembly_h

class Simulation;

namespace TMRSystem
{
  class TS2target;
  class TS2ModifyTarget;
}

namespace moderatorSystem
{
  class Reflector;
  class Groove;
  class Hydrogen;
  class VacVessel;
  class PreMod;
  class HPreMod;
  class FlightLine;
  class HWrapper;
  class Bucket;
  class RefCutOut;
  
/*!
  \class ReflectorAssembly
  \version 1.0
  \author S. Ansell
  \date August 2022
  \brief Constructs the TS2 reflector and all inner components
*/

class ReflectorAssembly :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp
{
 private:

  std::string decType;                     ///< Type of decoupled mod.
  std::string decFile;                     ///< File if used.
  bool orthoHFlag;                         ///< Add orthoH to coupled
  bool boltFlag;                           ///< Add Reflector bolts
  
  std::shared_ptr<TMRSystem::TS2target> TarObj;  ///< Target object

  std::shared_ptr<Groove> GrooveObj;        ///< Groove Moderator
  std::shared_ptr<Hydrogen> HydObj;         ///< Hydrogen moderator
  std::shared_ptr<VacVessel> VacObj;        ///< Vac Vessel round G/H
  std::shared_ptr<PreMod> PMgroove;         ///< Groove Pre mod
  std::shared_ptr<HPreMod> PMhydro;          ///< Hydro Pre mod
  std::shared_ptr<HWrapper> Horn;           ///< Hydro Horn wrapper

  std::shared_ptr<attachSystem::FixedComp> DMod;  ///< Decoupled moderator
  std::shared_ptr<VacVessel> DVacObj;       ///< Vac Vessel round 
  std::shared_ptr<PreMod> PMdec;            ///< Pre-moderator for decoupled
  std::shared_ptr<RefCutOut> IRcut;         ///< ReflectorAssembly cut out for chipIR
  std::shared_ptr<Bucket> CdBucket;         ///< Cd Bucket
  std::shared_ptr<Reflector> RefObj;        ///< Reflector

  void createObjects(Simulation&);
  void processDecoupled(Simulation&);

 public:

  ReflectorAssembly(const std::string&);
  ReflectorAssembly(const ReflectorAssembly&);
  ReflectorAssembly& operator=(const ReflectorAssembly&);
  virtual ~ReflectorAssembly();

  void insertPipeObjects(Simulation&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);


};

}

#endif
 
