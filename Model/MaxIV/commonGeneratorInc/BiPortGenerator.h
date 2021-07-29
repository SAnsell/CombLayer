/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BiPortGenerator.h
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
#ifndef setVariable_BiPortGenerator_h
#define setVariable_BiPortGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BiPortGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief BiPortGenerator for variables
*/

class BiPortGenerator
{
 private:

  
  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length

  double flangeRadius;        ///< Vertical Flange radius
  double flangeLength;        ///< Vertical Flange length
  double capThick;            ///< Vertical Cap thickness

  double outLength;           ///< Flange on outlet port
  
  double inPortRadius;        ///< Radius on inlet port
  double inWallThick;         ///< Wall thickness on inlet port
  double inFlangeRadius;      ///< Flange on inlet port
  double inFlangeLength;      ///< Flange on inlet port
  
  double outPortRadius;        ///< Radius on outlet port
  double outWallThick;         ///< Wall thickness on outlet port
  double outFlangeRadius;      ///< Flange on outlet port
  double outFlangeLength;      ///< Flange on outlet port

  std::string voidMat;         ///< void material
  std::string wallMat;         ///< Fe material layer
  std::string capMat;          ///< flange cap material layer

 public:

  BiPortGenerator();
  BiPortGenerator(const BiPortGenerator&);
  BiPortGenerator& operator=(const BiPortGenerator&);
  virtual ~BiPortGenerator();

  template<typename CF> void setCF(const double);
  template<typename CF> void setFrontCF();
  template<typename CF> void setBackCF(const double);
  
  void generateBPort(FuncDataBase&,const std::string&,
		     const double) const;

};

}

#endif
 
