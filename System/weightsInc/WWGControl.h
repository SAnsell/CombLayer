/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WWGControl.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
  class WWGWeight;
  
  /*!
    \class WWGControl
    \version 1.0
    \author S. Ansell
    \date April 2017
    \brief Controls WWG mesh maps
  */
  
class WWGControl 
{
 private:

  std::set<std::string> activeParticles;    ///< active particles
  std::vector<double> EBin;                 ///< energy bin default
  
  // exta factors for MARKOV:
  size_t nMarkov;                ///< Markov count

  std::map<std::string,Geometry::Plane> planePt;       ///< Plane points
  std::map<std::string,Geometry::Vec3D> sourcePt;      ///< Source Points
  std::map<std::string,Geometry::BasicMesh3D> meshUnit;     ///< mesh volumes
  std::map<std::string,std::vector<double>> engUnit;        ///< energy grid
  
  //  void help() const
  
  void procMarkov(const mainSystem::inputParam&,const std::string&,
		  const size_t);
  void procEnergyMesh(const mainSystem::inputParam&);
  void procSourcePoint(const Simulation&,const mainSystem::inputParam&);
  void procPlanePoint(const Simulation&,const mainSystem::inputParam&);
  void procMeshPoint(const Simulation&,const mainSystem::inputParam&);

  void wwgVTK(const mainSystem::inputParam&);
  void wwgCreate(const Simulation&,const mainSystem::inputParam&);
  void wwgCADIS(const Simulation&,const mainSystem::inputParam&);
  
  void wwgMarkov(const Simulation&,const mainSystem::inputParam&);
  void wwgNormalize(const mainSystem::inputParam&);
  void wwgActivate(const Simulation&,const mainSystem::inputParam&);
  
  void processPtString(std::string ptStr,std::string& pType,
		       size_t&,bool&) const;

  const std::vector<double>& getEnergy(std::string,size_t&) const;
  const std::vector<double>& getEnergy(const std::string&) const;
  const Geometry::BasicMesh3D& getGrid(const std::string&) const;
  
  bool hasSourcePoint(const std::string&) const;
  bool hasPlanePoint(const std::string&) const;
  const Geometry::Vec3D& getSourcePoint(const std::string&) const;
  const Geometry::Plane& getPlanePoint(const std::string&) const;

  
public:

  WWGControl();
  WWGControl(const WWGControl&);
  WWGControl& operator=(const WWGControl&);
  virtual ~WWGControl();
  
  void processWeights(Simulation&,const mainSystem::inputParam&);
    
};

}


#endif
 
