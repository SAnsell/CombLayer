/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/LinkUnit.h
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
#ifndef LinkUnit_h
#define LinkUnit_h

class Rule;

namespace attachSystem
{

/*!
  \class LinkUnit
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Connection from object to another

  Contains storage for the outer surface rule of an object.
  This storage can be access via a string for inclusion/exclusion
  of a particular object.
*/

class LinkUnit  
{
 private:

  int populated;                ///< Flag to set : axis:centre:linkSurf

  Geometry::Vec3D Axis;         ///< Connnection axis
  Geometry::Vec3D ConnectPt;    ///< Connection point
  
  int linkSurf;                 ///< Link surface [0 ==> Rule]
  HeadRule bridgeSurf;             ///< Common surface unit
  
 public:

  LinkUnit();
  LinkUnit(const LinkUnit&);
  LinkUnit& operator=(const LinkUnit&);
  ~LinkUnit();

  void complement();

  std::string getCommon() const;

  /// Check Axis 
  bool hasAxis() const { return (populated & 1); }
  /// Check Connection
  bool hasConnectPt() const { return (populated & 2); }

  const Geometry::Vec3D& getConnectPt() const;
  const Geometry::Vec3D& getAxis() const;

  int getLinkSurf() const;
  std::string getLinkString() const;

  void setAxis(const Geometry::Vec3D&);
  void setConnectPt(const Geometry::Vec3D&);

  void setBridgeSurf(const int);
  void setBridgeSurf(const std::string&);
  void addBridgeSurf(const int);
  void addBridgeSurf(const std::string&);

};

}

#endif
 
