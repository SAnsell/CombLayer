/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WeightControl.h
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
#ifndef WeightSystem_WeightControl_h
#define WeightSystem_WeightControl_h

///\file 

class Simulation;
namespace Geometry
{
  class Cone;
  class Plane;
}
namespace mainSystem
{
  class inputParam;
}

/*!
  \namespace WeightSystem
  \brief Main variance reduction ssytem
  \author S. Ansell
  \version 1.0
  \date April 2015
*/

namespace WeightSystem
{
  class ItemWeight;
  class CellWeight;
  class WWGWeight;
  
  /*!
    \class WeightControl
    \version 1.0
    \author S. Ansell
    \date October 2015
    \brief Input to Weigths controller
  */
  
class WeightControl
{
 protected:

  std::set<std::string> activeParticles;    ///< active particles
  
  double energyCut;              ///< Energy cut [MeV]
  double scaleFactor;            ///< Scale factor
  double weightPower;            ///< makes weight W^power
  double density;                ///< scales the material density
  double r2Length;               ///< scale factor of r2 Length 
  double r2Power;                ///< makes weight 1/r^power

  std::vector<double> EBand;     ///< Energy band
  std::vector<double> WT;        ///< Weight scalar
  
  std::vector<Geometry::Cone> conePt;         ///< Cone points
  std::vector<Geometry::Plane> planePt;       ///< Plane points
  std::vector<Geometry::Vec3D> sourcePt;      ///< Source Points

  void processPtString(std::string,std::string&,size_t&,bool&);
  
  void setHighEBand();
  void setMidEBand();
  void setLowEBand();
  
  void procParticles(const mainSystem::inputParam&);
  void procEnergyType(const mainSystem::inputParam&);
  void procSourcePoint(const mainSystem::inputParam&);
  void procPlanePoint(const mainSystem::inputParam&);

  void procParam(const mainSystem::inputParam&,const std::string&,
		const size_t,const size_t);  

  static void help();

  static void procConeHelp();
  static void procCalcHelp();
  static void procEnergyTypeHelp();
  static void procObjectHelp();
  static void procRebaseHelp();

 public:

  WeightControl();
  WeightControl(const WeightControl&);
  WeightControl& operator=(const WeightControl&);
  virtual ~WeightControl();
  
  virtual void processWeights(Simulation&,const mainSystem::inputParam&);
  virtual void normWeights(Simulation&,const mainSystem::inputParam&);
    
};

}


#endif
 
