/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/DomeConnectorGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef setVariable_DomeConnectorGenerator_h
#define setVariable_DomeConnectorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DomeConnectorGenerator
  \version 1.0
  \author S. Ansell
  \date August 2020
  \brief DomeConnectorGenerator for variables
*/

class DomeConnectorGenerator
{
 private:

  double curveRadius;           ///< inner Radius of dome (extent)
  double innerRadius;           ///< inner Radius of straight section
  double curveStep;             ///< Step out from flat start plane
  double joinStep;              ///< length of join dome-flat plane dist
  double flatLen;               ///< length of flat extent
  double plateThick;            ///< Thickness of plate
  double flangeRadius;          ///< Flange radius
  double flangeLen;             ///< Flange length

  std::string wallMat;          ///< Wall material
  std::string voidMat;          ///< inner (Void) material

 public:

  DomeConnectorGenerator();
  DomeConnectorGenerator(const DomeConnectorGenerator&);
  DomeConnectorGenerator& operator=(const DomeConnectorGenerator&);
  ~DomeConnectorGenerator();

  /// set main dome sphere:
  void setSphere(const double R,const double S)
  { curveRadius=R; curveStep=S; }
  void setSphere(const double R,const double F,const double S)
  { curveRadius=R; flangeRadius=F; curveStep=S; }
  void setConnect(const double R,const double S)
  { innerRadius=R; joinStep=S; }
  void setFlat(const double L)
  { flatLen=L; }

  /// set all the lengths
  void setLengths(const double LA,const double LB,const double LC)
  { curveStep=LA;joinStep=LB;flatLen=LC; }
  
  template<typename CF> void setFlangeCF(const double);

  /// setter for material name
  void setMat(std::string M, std::string IM="Void")
  { wallMat= std::move(M); voidMat = std::move(IM);  }

  void generateDome(FuncDataBase&,const std::string&,const size_t) const;

};

}

#endif
