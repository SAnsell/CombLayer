/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/CH4Layer.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef ts1System_CH4Layer_h
#define ts1System_CH4Layer_h

class Simulation;


namespace ts1System
{

/*!
  \class CH4Layer
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief TS1 CH4Layer upgrade
*/

class CH4Layer : public constructSystem::ModBase
{
 private:
    
  std::vector<LayerInfo> LVec;  ///< Layer Info vector [walls]

  size_t nPoison;                  ///< Number of poison
  std::vector<double> poisonYStep; ///< Y centre of poison layer
  double poisonThick;              ///< Poison thickness
  int poisonMat;                   ///< Poison (Gadolinium) 
  double pCladThick;               ///< Poison thickness
  int pCladMat;                    ///< Poison Cladding (Al)  


  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

  double checkUnit(const FuncDataBase&,const size_t,
		   const std::string&,const double,
		   const std::string&,const double,
		   const std::string&,const double) const;

 public:

  CH4Layer(const std::string&);
  CH4Layer(const CH4Layer&);
  CH4Layer& operator=(const CH4Layer&);
  virtual CH4Layer* clone() const;
  virtual ~CH4Layer();

  virtual Geometry::Vec3D 
    getSurfacePoint(const size_t,const size_t) const;
  virtual int 
    getLayerSurf(const size_t,const size_t) const;
  virtual std::string 
    getLayerString(const size_t,const size_t) const;

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
