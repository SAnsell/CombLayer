/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedGroup.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef attachSystem_FixedGroup_h
#define attachSystem_FixedGroup_h

namespace attachSystem
{
/*!
  \class FixedGroup
  \version 1.0
  \author S. Ansell
  \date  April 2014
  \brief Multiple FixedComp units for one object
*/

class FixedGroup : public FixedComp
{
 protected:

  std::string primKey;     ///< Primary key
  std::string sndKey;      ///< Secondary key
  /// Fixed Storage
  typedef std::shared_ptr<attachSystem::FixedComp> CompTYPE;
  /// Map storage
  typedef std::map<std::string,CompTYPE> FTYPE;
  /// Named Container
  FTYPE FMap;

  Geometry::Vec3D bX;          ///< Secondary X
  Geometry::Vec3D bY;          ///< Secondary Y
  Geometry::Vec3D bZ;          ///< Secondary Z
  Geometry::Vec3D bOrigin;     ///< Secondary origin
  Geometry::Vec3D bExit;       ///< Secondary origin
  
  void registerKey(const std::string&,const size_t);

 public:
  
  FixedGroup(const std::string&,const std::string&,const size_t);
  FixedGroup(const std::string&,const std::string&,const size_t,
	     const std::string&,const size_t);
  FixedGroup(const std::string&,const std::string&,const size_t,
	     const std::string&,const size_t,
	     const std::string&,const size_t);
  FixedGroup(const std::string&,const std::string&,const size_t,
	     const std::string&,const size_t,
	     const std::string&,const size_t,
	     const std::string&,const size_t);

  FixedGroup(const FixedGroup&);
  FixedGroup& operator=(const FixedGroup&);
  virtual ~FixedGroup();

  virtual void createUnitVector(const attachSystem::FixedComp&,
				const long int);
  
  void setDefault(const std::string&);
  void setSecondary(const std::string&);
  void setDefault(const std::string&,const std::string&);

  /// Size accessor
  size_t nGroups() const { return FMap.size(); } 
  bool hasKey(const std::string&) const;
  FixedComp& addKey(const std::string&,const size_t);
  virtual FixedComp& getKey(const std::string&);
  virtual const FixedComp& getKey(const std::string&) const;
  const FixedComp& getPrimary() const;
  FixedComp& getPrimary();

  
  virtual void applyRotation(const localRotate&);
  virtual void applyRotation(const Geometry::Vec3D&,const double);

  
  void setAxisControl(const long int,const Geometry::Vec3D&);

};

}

#endif
 
