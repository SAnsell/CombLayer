/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/ContainedComp.h
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
#ifndef attachSystem_ContainedComp_h
#define attachSystem_ContainedComp_h

class Simulation;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
/*!
  \class ContainedComp
  \version 1.0
  \author S. Ansell
  \date August 2010
  \brief Allows an object to be wrapped

  Contains storage for the outer surface rule of an object.
  This storage can be access via a string for inclusion/exclusion
  of a particular object.
*/

class ContainedComp  
{
 private:

  HeadRule boundary;        ///< Boundary object [Imposed containment]
  HeadRule outerSurf;       ///< Outer surfaces [Excluding boundary]

  std::vector<int> insertCells;    ///< Cell to insert into

  static int validIntersection(const HeadRule&,const bool,
			       const Geometry::Surface*,
			       const Geometry::Surface*);

 public:

  ContainedComp();
  ContainedComp(const ContainedComp&);
  ContainedComp& operator=(const ContainedComp&);
  virtual ~ContainedComp();
  
  virtual std::string getExclude() const;
  virtual std::string getCompExclude() const;
  virtual std::string getContainer() const;
  virtual std::string getCompContainer() const;

  virtual std::string 
    getCompContainer(const Geometry::Surface*,
		     const Geometry::Surface*) const;


  void copyRules(const ContainedComp&);
  void clearRules();
  void copyInterObj(const ContainedComp&);

  /// Test if has outer rule
  bool hasOuterSurf() const { return outerSurf.hasRule(); }
  /// Test if has boundary rule
  bool hasBoundary() const { return boundary.hasRule(); }
  int isBoundaryValid(const Geometry::Vec3D&) const;

  int isOuterValid(const Geometry::Vec3D&) const;
  int isOuterValid(const Geometry::Vec3D&,const std::set<int>&) const;
  int isOuterValid(const Geometry::Vec3D&,const int) const;

  // line in
  bool isOuterLine(const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  void addOuterSurf(const int);
  void addOuterSurf(const std::string&);
  void addOuterUnionSurf(const std::string&);

  void addBoundarySurf(const int);
  void addBoundarySurf(const std::string&);
  void addBoundaryUnionSurf(const int);
  void addBoundaryUnionSurf(const std::string&);

  // Determine the surface that the line intersect first 
  // and its sign.
  int surfOuterIntersect(const Geometry::Line&) const;

  void addInsertCell(const int);
  void addInsertCell(const std::vector<int>&);
  void addInsertCell(const ContainedComp&);
  void setInsertCell(const int);
  void setInsertCell(const std::vector<int>&);

  void insertObjects(Simulation&);
  void insertInCell(Simulation&,const int) const;
  void insertInCell(Simulation&,const std::vector<int>&) const;

  /// Accessor to surface [ugly]
  std::vector<Geometry::Surface*> getSurfaces() const;
  std::vector<const Geometry::Surface*> getConstSurfaces() const;
    
  /// Accessor [ugly]
  const std::vector<int>& getInsertCells() const 
    { return insertCells; }

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const ContainedComp&);

}

#endif
 
