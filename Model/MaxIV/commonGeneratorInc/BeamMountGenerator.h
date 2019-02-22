/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BeamMountGenerator.h
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
#ifndef setVariable_BeamMountGenerator_h
#define setVariable_BeamMountGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BeamMountGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BeamMountGenerator for variables
*/

class BeamMountGenerator
{
 private:

  int blockType;          ///< Type of block [0:none/1:centre/2:edge]
  double outLift;         ///< Amount to lift [when raised]
  double beamLift;        ///< Amount to lift by in the beam
  
  double supportRadius;    ///< Radius of support  
  std::string supportMat;          ///< support material

  double xyAngle;          ///< Rotation angle about Z
  double height;           ///< height total 
  double width;            ///< width accross beam
  double length;           ///< Thickness in normal direction to reflection  
  std::string blockMat;    ///< Base material    

 public:

  BeamMountGenerator();
  BeamMountGenerator(const BeamMountGenerator&);
  BeamMountGenerator& operator=(const BeamMountGenerator&);
  ~BeamMountGenerator();

  void setThread(const double,const std::string&);

  void setLift(const double,const double);
  void setOutLift(const double L) { outLift=L; }
  void setBeamLift(const double L) { beamLift=L; }

  void setCentreBlock(const double,const double,
		      const double,const double,const std::string&);
  void setEdgeBlock(const double,const double,
		    const double,const double,const std::string&);
  void generateMount(FuncDataBase&,const std::string&,
		     const int) const;

};

}

#endif
 
