/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/RefBox.h
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
#ifndef moderatorSystem_RefBox_h
#define moderatorSystem_RefBox_h

class Simulation;


namespace ts1System
{

/*!
  \class RefBox
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief TS1 t1Reflector [insert object]
*/

class RefBox : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  std::vector<int> surfNum;          ///< Signed surface numbers
  std::vector<Geometry::Vec3D> surfCent;    ///< Relative to origin
  std::vector<Geometry::Vec3D> surfAxis;    ///< Axis in X:Y:Z

  size_t nLayers;
  std::vector<double> layerThick;
  std::vector<int> layerMat;
  int defMat;                   ///< Default material
  

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  RefBox(const std::string&);
  RefBox(const RefBox&);
  RefBox& operator=(const RefBox&);
  virtual ~RefBox();

  void addSurface(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void addSurface(const attachSystem::FixedComp&,const int,const size_t);
  void addSurface(const std::string&,const int,const size_t);
  
  std::string getComposite(const std::string&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
