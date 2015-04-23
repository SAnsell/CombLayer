/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/refPlate.h
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
#ifndef moderatorSystem_refPlate_h
#define moderatorSystem_refPlate_h

class Simulation;


namespace ts1System
{

/*!
  \class refPlate
  \version 1.0
  \author S. Ansell
  \date September 2012
  \brief TS1 refPlate [insert object]
*/

class refPlate  : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int pIndex;    ///< Actual surface offset
  int cellIndex;        ///< Cell index
  int planeFlag;        ///< Plane flag [0-empty/63 all set]

  int SN[6];            ///< Surface Number
  int matN;             ///< Material number  
  
  void createObjects(Simulation&);
  static size_t dirType(const std::string&);
  static size_t dirOppositeType(const std::string&);

 public:

  refPlate(const std::string&);
  refPlate(const refPlate&);
  refPlate& operator=(const refPlate&);
  ~refPlate() {}   ///< Destructor


  /// Name : Index 
  void setOrigin(const std::string&,const size_t);
  void setOrigin(const FixedComp&,const size_t);
  // +/-X/Y/Z/  : Name : Index
  void setPlane(const std::string&,const std::string&,const int,const size_t);
  void setPlane(const std::string&,const FixedComp&,const int,const size_t);
  void setPlane(const std::string&,const Geometry::Vec3D&,
		const Geometry::Vec3D&);
  void setPlane(const std::string&,const double);
  void setPlane(const std::string&,const std::string&,const size_t,
		const double);

  void createAll(Simulation&);


};

}  

#endif
