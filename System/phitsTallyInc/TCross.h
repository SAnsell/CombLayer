/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/TCross.h
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
#ifndef phitsSystem_TCross_h
#define phitsSystem_TCross_h

namespace phitsSystem
{
/*!
  \class TCross
  \version 1.0
  \date Septebmer 2018
  \author S. Ansell
  \brief 2d plot for phits
*/

class TCross : public phitsTally
{
 private:
  
  bool fluxFlag;                  ///< Flux or current

  eType energy;                   ///< Energy grid / value
  aType angle;                    ///< Angle grid / value
  std::string axis;               ///< Axis (eng/angle etc)
  int unit;                       ///< Unit 
  int regionA;                    ///< first region
  int regionB;                    ///< second region
    
 public:

  explicit TCross(const int);
  TCross(const TCross&);
  virtual TCross* clone() const;  
  TCross& operator=(const TCross&);
  virtual ~TCross();

  void setFlux() { fluxFlag=1; }  ///< set the tally as a flux tally
  void setRegions(const int A,const int B) { regionA=A; regionB=B;}
  /// set units
  void setUnit(const int A) { unit=A; }
  void setUnit(const std::string&);

  virtual void renumberCell(const int,const int);
  virtual void setEnergy(const eType& A) { energy = A; }
  virtual void setAngle(const aType& A) { angle = A; }
  
  virtual void write(std::ostream&,const std::string&) const;
  
};

}

#endif
