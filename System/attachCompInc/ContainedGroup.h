/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/ContainedGroup.h
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
#ifndef attachSystem_ContainedGroup_h
#define attachSystem_ContainedGroup_h

class Simulation;
class HeadRule;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
/*!
  \class ContainedGroup
  \version 1.0
  \author S. Ansell
  \date August 2010
  \brief Allows an object to be wrapped

  Contains storage for the outer surface rule of an object.
  This storage can be access via a string for inclusion/exclusion
  of a particular object.
*/

class ContainedGroup  
{
 private:

  /// Map storage
  typedef std::map<std::string,ContainedComp> CTYPE;
  /// Named Container
  CTYPE CMap;

public:

  // To be removed:
  void addBoundarySurf(const std::string&,const int);
  void addBoundarySurf(const std::string&,const std::string&);
  void addBoundaryUnionSurf(const std::string&,const int);
  void addBoundaryUnionSurf(const std::string&,const std::string&);

  bool hasBoundary(const std::string&) const;
  
 public:

  ContainedGroup();
  explicit ContainedGroup(const std::string&);
  ContainedGroup(const std::string&,const std::string&);
  ContainedGroup(const std::string&,const std::string&,
		 const std::string&);
  ContainedGroup(const std::string&,const std::string&,
		 const std::string&,const std::string&);
  ContainedGroup(const std::string&,const std::string&,
		 const std::string&,const std::string&,
		 const std::string&);
  ContainedGroup(const std::string&,const std::string&,
		 const std::string&,const std::string&,
		 const std::string&,const std::string&);
  ContainedGroup(const ContainedGroup&);
  ContainedGroup& operator=(const ContainedGroup&);
  virtual ~ContainedGroup();

  virtual std::string getAllExclude() const;
  
  virtual const HeadRule& getOuterSurf(const std::string&) const;
  virtual const HeadRule& getBoundary(const std::string&) const;

  virtual std::string getExclude(const std::string&) const;
  virtual std::string getContainer(const std::string&) const;
  virtual std::string getCompExclude(const std::string&) const;
  virtual std::string getCompContainer(const std::string&) const;

  void clearRules();
  void clearRule(const std::string&);
  /// Test if has rule
  bool hasOuterSurf(const std::string&) const;

  
  void addOuterSurf(const std::string&,const int);
  void addOuterSurf(const std::string&,const std::string&);
  void addOuterSurf(const std::string&,const HeadRule&);
  void addOuterSurf(const std::string&,const ContainedComp&);

  void addOuterUnionSurf(const std::string&,const std::string&);
  void addOuterUnionSurf(const std::string&,const HeadRule&);
  void addOuterUnionSurf(const std::string&,const ContainedComp&);


  // Determine the surface that the line intersect first 
  // and its sign.
  int surfOuterIntersect(const std::string&,const Geometry::Line&) const;

  void addAllInsertCell(const int);
  void addAllInsertCell(const ContainedComp&);
  void addAllInsertCell(const std::vector<int>&);

  void addInsertCell(const std::string&,const int);
  void addInsertCell(const std::string&,const std::vector<int>&);
  void addInsertCell(const std::string&,const ContainedComp&);

  void setInsertCell(const std::string&,const int);
  void setAllInsertCell(const int);


  void insertObjects(Simulation&);

  virtual void insertAllInCell(Simulation&,const int) const;
  virtual void insertAllInCell(Simulation&,const std::vector<int>&) const;

  void insertInCell(const std::string&,Simulation&,const int) const;
  void insertInCell(const std::string&,Simulation&,
		    const std::vector<int>&) const;
  void insertInCell(const std::string&,MonteCarlo::Object&) const;


  /// Size accessor
  size_t nGroups() const { return CMap.size(); } 
  bool hasKey(const std::string&) const;
  ContainedComp& addCC(const std::string&);
  ContainedComp& getCC(const std::string&);
  const ContainedComp& getCC(const std::string&) const;

};

}

#endif
 
