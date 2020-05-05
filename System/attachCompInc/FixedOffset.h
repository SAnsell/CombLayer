/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedOffset.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef attachSystem_FixedOffset_h
#define attachSystem_FixedOffset_h

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

class FixedOffset  : public FixedComp
{
 protected:

  double preXYAngle;     ///< pre-shift xy Angle
  double preZAngle;      ///< pre-shift z Angle
  double xStep;          ///< x step
  double yStep;          ///< y step
  double zStep;          ///< z step
  double xyAngle;        ///< xy Angle
  double zAngle;         ///< z Angle
  
 public:

  explicit FixedOffset(const size_t);
  FixedOffset(const size_t,const std::string&);
  FixedOffset(const std::string&,const size_t);
  FixedOffset(const std::string&,const size_t,const size_t);
  FixedOffset(const FixedOffset&);
  FixedOffset& operator=(const FixedOffset&);
  virtual ~FixedOffset() {}     ///< Destructor

  virtual void populate(const FuncDataBase&);
  virtual void populate(const std::map<std::string,
			std::vector<std::string>>&);
  
  virtual void populate(const std::string&,const FuncDataBase&);

  virtual void createUnitVector(const attachSystem::FixedComp&,
				const long int);
  virtual void createCentredUnitVector
    (const attachSystem::FixedComp&,const long int,const double);
  
  void setOffset(const double,const double,const double);
  void setPreRotation(const double,const double);
  void setRotation(const double,const double);
  void applyOffset();

  void linkShift(const size_t);
  void linkAngleRotate(const size_t);
  
};

}

#endif
 
