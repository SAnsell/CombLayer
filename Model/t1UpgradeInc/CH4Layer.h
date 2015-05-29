/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/CH4Layer.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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

 protected:
    
  std::vector<LayerInfo> LVec;     ///< Layer Info vector [walls]
  
  void createFrontRule(const LayerInfo&,const int,
		       const size_t,HeadRule&) const;
  void createBackRule(const LayerInfo&,const int,
		       const size_t,HeadRule&) const;


  virtual void populate(const FuncDataBase&);

  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

  double checkUnit(const FuncDataBase&,const std::string&,
		   const size_t,
		   const std::string&,const double,
		   const std::string&,const double,
		   const std::string&,const double,
		   const bool =0,const double =0.0) const;
  void createLayerSurf(const std::vector<LayerInfo>&,
		       const int);

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

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const attachSystem::FixedComp*,const long int);
    
};

}

#endif
 
