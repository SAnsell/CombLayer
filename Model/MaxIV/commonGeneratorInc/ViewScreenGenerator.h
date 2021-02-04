/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/ViewScreenGenerator.h
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
#ifndef setVariable_ViewScreenGenerator_h
#define setVariable_ViewScreenGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ViewScreenGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief ViewScreenGenerator for variables
*/

class ViewScreenGenerator
{
 private:

  double radius;               ///< void radius   
  double height;               ///< void height [+z]
  double depth;                ///< void depth [-z]
  double wallThick;            ///< pipe thickness
  double plateThick;           ///< flange plate thick
  double flangeRadius;         ///< flange radius
  double flangeLength;         ///< flange length

  // front/back port 
  double portARadius;           ///< port Radius
  double portBRadius;           ///< port Radius 
  double portALength;           ///< port length (from centre)
  double portBLength;           ///< port length (from centre)
  
  double flangeARadius;         ///< Joining Flange radius 
  double flangeALength;         ///< Joining Flange length
  double flangeBRadius;         ///< Joining Flange radius 
  double flangeBLength;         ///< Joining Flange length

  // centre port [left 45 ]
  double viewRadius;           ///< Viewing Radius
  double viewLength;           ///< viewing Length [centre to flange end]
  double viewFlangeRadius;     ///< Flange outer Radius
  double viewFlangeLength;     ///< Flange length
  double viewPlateThick;       ///< View Flange Plate thickness

  std::string voidMat;               ///< void material
  std::string wallMat;               ///< wall material

 public:

  ViewScreenGenerator();
  ViewScreenGenerator(const ViewScreenGenerator&);
  ViewScreenGenerator& operator=(const ViewScreenGenerator&);
  virtual ~ViewScreenGenerator();

  template<typename T> void setCF();
  template<typename T> void setPortBCF();

  void generateView(FuncDataBase&,
		    const std::string&) const;

};

}

#endif
