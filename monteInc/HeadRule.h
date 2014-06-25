/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/HeadRule.h
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
#ifndef HeadRule_h
#define HeadRule_h

class Token;
class Rule;
class CompGrp;

namespace Geometry
{
  class Surface;
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

  Rule* findKey(const int); 
  void removeItem(Rule*);
  static int procPair(std::string&,std::map<int,Rule*>&,int&);
  static CompGrp* procComp(Rule*);

  void createAddition(const int,const Rule*);

 public:


  HeadRule();
  HeadRule(const HeadRule&);
  HeadRule& operator=(const HeadRule&);
  ~HeadRule();

  const Rule* getTopRule() const { return HeadNode; }

  void populateSurf();
  void reset();

  /// Has a valid rule
  bool hasRule() const { return (HeadNode) ? 1 : 0; } 
  bool isComplementary() const;
  bool isUnion() const;

  bool isValid(const Geometry::Vec3D&,const std::set<int>&) const;
  bool isValid(const Geometry::Vec3D&,const int) const;           
  bool isValid(const Geometry::Vec3D&) const;           
  int pairValid(const int,const Geometry::Vec3D&) const;           
  bool isValid(const std::map<int,int>&) const; 
  bool isDirectionValid(const Geometry::Vec3D&,const int) const; 
  int trackSurf(const Geometry::Vec3D&,const Geometry::Vec3D&,
		double&) const;

  std::set<const Geometry::Surface*> getOppositeSurfaces() const;
  std::vector<const Geometry::Surface*> getSurfaces() const;
  std::vector<int> getSurfaceNumbers() const;
  std::vector<int> getTopSurfaces() const;

  int removeItems(const int);
  int removeTopItem(const int);
  int substituteSurf(const int,const int,const Geometry::Surface*);

  void makeComplement();

  int procString(const std::string&);

  void addIntersection(const int);
  void addUnion(const int);
  void addIntersection(const std::string&);
  void addUnion(const std::string&);
  void addIntersection(const HeadRule&);
  void addUnion(const HeadRule&);
  void addIntersection(const Rule*);
  void addUnion(const Rule*);

  std::string display() const;
  std::string display(const Geometry::Vec3D&) const;
  std::string displayAddress() const;
  void displayVec(std::vector<Token>&) const;  

};  

#endif
