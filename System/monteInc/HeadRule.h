/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/HeadRule.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef HeadRule_h
#define HeadRule_h

class Token;
class Rule;
class CompGrp;
class SurfPoint;

namespace Geometry
{
  class Surface;
  struct interPoint;
}

namespace ModelSupport
{
  class surfRegister;
}

/*!
  \class HeadRule
  \brief Holds the topRule pointer
  \author S.Ansell
  \version 1.0
  \date May 2013
 
  Base class for a rule item in the tree. 
*/

class HeadRule
{
 private:

  Rule* HeadNode;                    ///< Parent object (for tree)
  ///< set of surfaces with opposite signs
  std::set<const Geometry::Surface*> signPairedSurf;   
  
  Rule* findKey(const int); 
  void removeItem(const Rule*);
  static int procPair(std::string&,std::map<int,Rule*>&,int&);
  static CompGrp* procComp(Rule*);

  void createAddition(const int,const Rule*);
  const SurfPoint* findSurf(const int) const;
  
 public:

  HeadRule();
  explicit HeadRule(const int);
  explicit HeadRule(const ModelSupport::surfRegister&,const int);
  explicit HeadRule(const ModelSupport::surfRegister&,const int,const int);
  explicit HeadRule(const std::string&);
  HeadRule(const HeadRule&);
  HeadRule(HeadRule&&);
  HeadRule(const Rule*);
  HeadRule& operator=(const HeadRule&);
  ~HeadRule();
  
  bool operator==(const HeadRule&) const;
  bool operator!=(const HeadRule&) const;
  HeadRule& operator+=(const HeadRule&);
  HeadRule& operator*=(const HeadRule&);
  HeadRule& operator-=(const HeadRule&);
  HeadRule& operator/=(const HeadRule&);
  HeadRule operator+(const HeadRule&) const;
  HeadRule operator*(const HeadRule&) const;
  HeadRule operator-(const HeadRule&) const;
  HeadRule operator/(const HeadRule&) const;
  
  /// access main rule
  const Rule* getTopRule() const { return HeadNode; }

  void populateSurf();
  void reset();

  /// Has a valid rule
  bool hasRule() const { return (HeadNode) ? 1 : 0; }
  /// has valid filled rule [non-empty]
  bool isEmpty() const;
  bool isComplementary() const;
  bool isUnion() const;

  bool isAnyValid(const Geometry::Vec3D&,const std::set<int>&) const;
  bool isSideValid(const Geometry::Vec3D&,const int) const;

  bool isValid(const Geometry::Vec3D&) const;
  bool isValid(const Geometry::Vec3D&,const int) const;
  bool isValid(const std::map<int,int>&) const;
  bool isValid(const Geometry::Vec3D&,const std::map<int,int>&) const;

  bool isLineValid(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  bool isZeroVolume() const;

  std::set<int> getPairedSurf() const;
  
  std::set<int> surfValid(const Geometry::Vec3D&) const;
  std::tuple<int,const Geometry::Surface*,Geometry::Vec3D,double>
  trackSurfIntersect(const Geometry::Vec3D&,const Geometry::Vec3D&)
    const;
  
  std::pair<int,double> trackSurfDistance
    (const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  std::pair<int,double> trackSurfDistance
    (const Geometry::Vec3D&,const Geometry::Vec3D&,const std::set<int>&) const;

  int trackSurf(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  int trackSurf(const Geometry::Vec3D&,const Geometry::Vec3D&,
		const std::set<int>&) const;

  Geometry::Vec3D trackPoint(const Geometry::Vec3D&,
			     const Geometry::Vec3D&) const;
  Geometry::Vec3D trackClosestPoint
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     const Geometry::Vec3D&) const;
  int trackClosestSurface
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     const Geometry::Vec3D&) const;

  size_t calcSurfIntersection
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     std::vector<Geometry::interPoint>&) const;

  size_t calcSurfIntersection
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     std::vector<Geometry::Vec3D>&) const;

  size_t calcSurfSurfIntersection(std::vector<Geometry::Vec3D>&) const;

  std::set<const Geometry::Surface*> getOppositeSurfaces() const;
  const Geometry::Surface* getSurface(const int) const;
  const Geometry::Surface* primarySurface() const;
  std::set<const Geometry::Surface*> getSurfaces() const;
  std::set<int> getSignedSurfaceNumbers() const;
  std::set<int> getSurfaceNumbers() const;
  std::vector<int> getTopSurfaces() const;
  int getPrimarySurface() const;

  
  const Rule* findNode(const size_t,const size_t) const;

  std::vector<const Rule*> findTopNodes() const;
  std::vector<const Rule*> findNodes(const size_t) const;
  HeadRule getComponent(const size_t,const size_t) const;
  bool subMatched(const HeadRule&,const HeadRule&);  
  bool partMatched(const HeadRule&) const;

  int getSingleSurf() const;
  
  int removeItems(const int);
  int removeUnsignedItems(const int);
  int removeMatchedPlanes(const Geometry::Vec3D&,const double);
  int removeOuterPlane(const Geometry::Vec3D&,const Geometry::Vec3D&,
		       const double);
  void isolateSurfNum(const std::set<int>&);
  int removeTopItem(const int);
  int substituteSurf(const ModelSupport::surfRegister&,
		     const int,const int,const int);
  int substituteSurf(const ModelSupport::surfRegister&,
		     const int,const int);
  int substituteSurf(const int,const int,const Geometry::Surface*);
  void removeCommon();
  void removeComplement();

  std::set<int> findAxisPlanes(const Geometry::Vec3D&,const double);
  int findAxisPlane(const Geometry::Vec3D&,const double);
  
  void makeComplement();
  HeadRule complement() const;

  int procSurface(const Geometry::Surface*);
  int procSurfNum(const int);
  int procRule(const Rule*);
  int procString(const std::string&);

  HeadRule& addIntersection(const int);
  HeadRule& addUnion(const int);
  HeadRule& addIntersection(const std::string&);
  HeadRule& addUnion(const std::string&);
  HeadRule& addIntersection(const HeadRule&);
  HeadRule& addUnion(const HeadRule&);
  HeadRule& addIntersection(const Rule*);
  HeadRule& addUnion(const Rule*);

  int level(const int) const;
  HeadRule getLevel(const size_t) const;
  size_t countNLevel(const size_t) const;

  HeadRule makeValid(const Geometry::Vec3D&) const;
  
  bool Intersects(const HeadRule&) const;

  std::string display() const;
  std::string display(const Geometry::Vec3D&) const;
  std::string displayAddress() const;
  void displayVec(std::vector<Token>&) const;  

  std::string displayFluka() const;
  std::string displayPOVRay() const;
  
};  

std::ostream&
operator<<(std::ostream&,const HeadRule&);

#endif
