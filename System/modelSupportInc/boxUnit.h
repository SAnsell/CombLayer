/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/boxUnit.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef ModelSupport_boxUnit_h
#define ModelSupport_boxUnit_h

namespace ModelSupport
{
 
class surfRegister;

/*!
  \class boxUnit
  \version 1.0
  \author S. Ansell
  \date October 2011
  \brief A single unit of a pipe
*/

class boxUnit :
    public attachSystem::FixedUnit,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap
{
 private:

  boxUnit* prev;            ///< Previous pipe unit
  boxUnit* next;            ///< Next pipe unit

  Geometry::Vec3D APt;      ///< Start point
  Geometry::Vec3D BPt;      ///< End Point
  
  Geometry::Vec3D Axis;     ///< Main axis
  Geometry::Vec3D ANorm;    ///< A-Pt Normal [outer]
  Geometry::Vec3D BNorm;    ///< B-Pt Normal [outer]

  Geometry::Vec3D XUnit;    ///< X point Axis
  Geometry::Vec3D ZUnit;    ///< Z point Axis

  HeadRule ASurf;           ///< Start point rule if used [inward facing]
  HeadRule BSurf;           ///< End point rule if used [inward facing]
  double maxExtent;         ///< longest length

  size_t activeFlag;                   ///< Flags for active layers
  std::vector<boxValues> boxVar;       ///< Cylinder variables
  size_t nSides;                       ///< Number of sides
  std::set<int> cellCut;               ///< Cells the pipe 


  void calcNorm(const int,const Geometry::Vec3D&,
		const Geometry::Vec3D&);
  void calcXZ(const Geometry::Vec3D&,const Geometry::Vec3D&);

  void checkForward();
  void excludeUnit(Simulation&,
		   const std::map<int,MonteCarlo::Object*>&) const;
  size_t getOuterIndex() const;


  /// access full extent
  double getExtent() const { return maxExtent; }
  double getTanAngle(const Geometry::Vec3D&) const;
  Geometry::Vec3D getAExtra() const;
  Geometry::Vec3D getBExtra() const;
  std::string createCaps() const;

  // Main build
  void populate(const size_t,const std::vector<boxValues>&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createOuterObjects();
  void insertObjects(Simulation&);
  
 public:
  
  boxUnit(const std::string&,const size_t);
  boxUnit(const boxUnit&);
  /// Virtual constructor
  virtual boxUnit* clone() const { return new boxUnit(*this); }  
  boxUnit& operator=(const boxUnit&);
  virtual ~boxUnit();


  /// Access ZUnit vector
  const Geometry::Vec3D& getZUnit() const { return ZUnit; }
  /// Access Axis vector
  const Geometry::Vec3D& getAxis() const { return Axis; }
  const Geometry::Vec3D& getPt(const int) const;
  const HeadRule& getCap(const int) const;

  void clearInsertSet();
  void addInsertSet(const std::set<int>&);


  void setPoints(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setASurf(const HeadRule&);
  void setBSurf(const HeadRule&);
  void setZUnit(const Geometry::Vec3D&);
  void connectFrom(boxUnit*);
  void connectTo(boxUnit*);


  void buildUnit(Simulation&,const size_t,
		 const std::vector<boxValues>&);
    
};

}

#endif
