/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/CLRTubeGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_CLRTubeGenerator_h
#define setVariable_CLRTubeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CLRTubeGenerator 
  \version 1.0
  \author S. Ansell
  \date March 2021
  \brief Compound refractive lens variable generator
*/

class CLRTubeGenerator 
{
 private:

  double zLift;                  ///< Movement distance
  
  double length;                 ///< Main box length
  double width;                  ///< Main box width
  double height;                 ///< Main box height
  
  double innerLength;            ///< inner Length
  
  double captureWidth;           ///< void in capture space
  double captureHeight;          ///< void in capture space
  double captureDepth;           ///< void in capture space 

  double supportWidth;           ///< support Width
  double supportHeight;          ///< support Height
  double supportDepth;           ///< support Height

  double magWidth;               ///< Magnet Width
  double magHeight;              ///< Magnet Height
  double magDepth;               ///< Magnet Depth

  size_t lensNSize;              ///< Number of lenss
  double lensLength;             ///< lens unit length
  double lensMidGap;             ///< lens Step (void)
  double lensRadius;             ///< lens Radius (focus)
  double lensOuterRadius;        ///< lens Outer Radius 
  double lensSupportRadius;      ///< lens Outer Radius support

  double innerRadius;            ///< inner pipe radius
  double innerThick;             ///< inner pipe thickness

  double portLength;             ///< Port length
  double portRadius;             ///< port Radius 
  double portThick;              ///< thickness of port pipe

  double flangeRadius;           ///< Flange radius
  double flangeLength;           ///< Flange thickness


  std::string voidMat;               ///< Void Material
  std::string lensMat;               ///< Lens material
  std::string lensOuterMat;          ///< Outer support lens Material
  std::string pipeMat;               ///< Pipe Material
  std::string mainMat;               ///< Support Material
  std::string magnetMat;             ///< Magnet material
  std::string flangeMat;             ///< Flange material

 public:

  CLRTubeGenerator();
  CLRTubeGenerator(const CLRTubeGenerator&);
  CLRTubeGenerator& operator=(const CLRTubeGenerator&);
  virtual ~CLRTubeGenerator();

  template<typename T> void setCF();
  template<typename T> void setPortCF(const double);
  
  void generatePump(FuncDataBase&,const std::string&,
		    const bool) const;


};

}

#endif
 
