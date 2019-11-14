/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MirrorGenerator.h
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
#ifndef setVariable_MirrorGenerator_h
#define setVariable_MirrorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MirrorGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief MirrorGenerator for variables
*/

class MirrorGenerator
{
 private:

  double xAngle;         ///< x Angle
  double yAngle;         ///< y Angle
  double zAngle;         ///< z Angle
   
  double radius;         ///< mirror radius

  double length;         ///< length along beam
  double thick;          ///< Thickness in normal direction to reflection
  double width;          ///< width accross beam

  double baseTop;          ///< Base thickness
  double baseDepth;        ///< Base thickness
  double baseGap;          ///< Gap
  double baseOutWidth;     ///< Extra width/length of base


  std::string mirrMat;         ///< xtal material
  std::string baseMat;         ///< base material
    
 public:

  MirrorGenerator();
  MirrorGenerator(const MirrorGenerator&);
  MirrorGenerator& operator=(const MirrorGenerator&);
  ~MirrorGenerator();

  void setPrimaryAngle(const double,const double,const double);
  void setRadius(const double);
  // L/T/W
  void setPlate(const double,const double,const double);

  void setMaterial(const std::string&,const std::string&);
  void setSupport(const double,const double,
		  const double,const double);
  
  
  void generateMirror(FuncDataBase&,const std::string&,
		      const double,const double,const double,
		      const double,const double,
		      const double) const;

};

}

#endif
 
