/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/MLMonoGenerator.h
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
#ifndef setVariable_MLMonoGenerator_h
#define setVariable_MLMonoGenerator_h

namespace setVariable
{

/*!
  \class MLMonoGenerator
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Double Mirror Mono arrangement
*/

class MLMonoGenerator 
{
 private:

  double gap;               ///< Gap thickness
  double phiA;               ///< Mono Pitch
  double phiB;               ///< Mono Pitch
      

  double widthA;            ///< Width of block across beam
  double heightA;           ///< Depth into beam
  double lengthA;           ///< Length along beam
  
  double widthB;            ///< Width of block across beam
  double heightB;           ///< Depth into beam
  double lengthB;           ///< Length along beam

  double supportAGap;         ///< Gap after mirror (before back)
  double supportAExtra;       ///< Base/Top extra length
  double supportABackThick;   ///< Back thickness
  double supportABackLength;   ///< Back lenght (in part)
  double supportABase;         ///< Base/Top thickness


  std::string mirrorAMat;             ///< XStal material
  std::string mirrorBMat;             ///< XStal material
  std::string baseAMat;               ///< Base material
  std::string baseBMat;              ///< Base material

 public:

  MLMonoGenerator();
  MLMonoGenerator(const MLMonoGenerator&);
  MLMonoGenerator& operator=(const MLMonoGenerator&);
  virtual ~MLMonoGenerator();


  void generateMono(FuncDataBase&,const std::string&,
		    const double,const double,const double) const;

};

}

#endif
 
