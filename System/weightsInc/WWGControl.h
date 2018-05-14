/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WWGControl.h
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
#ifndef WeightSystem_WWGControl_h
#define WeightSystem_WWGControl_h

///\file 

class Simulation;
namespace Geometry
{
  class Plane;
}

namespace WeightSystem
{
  class ItemWeight;
  class CellWeight;
  class WWGWeight;
  
  /*!
    \class WWGControl
    \version 1.0
    \author S. Ansell
    \date April 2017
    \brief Controls WWG mesh maps
  */
  
class WWGControl : public WeightControl
{
 private:

  // exta factors for MARKOV:
  size_t nMarkov;                ///< Markov count  

  WWGWeight* sourceFlux;            ///< Source flux
  WWGWeight* adjointFlux;           ///< Adjoint flux
  
  //  void help() const
  
  void procMarkov(const mainSystem::inputParam&,const std::string&,
		  const size_t);
  void wwgSetParticles(const std::set<std::string>&);
  void wwgMesh(const mainSystem::inputParam&);
  void wwgVTK(const mainSystem::inputParam&);
  void wwgInitWeight();
  void wwgCreate(const Simulation&,const mainSystem::inputParam&);
  void wwgCombine(const Simulation&,const mainSystem::inputParam&);

  
  void wwgMarkov(const Simulation&,const mainSystem::inputParam&);
  void wwgNormalize(const mainSystem::inputParam&);
    
 public:

  WWGControl();
  WWGControl(const WWGControl&);
  WWGControl& operator=(const WWGControl&);
  virtual ~WWGControl();
  
  void processWeights(Simulation&,const mainSystem::inputParam&);
    
};

}


#endif
 
