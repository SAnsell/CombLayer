/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/SplitInner.h
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
#ifndef ts1System_SplitInner_h
#define ts1System_SplitInner_h

class Simulation;


namespace ts1System
{

/*!
  \class SplitInner
  \version 1.0
  \author S. Ansell
  \date Septeber 2014
  \brief TS1 SplitInner upgrade
*/

class SplitInner : public ts1System::CH4Layer
{
 private:

  const std::string IKeyName;     ///< Inner key name
  const int innerIndex;           ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t innerNLayer;             ///< Number of layers [internal]
  std::vector<double> thick;      ///< thickness of layers
  std::vector<int> mat;           ///< Materials
  std::vector<double> temp;       ///< Temperatures
    
  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  SplitInner(const std::string&,const std::string&);
  SplitInner(const SplitInner&);
  SplitInner& operator=(const SplitInner&);
  virtual SplitInner* clone() const;
  virtual ~SplitInner();

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
 
