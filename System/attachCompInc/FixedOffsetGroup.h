/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedOffset.h
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
#ifndef attachSystem_FixedOffsetGroup_h
#define attachSystem_FixedOffsetGroup_h

class FuncDataBase;

namespace attachSystem
{

/*!
  \struct offset 
  \version 1.0
  \author S. Ansell
  \date October 2015
  \brief Holding system to control offsets
*/
  
struct offset
{
  double preXYAngle;  ///< pre-xy Angle
  double preZAngle;   ///< pre-z Angle
  double xStep;       ///< x step
  double yStep;       ///< y step
  double zStep;       ///< z step
  double xyAngle;     ///< xy Angle
  double zAngle;      ///< z Angle

  void copy(double&,double&,double&,double&,
	    double&,double&,double&) const;
};
  
/*!
  \class FixedOffsetGroup
  \version 1.0
  \author S. Ansell
  \date  July 2015
  \brief FixedComp derivative which handles x-z step and xy/z angle rotation
    for a set of FixedComp groups
*/

class FixedOffsetGroup  : public FixedGroup
{
 protected:

  double preXYAngle;  ///< pre-shift xy Angle
  double preZAngle;   ///< pre-shift z Angle
  double xStep;       ///< x step
  double yStep;       ///< y step
  double zStep;       ///< z step
  double xyAngle;     ///< xy Angle
  double zAngle;      ///< z Angle  

  /// Mapping of group and local offset
  std::map<std::string,offset> GOffset;
  
  static void populateOffset(const FuncDataBase&,const std::string&,
			     offset&);
  
 public:

  FixedOffsetGroup(const std::string&,const std::string&,
		   const size_t);
  FixedOffsetGroup(const std::string&,const std::string&,
		   const size_t,const std::string&,const size_t);
  FixedOffsetGroup(const std::string&,const std::string&,
		   const size_t,const std::string&,const size_t,
		   const std::string&,const size_t);
  FixedOffsetGroup(const std::string&,const std::string&,const size_t,
		   const std::string&,const size_t,
		   const std::string&,const size_t,
		   const std::string&,const size_t);
  FixedOffsetGroup(const FixedOffsetGroup&);
  FixedOffsetGroup& operator=(const FixedOffsetGroup&);
  virtual ~FixedOffsetGroup() {}     ///< Destructor

  virtual void populate(const FuncDataBase&);

  void applyOffset();
};

}

#endif
 
