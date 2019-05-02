/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/EPCombineGenerator.h
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
#ifndef setVariable_EPCombineGenerator_h
#define setVariable_EPCombineGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class EPCombineGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief EPCombineGenerator for variables
*/

class EPCombineGenerator 
{
 private:

  double length;                ///< frame length

  double photonXStep;            ///< Initial photon gap
  double electronXStep;          ///< Initial electorn gap  
  double photonXYAngle;          ///< XY angle of electron beam to proton
  double electronXYAngle;        ///< XY angle of electron beam to proton

  double electronRadius;         ///< Electron radius
  double photonRadius;           ///< Photon radius

  double skinThick;              ///< skin thickness
  
  double wallXStep;              ///< Outer Wall centre offset
  double wallStartLen;           ///< Outer wall start length
  double wallWidth;              ///< Outer wall box
  double wallHeight;             ///< Outer wall box

  double flangeAXStep;            ///< flange x step
  double flangeARadius;           ///< flange radius
  double flangeALength;           ///< flange length

  double flangeBXStep;            ///< back flange xstep
  double flangeBRadius;           ///< back flange radius
  double flangeBLength;           ///< back flange length

  std::string voidMat;                    ///< void material
  std::string wallMat;                    ///< wall material
  std::string flangeMat;                  ///< Port material

 public:

  EPCombineGenerator();
  EPCombineGenerator(const EPCombineGenerator&);
  EPCombineGenerator& operator=(const EPCombineGenerator&);
  virtual ~EPCombineGenerator();
  
  
  virtual void generatePipe(FuncDataBase&,const std::string&) const;

};

}

#endif
 
