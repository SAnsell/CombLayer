/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/shutterInfo.h
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
#ifndef shutterInfo_h
#define shutterInfo_h

namespace shutterSystem
{
  /// Different faces for a view centre
  enum MODTYPE { groove=0,hydrogen=1,narrow=2,broad=3,chipIR=4 };

/*!
  \class shutterInfo
  \brief Information on each shutter
  \author S. Ansell
  \date March 2009
  \version 1.0
*/

class shutterInfo
{
private:

  //  enum MODTYPE { groove=0,hydrogen=1,narrow=2,broad=3 };

  static const int lSide;     ///< Left side
  static const int rSide;     ///< Right side
  static const int lWindow;   ///< Left window
  static const int rWindow;   ///< Right window
  static const int lTorpedo;  ///<  Left torpedo layer
  static const int rTorpedo;  ///<  Right torpedo layer
  static const int tDTorpedo; ///< Top Decoupled torpedo layer
  static const int bDTorpedo; ///< Base Decoupled torpedo layer
  static const int tCTorpedo; ///< Top Coupled torpeod layer
  static const int bCTorpedo; ///< Base Coupled torpeod layer
  static const int lShutter;  ///< Left shutter (base number)
  static const int rShutter;  ///< Right shutter (base number)
  static const int tCShutter; ///< Top Coupled Shutter layer
  static const int bCShutter; ///< Base Coupled shutter layer
 static const int tCIRShutter; ///< Top ChipIR Shutter layer
  static const int bCIRShutter; ///< Base ChipIR shutter layer
  static const int tDShutter; ///< Top Decoupled Shutter layer
  static const int bDShutter; ///< Base Decoupled shutter layer
  static const int lBulkInner;  ///< Left bulk inner
  static const int rBulkInner;  ///< Right bulk inner
  static const int lBulkOuter;  ///< Left bulk outer
  static const int rBulkOuter;  ///< Right bulk outer
  static const int tBulkInner;  ///< top bulk inner
  static const int bBulkInner;  ///< base bulk inner
  static const int tBulkOuter;  ///< Left bulk outer
  static const int bBulkOuter;  ///< Right bulk outer
  static const int cTop;      ///< Coupled Top
  static const int cBase;     ///< Decoupled Base
  static const int dTop;      ///< decoupled Top
  static const int dBase;     ///< decoupled Base
  static const int vDivide;   ///< Divider down middle of system  
  static double steelInner;  ///< inner steel distance
  static double steelOuter;  ///< outer steel distance

  int number;     ///< Index number
  double angle;   ///< Angle of shutter [radians] 

  MODTYPE type;              ///< Type of shutter [ 0 : coupled / 1 : decouple]
  Geometry::Vec3D ModCentre; ///< Moderator centre [or 0,0,sZ for chip]
  Geometry::Vec3D Axis;      ///< Axis of shutter 
  
  static MODTYPE convType(const int);

public:

  shutterInfo(const int,const double,const int);
  shutterInfo(const shutterInfo&);
  shutterInfo& operator=(const shutterInfo&);
  ~shutterInfo() {} ///< Destructor
  
  /// Set the centre
  void setModCentre(const Geometry::Vec3D& A) { ModCentre=A; }
  /// set shutter steel distance
  void setSteel(const double SI,const double SO)
    {
      steelInner=SI;
      steelOuter=SO;
    }

  /// Access to number
  int getIndex() const { return number; }
  /// Access to type
  int getType() const { return type; }
  /// Access angle (radians)
  double getAngle() const { return angle; }
  /// Access axis
  const Geometry::Vec3D& getAxis() const 
    { return Axis; }
  /// Access Moderator centre
  const Geometry::Vec3D& getModCentre() const 
    { return ModCentre; }

  Geometry::Vec3D getNormal(const Geometry::Vec3D&) const;
  std::string innerSurface() const;
  std::string torpedoTops(const int) const;

  /// Determine if shutter is similar height + side
  bool match(const shutterInfo& A) const 
    { return (A.type==type && A.angle*angle>0); }

  std::string shutterSides(const int) const;
  std::string bulkInnerSides(const int) const;
  std::string bulkOuterSides(const int) const;

  Triple<int> getShutterSides() const;
  std::pair<int,int> getShutterVertSurf() const;

  int calcTallyPlanes(const int,const int,std::vector<int>&) const;
  int getWindowSurf(const int) const;
};

}

#endif
