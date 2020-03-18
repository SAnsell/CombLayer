/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/makeReflector.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef moderatorSystem_makeReflector_h
#define moderatorSystem_makeReflector_h

class Simulation;

namespace TMRSystem
{
  class TS2target;
  class TS2ModifyTarget;
}

namespace moderatorSystem
{
  class Groove;
  class Hydrogen;
  class VacVessel;
  class PreMod;
  class FlightLine;
  class HWrapper;
  class Bucket;
  class RefCutOut;
  
/*!
  \class makeReflector
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief makeReflector [insert object]
*/

class makeReflector 
{
 private:
  
  std::shared_ptr<TMRSystem::TS2target> TarObj;  ///< Target object

  // TO BE MOVED INTO COUPLED OBJECT:
  std::shared_ptr<Groove> GrooveObj;        ///< Groove Moderator
  std::shared_ptr<Hydrogen> HydObj;         ///< Hydrogen moderator
  std::shared_ptr<VacVessel> VacObj;        ///< Vac Vessel round G/H
  std::shared_ptr<FlightLine> FLgroove;     ///< Groove flight line 
  std::shared_ptr<FlightLine> FLhydro;      ///< Hydrogen flight line
  std::shared_ptr<PreMod> PMgroove;         ///< Groove Pre mod
  std::shared_ptr<PreMod> PMhydro;          ///< Hydro Pre mod
  std::shared_ptr<HWrapper> Horn;           ///< Hydro Horn wrapper

  std::shared_ptr<attachSystem::FixedComp> DMod;  ///< Decoupled moderator
  std::shared_ptr<VacVessel> DVacObj;       ///< Vac Vessel round 
  std::shared_ptr<FlightLine> FLwish;       ///< Wish [broad] flight line
  std::shared_ptr<FlightLine> FLnarrow;     ///< Narrow flight line
  std::shared_ptr<PreMod> PMdec;            ///< Pre-moderator for decoupled
  std::shared_ptr<RefCutOut> IRcut;         ///< makeReflector cut out for chipIR
  std::shared_ptr<Bucket> CdBucket;         ///< Cd Bucket
  std::shared_ptr<Reflector> RefObj;        ///< Reflector

  void createObjects(Simulation&);
  void createInternalObjects(Simulation&,const mainSystem::inputParam&);
  void processDecoupled(Simulation&,const mainSystem::inputParam&);
  void setTarget(const mainSystem::inputParam&);


 public:

  makeReflector();
  makeReflector(const makeReflector&);
  makeReflector& operator=(const makeReflector&);
  virtual ~makeReflector();

  int calcModeratorPlanes(const int,std::vector<int>&,int&) const;
  Geometry::Vec3D getViewOrigin(const int) const;
    
  void insertPipeObjects(Simulation&,const mainSystem::inputParam&);

  virtual std::string getExclude() const;

  void build(Simulation&,const mainSystem::inputParam&,int&);


};

}

#endif
 
