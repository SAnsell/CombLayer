/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/MonoPlug.h
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
#ifndef ShutterSystem_MonoPlug_h
#define ShutterSystem_MonoPlug_h

class Simulation;

namespace shutterSystem
{

/*!
  \class MonoPlug
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for for the ts1 mono [top plug]
*/

class MonoPlug : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int plugIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t nPlugs;                   
  std::vector<double> plugRadii;   ///< Inner plug radii [base to top]
  std::vector<double> plugZLen;    ///< Inner plug lengths [Base to top]
  double plugClearance;            ///< clearance
  double dividerZLen;              ///< Divide distance

  int steelMat;                    ///< Inner material
  int concMat;                     ///< Wall material

  // Functions:

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&,const long int,
		     const attachSystem::FixedComp&,
		     const attachSystem::FixedComp&);
  void createLinks();

 public:

  MonoPlug(const std::string&);
  MonoPlug(const MonoPlug&);
  MonoPlug& operator=(const MonoPlug&);
  virtual ~MonoPlug();

  void createAll(Simulation&,const long int,
		 const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&);
  
};

}

#endif
 
