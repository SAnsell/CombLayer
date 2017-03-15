/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WeightCone.h
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
#ifndef WeightSystem_WeightCone_h
#define WeightSystem_WeightCone_h

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
    \class WeightCone
    \version 1.0
    \author S. Ansell
    \date January 2017
    \brief Build a cone for variance reductuion
  */
  
class WeightCone
{
 private:

  double energyCut;              ///< Energy cut [MeV]
  double scaleFactor;            ///< Scale factor
  double minWeight;              ///< Min weight
  double weightPower;            ///< makes weight W^power
  std::vector<double> EBand;     ///< Energy bandk
  std::vector<double> WT;        ///< Weight scalar
  
  std::set<std::string> objectList;  ///< Object list to this cut [local]

  bool activeAdjointFlag;                   ///< Active plane
  std::string activePtType;                 ///< ptType 
  size_t activePtIndex;                     ///< plant/source/track pt 


  std::vector<Geometry::Cone> conePt;         ///< Cone points
  std::vector<Geometry::Plane> planePt;       ///< Plane points
  std::vector<Geometry::Vec3D> sourcePt;      ///< Source Points
  
  void setHighEBand();
  void setMidEBand();
  void setLowEBand();

  void scaleObject(const Simulation&,const std::string&,
		   const double,const double);
  void scaleAllObjects(const Simulation&,const double,const double);
  double findMax(const Simulation&,const std::string&,
		 const size_t,const double) const;
  
  void help() const;
  
  void procType(const mainSystem::inputParam&);
  void procParam(const mainSystem::inputParam&,const std::string&,
		const size_t,const size_t);
  void procTypeHelp() const;

  void procSourcePoint(const mainSystem::inputParam&);
  void procPlanePoint(const mainSystem::inputParam&);
  void procTrackLine(const mainSystem::inputParam&);
  void procObject(const Simulation&,
		  const mainSystem::inputParam&);
  void procRebase(const Simulation&,
		  const mainSystem::inputParam&);
  void procTrack(const Simulation&,
		  const mainSystem::inputParam&);

  void processPtString(std::string);

  void procCalcHelp() const;
  void procRebaseHelp() const;
  void procObjectHelp() const;
  void procConeHelp() const;
  
  
  void setWeights(Simulation&);
  void cTrack(const Simulation&,const Geometry::Vec3D&,
	      const std::vector<Geometry::Vec3D>&,
	      const std::vector<long int>&,
	      ItemWeight&);
  void cTrack(const Simulation&,const Geometry::Plane&,
	      const std::vector<Geometry::Vec3D>&,
	      const std::vector<long int>&,
	      ItemWeight&);


  void calcPoints(std::vector<Geometry::Vec3D>&,
		  std::vector<long int>&) const;
		  
  // WWG stuff
  void wwgGetFactors(const mainSystem::inputParam&,
		     double&,double&) const;

  void wwgMesh(const mainSystem::inputParam&);
  void wwgEnergy();
  void wwgCreate(Simulation&,const mainSystem::inputParam&);

  void calcWWGTrack(const Simulation&,const Geometry::Plane&,
		    WWGWeight&);
  void calcWWGTrack(const Simulation&,const Geometry::Vec3D&,
		    WWGWeight&);
  void calcCellTrack(const Simulation&,const Geometry::Vec3D&,
		     const std::vector<int>&,CellWeight&);
  void calcCellTrack(const Simulation&,const Geometry::Plane&,
		     const std::vector<int>&,CellWeight&);
  void calcCellTrack(const Simulation&,const Geometry::Cone&,
		     CellWeight&);


 public:

  WeightCone();
  WeightCone(const WeightCone&);
  WeightCone& operator=(const WeightCone&);
  ~WeightCone();

  
  void processWeights(Simulation&,const mainSystem::inputParam&);
    
};

}


#endif
 
