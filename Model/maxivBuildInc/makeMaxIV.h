/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuildInc/makeMaxIV.h
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
#ifndef xraySystem_makeMaxIV_h
#define xraySystem_makeMaxIV_h


namespace xraySystem
{
  class InjectionHall;
  class R1Ring;
  class R3Ring;
  /*!
    \class makeMaxIV
    \version 1.0
    \author S. Ansell
    \date May 2018
    \brief Main beamline system for MaxIV
  */
  
class makeMaxIV
{
 private:

  /// Main R1 Ring
  std::shared_ptr<R1Ring> r1Ring;
  /// Main R1 Ring
  std::shared_ptr<R3Ring> r3Ring;
  /// Linac/SPF Hall
  std::shared_ptr<InjectionHall> injectionHall;

  
  void populateStopPoint(const mainSystem::inputParam&,
			 const std::set<std::string>&,
			 std::map<std::string,std::string>&) const;
  std::string getActiveStop(const std::map<std::string,std::string>&,
			    const std::string&) const;
    
  bool buildR1Ring(Simulation&,const mainSystem::inputParam&);

  bool buildR3Ring(Simulation&,const mainSystem::inputParam&);

  bool buildInjection(Simulation&,const mainSystem::inputParam&);
  
 public:
  
  makeMaxIV();
  makeMaxIV(const makeMaxIV&);
  makeMaxIV& operator=(const makeMaxIV&);
  ~makeMaxIV();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
