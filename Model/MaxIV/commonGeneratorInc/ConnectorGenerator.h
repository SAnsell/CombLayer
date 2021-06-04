/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/ConnectorGenerator.h
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
#ifndef setVariable_ConnectorGenerator_h
#define setVariable_ConnectorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class HRJawGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief ConnectorGenerator for variables
*/

class ConnectorGenerator 
{
 private:

  double radius;                ///< radius of main pipe
  double wallThick;             ///< wall thickness
 
  double innerRadius;           ///< Radius of inner tube
  double innerLength;           ///< Length of inner tube [fractional]

  double outerRadius;           ///< outer shield radius 
  double outerLength;           ///< outer shield length [fractional]

  double flangeRadius;          ///< Flange radius
  double flangeLength;          ///< Flange thickness

  std::string voidMat;          ///< Material for void
  std::string wallMat;          ///< Material
  std::string flangeMat;        ///< Flange material

 public:

  ConnectorGenerator();
  ConnectorGenerator(const ConnectorGenerator&);
  ConnectorGenerator& operator=(const ConnectorGenerator&);
  virtual ~ConnectorGenerator();

  template<typename T> void setCF(const double);
  template<typename T> void setPortCF();

  void setOuter(const double,const double);
  void setPort(const double,const double);
  
  void generatePipe(FuncDataBase&,const std::string&,const double) const;


};

}

#endif
 
