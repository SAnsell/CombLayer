/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   flukaTallyInc/resnuclei.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

  static int zOut;     ///< Max Z number
  static int mOut;     ///< Max of N-Z

  int AMax;            ///< Max Z of material
  int ZMax;            ///< Max A of material
  int cellA;           ///< Cell number

public:

  resnuclei(const std::string&,const int,const int);
  resnuclei(const resnuclei&);
  resnuclei* clone() const override;
  resnuclei& operator=(const resnuclei&);
  ~resnuclei() override;

  /// return fluka name
  std::string getType() const override { return "RESNUCLE"; }
  /// set cell for tally
  void setCell(const int CN) { cellA=CN; }
  void setZaid(const int,const int);
  int getZMax() const { return ZMax; }
  int getAMax() const { return AMax; }
  void setMaxAZ(const int,const int);
  void write(std::ostream&) const override;
};

}

#endif
