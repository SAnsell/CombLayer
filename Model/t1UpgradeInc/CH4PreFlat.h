/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/CH4PreFlat.h
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
#ifndef ts1System_CH4PreFlat_h
#define ts1System_CH4PreFlat_h

class Simulation;

namespace ts1System
{

/*!
  \class CH4PreFlat
  \version 1.0
  \author S. Ansell
  \date July 2013
  \brief Pre-mod for CH4 
*/

class CH4PreFlat : public CH4PreModBase
{
 private:
  
  double width;             ///< Side of water thickness
  double height;              ///< Total height
  double depth;             ///< Total height

  double alThick;        ///< Al wall thickness
  double vacThick;       ///< Vac wall thickness

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  void getConnectPoints(const attachSystem::FixedComp&,
			const size_t);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CH4PreFlat(const std::string&);
  CH4PreFlat(const CH4PreFlat&);
  CH4PreFlat& operator=(const CH4PreFlat&);
  virtual ~CH4PreFlat();
  virtual CH4PreFlat* clone() const;

  virtual Geometry::Vec3D 
    getSurfacePoint(const size_t,const long int) const;
  virtual int 
    getLayerSurf(const size_t,const long int) const;
  virtual std::string 
    getLayerString(const size_t,const long int) const;

  
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int,const long int);

};

}

#endif
 
