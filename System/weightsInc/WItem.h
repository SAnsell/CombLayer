/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WItem.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef WeightSystem_WItem_h
#define WeightSystem_WItem_h

namespace WeightSystem
{

/*!
  \class WItem
  \brief Holds information on a given weighted cell
  \version 1.0
  \date May 2006
  \author S. Ansell
*/

class WItem
{
 private:

  int cellN;                         ///< Cell number
  double Density;                    ///< Density
  double Tmp;                        ///< Temperature [kelvin]
  std::vector<double> Val;           ///< Values
  
 public:
  
  explicit WItem(const int);
  explicit WItem(const int,const double,const double);
  WItem(const WItem&);
  WItem& operator=(const WItem&);
  ~WItem();

  /// Return cell number:
  int getCellNumber() const { return cellN; }
  int isMasked() const;

  void setCellNumber(const int I) { cellN=I; }       ///< SEt cell nubmer
  void setDensity(const double D) { Density=D; }     ///< Set density
  void setTemp(const double T) { Tmp=T; }            ///< Set temp
  void setWeight(const size_t,const double);
  void setWeight(const std::vector<double>&);
  void scaleWeight(const std::vector<double>&);
  void scaleWeight(const double&);
  void rescale(const double,const double);
  void rescale(const int,const int,const double);
  void mask();
  void mask(const size_t);

  bool isNegative() const;
  /// Access weight
  const std::vector<double>& getWeight() const
    { return Val; }
  
  void write(std::ostream&) const;          
};

std::ostream&
operator<<(std::ostream&,const WItem&);

}  

#endif
