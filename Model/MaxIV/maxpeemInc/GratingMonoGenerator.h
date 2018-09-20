/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/GratingMonoGenerator.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef setVariable_GratingMonoGenerator_h
#define setVariable_GratingMonoGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class GratingMonoGenerator
  \version 1.0
  \author S. Ansell
  \date July 2018
  \brief GratingMonoGenerator for variables
*/

class GratingMonoGenerator
{
 private:
  
  Geometry::Vec3D rotCent;  ///< Rotation centre
  double theta;             ///< Theta angle

  Geometry::Vec3D mOffset;  ///< mirror offset [at zero theta]
  double mWidth;            ///< Width of lower mirror
  double mThick;            ///< Thickness of lower mirror
  double mLength;           ///< Length of lower mirror

  Geometry::Vec3D gOffset;  ///< grating offset [at zero theta]
  double gWidth;            ///< grating width
  double gThick;            ///< grating thickness
  double gLength;           ///< grating length

  double mBaseThick;        ///< mirror Base thickness
  double mBaseWidth;        ///< mirror Base width
  double mBaseLength;       ///< mirror Base length

  double gBaseThick;        ///< Base thickness on grating
  double gBaseWidth;        ///< base width on grating
  double gBaseLength;       ///< Base length on grating
    
  std::string xtalMat;             ///< XStal material
  std::string baseMat;             ///< Base material
    

 public:

  GratingMonoGenerator();
  GratingMonoGenerator(const GratingMonoGenerator&);
  GratingMonoGenerator& operator=(const GratingMonoGenerator&);
  ~GratingMonoGenerator();


  void generateGrating(FuncDataBase&,const std::string&,
		       const double,const double) const;

};

}

#endif
 
