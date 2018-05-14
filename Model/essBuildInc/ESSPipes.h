/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/makeESS.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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
#ifndef essSystem_ESSPipes_h
#define essSystem_ESSPipes_h

namespace constructSystem
{
  class SupplyPipe;

}

namespace essSystem
{
  /*!
    \class ESSPipes
    \version 1.0
    \author S. Ansell/K. Batkov
    \date July 2016
    \brief Builded and control pipes for ESS-moderator
  */
  
class ESSPipes
{
 private:

  /// storage for pipes
  typedef std::shared_ptr<constructSystem::SupplyPipe> PipeTYPE;
  /// Main map of all pipes
  std::map<std::string,PipeTYPE> PipeMap;


  void registerPipes(const std::vector<std::string>&);
  void buildH2Pipe(Simulation&,const std::string&,
		   const std::string&,const  std::string&,
		   const std::string&,
		   const  std::string& connect="",
		   const std::string& invar="");

  PipeTYPE& getPipe(const std::string&);

  
 public:

  ESSPipes();
  ESSPipes(const ESSPipes&);
  ESSPipes& operator=(const ESSPipes&);
  ~ESSPipes() {}   ///< Destructor

  void buildTopPipes(Simulation&,const std::string&);
  void buildLowPipes(Simulation&,const std::string&);

};
 

}

#endif
