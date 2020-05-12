/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/BPMGenerator.h
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
#ifndef setVariable_EArrivalMonGenerator_h
#define setVariable_EArrivalMonGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class EArrivalMonGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief EArrivalMonGenerator for variables
*/

class EArrivalMonGenerator 
{
 private:

  double radius;               ///< inner radius
  double length;               ///< Inner electorn thickness
  double thick;               ///< Inner electorn thickness
  double faceThick;            ///< front/back wall thickness

  double frontPipeILen;        ///< Front pipe inner step
  double frontPipeLen;         ///< Front pipe length (to end of flange)
  double frontPipeRadius;      ///< Front pipe radius 
  double frontPipeThick;       ///< Front pipe thickness 

  double backPipeILen;         ///< Back pipe inner step
  double backPipeLen;          ///< Back pipe length (to end of flange)
  double backPipeRadius;       ///< Back pipe radius 
  double backPipeThick;        ///< Back pipe thickness 

  double flangeRadius;         ///< Joining Flange radius
  double flangeLength;         ///< Joining Flange length

  double windowRotAngle;       ///< Window angle relative to Z 
  double windowRadius;         ///< Window radius
  double windowThick;          ///< window thickness
 
  std::string voidMat;                  ///< void material
  std::string mainMat;                  ///< main material
  std::string windowMat;                ///< window material
  std::string flangeMat;                ///< flange material  


  
 public:

  EArrivalMonGenerator();
  EArrivalMonGenerator(const EArrivalMonGenerator&);
  EArrivalMonGenerator& operator=(const EArrivalMonGenerator&);
  virtual ~EArrivalMonGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();
  
  virtual void generateMon(FuncDataBase&,const std::string&,
			   const double) const;

};

}

#endif
 
