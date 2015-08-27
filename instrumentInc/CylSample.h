/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   instrumentInc/CylSample.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef instrumentSystem_CylSample_h
#define instrumentSystem_CylSample_h

class Simulation;

namespace instrumentSystem
{

/*!
  \class CylSample
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder object
*/

class CylSample : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int samIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy rotation angle
  double zAngle;                  ///< z rotation angle

  size_t nLayers;                     ///< number of layers
  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<int> mat;               ///< Materials


  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CylSample(const std::string&);
  CylSample(const CylSample&);
  CylSample& operator=(const CylSample&);
  virtual ~CylSample();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
