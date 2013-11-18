/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/LinearComp.h
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
#ifndef attachSystem_LinearComp_h
#define attachSystem_LinearComp_h

namespace attachSystem
{
/*!
  \class LinearComp
  \version 2.0
  \author S. Ansell
  \date March 2010
  \brief Specialization of FixedComp

  FixedComp is a multi attach object, but Linear reduces
  that to two attachments. 
  Provides a simplified interface to FixedComp
*/

class LinearComp : public FixedComp
{
 protected:

  void createUnitBeamVector(const LinearComp&);

 public:

  LinearComp(const std::string&);
  LinearComp(const LinearComp&);
  LinearComp& operator=(const LinearComp&);
  virtual ~LinearComp() {}     ///< Destructor
  
  
  /// Accessor to NormIn
  virtual const Geometry::Vec3D& getStartNorm() const 
    { return Y; }  

  /// Accessor to Exit
  //  virtual const Geometry::Vec3D& getStart() const 
  //    { return Origin; }  

  virtual const Geometry::Vec3D& getExitNorm() const;
  virtual const Geometry::Vec3D& getExit() const;

  /// Accessor to output point [of actual beam]
  virtual const Geometry::Vec3D& getBeamAxis() const 
    { return getExitNorm(); }  
  /// Exit point of beam if different from true axis
  virtual const Geometry::Vec3D& getBeamExit() const 
    { return getExit(); }  

  virtual int getExitSurface() const;
  
  virtual void setExit(const Geometry::Vec3D&,const Geometry::Vec3D&);
  virtual void setExitSurf(const int);


  ///\cond ABSTRACT
  virtual int exitWindow(const double,std::vector<int>&,
			 Geometry::Vec3D&) const =0;

  ///\endcond ABSTRACT
};

}

#endif
 
