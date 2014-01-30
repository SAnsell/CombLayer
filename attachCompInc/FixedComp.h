/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/FixedComp.h
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
#ifndef attachSystem_FixedComp_h
#define attachSystem_FixedComp_h

namespace attachSystem
{
/*!
  \class FixedComp
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Component that is at a fixed position
*/

class FixedComp  
{
 protected:
  
  const std::string keyName;       ///< Key Name
  ModelSupport::surfRegister SMap; ///< Surface register

  Geometry::Vec3D X;            ///< X-coordinate [shutter x]
  Geometry::Vec3D Y;            ///< Y-coordinate [shutter y]
  Geometry::Vec3D Z;            ///< Z-coordinate 
  Geometry::Vec3D Origin;       ///< Beam Origin  

  Geometry::Vec3D beamAxis;     ///< Neutron direction [if different]

  std::vector<LinkUnit> LU;     ///< Linked unit items
  
  void createUnitVector();
  void createUnitVector(const FixedComp&);
  void createUnitVector(const FixedComp&,const long int);
  void createUnitVector(const Geometry::Vec3D&,const Geometry::Vec3D&,
			const Geometry::Vec3D&);
  void applyShift(const double,const double,const double);
  void applyAngleRotate(const double,const double);
  void applyAngleRotate(const double,const double,const double);
  void applyFullRotate(const double,const double,
		       const Geometry::Vec3D&);

  void setConnect(const size_t,const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setBasicExtent(const double,const double,const double);

  void setLinkSurf(const size_t,const FixedComp&,const size_t);
  void setLinkSurf(const size_t,const int);
  void addLinkSurf(const size_t,const int);
  void addLinkSurf(const size_t,const std::string&);

  void setLinkComponent(const size_t,const FixedComp&,const size_t);
  void setLinkCopy(const size_t,const FixedComp&,const size_t);

 public:

  FixedComp(const std::string&,const size_t);
  FixedComp(const std::string&,const size_t,const Geometry::Vec3D&);
  FixedComp(const std::string&,const size_t,
	    const Geometry::Vec3D&,const Geometry::Vec3D&,
	    const Geometry::Vec3D&,const Geometry::Vec3D&);
  FixedComp(const FixedComp&);
  FixedComp& operator=(const FixedComp&);
  virtual ~FixedComp() {}     ///< Destructor

  const LinkUnit& operator[](const size_t) const; 


  /// Get keyname
  const std::string& getKeyName() const { return keyName; }
  /// Access X
  const Geometry::Vec3D& getX() const 
    { return X; }  
  /// Access Y direction
  const Geometry::Vec3D& getY() const 
    { return Y; }  
  /// Access Z direction
  const Geometry::Vec3D& getZ() const 
    { return Z; }
  /// Access centre
  virtual const Geometry::Vec3D& getCentre() const 
    { return Origin; }  

  virtual int getExitWindow(const size_t,std::vector<int>&) const;
  virtual int getMasterSurf(const size_t) const;
			 
  void copyLinkObjects(const FixedComp&);
  /// How many connections
  size_t NConnect() const { return LU.size(); }
  const LinkUnit& getLU(const size_t)  const; 
  void setNConnect(const size_t);

  virtual int getLinkSurf(const size_t) const;
  virtual const Geometry::Vec3D& getLinkPt(const size_t) const;
  virtual const Geometry::Vec3D& getLinkAxis(const size_t) const;
  virtual std::string getLinkString(const size_t) const;
  virtual std::string getLinkComplement(const size_t) const;

  size_t findLinkAxis(const Geometry::Vec3D&) const;

  const Geometry::Vec3D& getExit() const;
  const Geometry::Vec3D& getExitNorm() const;
  void selectAltAxis(const size_t,Geometry::Vec3D&,
		      Geometry::Vec3D&,Geometry::Vec3D&) const;

  void applyRotation(const Geometry::Vec3D&,const double);
  void setExit(const Geometry::Vec3D&,const Geometry::Vec3D&);

};

}

#endif
 
