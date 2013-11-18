/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/transComp.h
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
#ifndef transComp_h
#define transComp_h

/*!
  \class transComp
  \version 1.0
  \date October 2009
  \author S. Ansell
  \brief Base class for rotation/translation
*/

class transComp
{
 public:
  
  transComp() {}                     ///< Constructor
  transComp(const transComp&) {}     ///< Copy constructor
  /// Assignment operator
  transComp& operator=(const transComp&) { return *this; }
  virtual ~transComp() {}            ///< Destructor

  ///\cond ABSTRACT
  virtual transComp* clone() const =0;

  virtual Geometry::Vec3D calc(const Geometry::Vec3D&) const =0;
  virtual Geometry::Vec3D calcAxis(const Geometry::Vec3D&) const =0;
  virtual void applyAxis(Geometry::Vec3D&) const =0;
  virtual void apply(Geometry::Surface*) const =0;
  virtual void apply(MonteCarlo::Object*) const =0;
  virtual void apply(Geometry::Vec3D&) const =0;  

  virtual Geometry::Vec3D calcRev(const Geometry::Vec3D&) const =0;
  virtual Geometry::Vec3D calcRevAxis(const Geometry::Vec3D&) const =0;
  virtual void reverseAxis(Geometry::Vec3D&) const =0;
  virtual void reverse(Geometry::Surface*) const =0;
  virtual void reverse(MonteCarlo::Object*) const =0;
  virtual void reverse(Geometry::Vec3D&) const =0;  

  ///\endcond ABSTRACT
};


/*!
  \class transRot
  \version 1.0
  \date March 2010
  \author S. Ansell
  \brief Rotation class for global motion
*/

class transRot : public transComp
{
 private:
  
  Geometry::Vec3D Axis;         ///< Main axis of rotation
  Geometry::Vec3D Offset;       ///< Offset 
  double Angle;                 ///< Angle
  
  Geometry::Quaternion QVec;         ///< Quaternion of rotation
  Geometry::Matrix<double> MRot;     ///< Matrix of rotation

  Geometry::Quaternion QRev;         ///< Quaternion of reverse rotation
  Geometry::Matrix<double> MRev;     ///< Matrix of reverse rotation
  
 public:

  transRot();
  transRot(const Geometry::Vec3D&,const Geometry::Vec3D&,const double);
  transRot(const transRot&);
  transRot& operator=(const transRot&);
  virtual transRot* clone() const;
  virtual ~transRot() {}         ///< Destructor
  
  virtual Geometry::Vec3D calc(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcAxis(const Geometry::Vec3D&) const;
  virtual void applyAxis(Geometry::Vec3D&) const;
  virtual void apply(Geometry::Surface*) const;
  virtual void apply(MonteCarlo::Object*) const;
  virtual void apply(Geometry::Vec3D&) const;

  virtual Geometry::Vec3D calcRev(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcRevAxis(const Geometry::Vec3D&) const;
  virtual void reverseAxis(Geometry::Vec3D&) const;
  virtual void reverse(Geometry::Surface*) const;
  virtual void reverse(MonteCarlo::Object*) const;
  virtual void reverse(Geometry::Vec3D&) const;  
};

/*!
  \class transMirror
  \version 1.0
  \date March 2010
  \author S. Ansell
  \brief Mirror class for global motion
*/

class transMirror : public transComp
{
 private:
  
  Geometry::Plane Mirror;      ///< Mirror Plane
    
 public:

  transMirror();
  transMirror(const Geometry::Plane&);
  transMirror(const transMirror&);
  transMirror& operator=(const transMirror&);
  virtual transMirror* clone() const;
  virtual ~transMirror() {}         ///< Destructor
  
  virtual Geometry::Vec3D calc(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcAxis(const Geometry::Vec3D&) const;
  virtual void applyAxis(Geometry::Vec3D&) const;
  virtual void apply(Geometry::Surface*) const;
  virtual void apply(MonteCarlo::Object*) const;
  virtual void apply(Geometry::Vec3D&) const;

  virtual Geometry::Vec3D calcRev(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcRevAxis(const Geometry::Vec3D&) const;
  virtual void reverseAxis(Geometry::Vec3D&) const;
  virtual void reverse(Geometry::Surface*) const;
  virtual void reverse(MonteCarlo::Object*) const;
  virtual void reverse(Geometry::Vec3D&) const;  

};

/*!
  \class transDisplace
  \version 1.0
  \date March 2010
  \author S. Ansell
  \brief Centre-Centre displacement class for global motion
*/

class transDisplace : public transComp
{
 private:
  
  Geometry::Vec3D Offset;      ///< Offset value
    
 public:

  transDisplace();
  transDisplace(const Geometry::Vec3D&);
  transDisplace(const transDisplace&);
  transDisplace& operator=(const transDisplace&);
  virtual transDisplace* clone() const;
  virtual ~transDisplace() {}         ///< Destructor
  
  virtual Geometry::Vec3D calc(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcAxis(const Geometry::Vec3D&) const;
  virtual void applyAxis(Geometry::Vec3D&) const;
  virtual void apply(Geometry::Surface*) const;
  virtual void apply(MonteCarlo::Object*) const;
  virtual void apply(Geometry::Vec3D&) const;

  virtual Geometry::Vec3D calcRev(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcRevAxis(const Geometry::Vec3D&) const;
  virtual void reverseAxis(Geometry::Vec3D&) const;
  virtual void reverse(Geometry::Surface*) const;
  virtual void reverse(MonteCarlo::Object*) const;
  virtual void reverse(Geometry::Vec3D&) const;  
};


#endif
