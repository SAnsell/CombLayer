/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/ExcludedComp.h
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
#ifndef attachSystem_ExcludedComp_h
#define attachSystem_ExcludedComp_h

class Simulation;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
  class ContainedComp;
  class ContainedGroup;

/*!
  \class ExcludedComp
  \version 1.0
  \author S. Ansell
  \date August 2010
  \brief Allows an object to be wrapped

  Contains storage for the outer surface rule of an object.
  This storage can be access via a string for inclusion/exclusion
  of a particular object.
*/

class ExcludedComp  
{
 private:

  std::vector<int> excludeCells;    ///< Cell to insert into [self registered]

  std::vector<std::string> ExcludeObj;   ///< Fixed object to use
  /// Boundary object [Encloses cut volume]
  Rule* boundary;                         

 public:

  ExcludedComp();
  ExcludedComp(const ExcludedComp&);
  ExcludedComp& operator=(const ExcludedComp&);
  virtual ~ExcludedComp();

  virtual void applyBoundary(Simulation&);

  virtual std::string getNotExcludeUnit() const;
  virtual std::string getExcludeUnit() const;

  void clearRules();
  /// Test if has rule
  bool hasExclude() const { return (boundary) ? 1 : 0; }
  int isExcludeUnitValid(const Geometry::Vec3D&) const;

  void addExcludeCell(const int);
  void setExcludeCell(const int);


  void addExcludeSurf(const attachSystem::FixedComp&,const long int);
  void addExcludeSurf(const std::string&,const long int);

  void addExcludeSurf(const int);
  void addExcludeSurf(const std::string&);
  void addExcludeObj(const std::string&);
  void addExcludeObj(const ContainedComp&);
  void addExcludeObj(const std::string&,const std::string&);
  void addExcludeObj(const std::string&,const ContainedGroup&);
};

}

#endif
 
