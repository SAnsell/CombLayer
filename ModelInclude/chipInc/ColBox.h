/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/ColBox.h
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
#ifndef hutchSystem_ColBox_h
#define hutchSystem_ColBox_h

class Simulation;

namespace hutchSystem
{
/*!
  \class ColBox
  \version 1.0
  \author S. Ansell
  \date February 2011
  \brief ColBox [insert object]
*/

class ColBox : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int colIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
 
  int insertCell;               ///< Cell to insert into
  Geometry::Vec3D XAxis;        ///< X actual direction
  Geometry::Vec3D YAxis;        ///< Y-movement axis 
  Geometry::Vec3D ZAxis;        ///< Z-movement axis 
  Geometry::Vec3D Centre;       ///< Centre point 
  
  double xStep;                 ///< X step accross beam
  double fStep;                 ///< forward step 
  double zStep;                 ///< Zstep of all
  double xyAngle;               ///< XY-Angle
  double zAngle;                ///< Zangle

  double width;             ///< Full Width
  double depth;             ///< Full Depth
  double height;            ///< Full Height 

  double roofThick;          ///< Roof thickness
  double floorThick;         ///< Roof thickness
  double frontThick;         ///< Front thickness
  double backThick;          ///< Back thickness
  
  double viewX;              ///< View width
  double viewZ;              ///< View height

  int outMat;                 ///< Material for roof
  int defMat;                 ///< Material for roof
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  ColBox(const std::string&);
  ColBox(const ColBox&);
  ColBox& operator=(const ColBox&);
  ~ColBox();

  void setMidFace(const Geometry::Vec3D&);

  void createPartial(Simulation&,const attachSystem::TwinComp&);
  void createAll(Simulation&,const attachSystem::TwinComp&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D& Pt) const;

};

}

#endif
 
