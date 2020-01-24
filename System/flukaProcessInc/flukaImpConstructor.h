/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcessInc/flukaImpConstructor.h
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
#ifndef physicsSystem_flukaImpConstructor_h
#define physicsSystem_flukaImpConstructor_h

///\file 

class Simulation;
namespace Geometry
{
  class Plane;
}

namespace mainSystem
{
  class inputParam;
}

namespace flukaSystem
{
  class flukaPhysics;
/*!
  \class flukaImpConstructor
  \brief Controls importance of cells in fluka
  \author S. Ansell
  \version 1.0
  \date March 2018
*/

class flukaImpConstructor 
{
 private:

  /// Typedef for ENDL type in help
  typedef ELog::OutputLog<ELog::EReport>& (*ENDL)
    (ELog::OutputLog<ELog::EReport>&);

  /// Typedef for tuple
  typedef std::tuple<size_t,int,std::string> impTYPE;

  void writeCUTHelp(std::ostream&,ENDL) const;

  void writeIMPHelp(std::ostream&,ENDL) const;

  void writeEMFHelp(std::ostream&,ENDL) const;

  void writeEXPHelp(std::ostream&,ENDL) const;

  void writeLAMHelp(std::ostream&,ENDL) const;

  void writeMATHelp(std::ostream&,ENDL) const;
  
  void insertCell(flukaPhysics&,const size_t,
		  const std::set<int>&,const std::string&,
		  const std::string*) const;
  void insertParticle(flukaPhysics&,const size_t,
		      const std::string&,const std::string&,
		      const std::string*) const;
  void insertPair(flukaPhysics&,const size_t,
		  const std::string&,const int,
		  const std::string&,const std::string*) const;
  
  void processGeneral(SimFLUKA&,
		      const mainSystem::inputParam&,
		      const size_t,const std::string&,
		      const impTYPE&) const;

 public:

  /// null constructor
  flukaImpConstructor() {}
  /// null copy constructor
  flukaImpConstructor(const flukaImpConstructor&) {}
  /// null assignment operator
  flukaImpConstructor& operator=(const flukaImpConstructor&) { return *this; }
  ~flukaImpConstructor() {}  ///< Destructor

  void processUnit(SimFLUKA&,
		   const mainSystem::inputParam&,
		   const size_t);
  void processCUT(SimFLUKA&,
		  const mainSystem::inputParam&,
		  const size_t);
  void processEXP(SimFLUKA&,
		  const mainSystem::inputParam&,
		  const size_t);
  void processEMF(SimFLUKA&,
		  const mainSystem::inputParam&,
		  const size_t);
  void processLAM(SimFLUKA&,
		  const mainSystem::inputParam&,
		  const size_t);
  void processMAT(SimFLUKA&,
		  const mainSystem::inputParam&,
		  const size_t);
};

}

#endif
 
