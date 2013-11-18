/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/insertInfo.h
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
#ifndef shutterSystem_insertInfo_h
#define shutterSystem_insertInfo_h

namespace shutterSystem
{

/*!
  \struct insertInfo
  \brief Information on each shutter insert component
  \author S. Ansell
  \date May 2009
  \version 1.0
*/

struct insertInfo
{
 public:

  int lNum;                 ///< Length number
  int rNum;                 ///< Radius number
  int matID;                ///< Material

  Geometry::Vec3D CPt;      ///< Centre point
  Geometry::Vec3D Axis;     ///< Axis line

  double Doffset;           ///< StartPoint [cm]
  double Length;            ///< Length [cm]
  double ORad;              ///< Outer Radius

  // Indexes:
  int innerRID;         ///< Inner Radius
  int outerRID;         ///< Outer Radius
  int fPlane;           ///< Front plane
  int bPlane;           ///< Back plane
  
  std::string Sides;    ///< Sides

  insertInfo(const int,const int);
  insertInfo(const int,const int,const int,const double,const double,const double);
  insertInfo(const insertInfo&);
  insertInfo(const int,const double,const insertInfo&);
  insertInfo& operator=(const insertInfo&);
  ~insertInfo() {} ///< Destructor

  void setCoordinates(const Geometry::Vec3D&,const Geometry::Vec3D&);
  /// Get Full length
  double calcFullLength() const { return Doffset+Length; }  
  Geometry::Vec3D calcStepPoint() const;
  Geometry::Vec3D calcFrontPoint() const;
  
  std::string cellString() const;
  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const insertInfo&);

/*!
  \class insertInfoSort
  \brief Compares insertInfo object to align 
  \version 1.0
  \author S. Ansell
  \date May 2009
*/
class insertInfoSort
{
 public:
  /// Actual comparitor
  bool operator()(const insertInfo& A,const insertInfo& B)  const
    {
      if (A.rNum!=B.rNum)   // This is an effective promotion of 0 to infinity
	return (!A.rNum || !B.rNum) ? A.rNum : A.rNum<B.rNum;
      
      return A.Doffset<B.Doffset;
    }
};

 
}

#endif
