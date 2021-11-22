/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTallyInc/resnuclei.h
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
#ifndef flukaSystem_resnuclei_h
#define flukaSystem_resnuclei_h

class SimFLUKA;

namespace flukaSystem
{
/*!
  \class resnuclei
  \version 1.0
  \date November 2018
  \author S. Ansell
  \brief Modification system to allow activation calc.
*/

class resnuclei : public flukaTally
{
 private:

  int AMax;            ///< Max Z of material
  int ZMax;            ///< Max A of material
  size_t cellA;        ///< Cell number
    
 public:

  resnuclei(const int,const int);
  resnuclei(const std::string&,const int,const int);
  resnuclei(const resnuclei&);
  virtual resnuclei* clone() const; 
  resnuclei& operator=(const resnuclei&);
  virtual ~resnuclei();

  /// return fluka name
  virtual std::string getType() const { return "RESNUCLE"; }
  
  void setCell(const int CN) { cellA=CN; }
  void setZaid(const int,const int);
  virtual void write(std::ostream&) const;  
};

}

#endif
