/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedOffset.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
  \class FixedOffset
  \version 1.0
  \author S. Ansell
  \date  2015
  \brief FixedComp derivative which handles x-z step and xy/z angle rotation
*/

class FixedOffsetGroup  : public FixedGroup
{
 protected:

  double xStep;       ///< x step
  double yStep;       ///< y step
  double zStep;       ///< z step
  double xyAngle;     ///< xy Angle
  double zAngle;      ///< z Angle
  
 public:

  FixedOffsetGroup(const std::string&,const std::string&,
		   const size_t);
  FixedOffsetGroup(const std::string&,const std::string&,
		   const size_t,const std::string&,const size_t);
  FixedOffsetGroup(const std::string&,const std::string&,
		   const size_t,const std::string&,const size_t,
		   const std::string&,const size_t);
  FixedOffsetGroup(const FixedOffsetGroup&);
  FixedOffsetGroup& operator=(const FixedOffsetGroup&);
  virtual ~FixedOffsetGroup() {}     ///< Destructor

  virtual void populate(const FuncDataBase&);

  void applyOffset();
};

}

#endif
 
