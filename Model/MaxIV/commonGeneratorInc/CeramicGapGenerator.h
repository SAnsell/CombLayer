/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/CeramicGapGenerator.h
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
#ifndef setVariable_CeramicGapGenerator_h
#define setVariable_CeramicGapGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CeramicGapGenerator
  \version 1.0
  \author S. Ansell
  \date June 2020
  \brief CeramicGapGenerator for variables
*/

class CeramicGapGenerator
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]

  double ceramicALen;           ///< Length after flange
  double ceramicWideLen;        ///< Wide length
  double ceramicGapLen;         ///< Length of ceramic insulation
  double ceramicBLen;           ///< Length before bellow

  double ceramicThick;          ///< Small wall thick
  double ceramicWideThick;      ///< Wide ceramic extra

  double pipeLen;               ///< Steel connecter after ceramic
  double pipeThick;             ///< Pipe thickness

  double bellowLen;             ///< Bellow length
  double bellowThick;           ///< Bellow thickness

  double flangeARadius;         ///< Joining Flange radius
  double flangeALength;         ///< Joining Flange length

  double flangeBRadius;         ///< Joining Flange radius
  double flangeBLength;         ///< Joining Flange length

  std::string voidMat;                  ///< void material
  std::string pipeMat;                  ///< main pipe material
  std::string ceramicMat;               ///< cermaic material
  std::string bellowMat;                ///< Bellow material
  std::string flangeMat;                ///< flange material
  std::string outerMat;                 ///< pipe material


 public:

  CeramicGapGenerator();
  CeramicGapGenerator(const CeramicGapGenerator&);
  CeramicGapGenerator& operator=(const CeramicGapGenerator&);
  virtual ~CeramicGapGenerator();

  void setBellowMat(const std::string&,const double);
  template<typename T> void setCF();
  template<typename T> void setAFlangeCF();
  template<typename T> void setBFlangeCF();

  virtual void generateCeramicGap(FuncDataBase&,
				  const std::string&) const;

};

}

#endif
