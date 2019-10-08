/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BeamMountGenerator.h
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
#ifndef setVariable_BeamPairGenerator_h
#define setVariable_BeamPairGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BeamPairGenerator
  \version 1.0
  \author S. Ansell
  \date October 2019
  \brief BeamPairGenerator for variables
*/

class BeamPairGenerator
{
 private:

  double outLiftA;          ///< lift [when raised from beam cent]
  double outLiftB;          ///< lift [when raised from beam cent]
  double gapA;             ///< Gap from centre point (top +ve up)
  double gapB;             ///< Gap from centre point (base +ve down)
  
  double supportRadius;    ///< Radius of support  
  std::string supportMat;  ///< support material

  double xStepA;           ///< xstep of unit A
  double yStepA;           ///< yStep of unit A
  double xStepB;           ///< xStep of unit B
  double yStepB;           ///< yStep of unit B
  
  double xyAngle;          ///< Rotation angle about Z
  double height;           ///< height total 
  double width;            ///< width accross beam
  double length;           ///< Thickness in normal direction to reflection  
  std::string blockMat;    ///< Base material    

 public:

  BeamPairGenerator();
  BeamPairGenerator(const BeamPairGenerator&);
  BeamPairGenerator& operator=(const BeamPairGenerator&);
  ~BeamPairGenerator();

  void setLift(const double,const double);
  void setXYStep(const double,const double,const double,const double);
  void setGap(const double,const double);
  void setThread(const double,const std::string&);
  void setBlock(const double,const double,
		      const double,const double,const std::string&);
  void generateMount(FuncDataBase&,const std::string&,
		     const int) const;

};

}

#endif
 
