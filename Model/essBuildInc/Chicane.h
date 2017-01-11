/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/Chicane.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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
#ifndef essSystem_Chicane_h
#define essSystem_Chicane_h

class Simulation;

namespace essSystem
{

struct chicaneUnit
{
public:

  Geometry::Vec3D XZoffset;  ///< Ignored Y component
  double length;      ///< Lengths of all segments
  double  width;     ///< Lengths of all segments
  double height;     ///< Lengths of all segments
  int mat;           ///< material

  /// constructor
  chicaneUnit(const double l,const double w,
	      const double h,const int M) :
    length(l),width(w),height(h),mat(M) {}
  
  chicaneUnit(const chicaneUnit& A) :
  XZoffset(A.XZoffset),length(A.length),
    width(A.width),height(A.height),mat(A.mat)
  {}

  chicaneUnit&
  operator=(const chicaneUnit& A)
  {
    if (this!=&A)
      {
	XZoffset=A.XZoffset;
	length=A.length;
	width=A.width;
	height=A.height;
	mat=A.mat;
      }
    return *this;
  }
    
 ~chicaneUnit() {}  
};
 
  
/*!
  \class Chicane
  \version 1.0
  \author Konstantin Batkov
  \date November 2016
  \brief Chicane
*/

class Chicane : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::FrontBackCut
{
 private:

  const int chicaneIndex;          ///< Index of surface offset
  int cellIndex;                   ///< Cell index

  size_t nBlock;                   ///< Number of segments
  std::vector<chicaneUnit> CUnits; ///< Chicane units.

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createUnitVector(const Bunker&,const size_t);
  
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  
 public:

  Chicane(const std::string&);
  Chicane(const Chicane&);
  Chicane& operator=(const Chicane&);
  virtual ~Chicane();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(Simulation&,const Bunker&,const size_t);

};

}

#endif
 

