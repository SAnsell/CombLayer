/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/Weights.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef Weights_h
#define Weights_h

/*!
  \namespace WeightSystem
  \brief Weigh modifications
  \version 1.0
  \author S. Ansell
  \date August 2008
*/

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

  void setCellNumber(const int I) { cellN=I; }       ///< SEt cell nubmer
  void setDensity(const double D) { Density=D; }     ///< Set density
  void setTemp(const double T) { Tmp=T; }            ///< Set temp
  void setWeight(const int,const double);
  void setWeight(const std::vector<double>&);
  void rescale(const double,const double);
  void rescale(const int,const int,const double);
  void mask();
  void mask(const int);
  void write(std::ostream&) const;          
};

/*!
  \class WControl
  \brief Hold control info for a given particle type
  \version 1.0
  \date May 2006
  \author S. Ansell
*/
class WControl : public WForm
{
 private:

  typedef std::map<int,WItem> ItemTYPE;  ///< The map item

  
  char ptype;           ///< Particle type
  std::vector<double> Energy;  ///< Energy windows.
  double wupn;         ///< Max weight befor upsplitting
  double wsurv;        ///< survival possiblitiy
  int maxsp;           ///< max split
  int mwhere;          ///< Check weight -1:col 0:all 1:surf
  int mtime;           ///< Flag to inditace energy(0)/time(1)

  ItemTYPE WVal;          ///< Weight values         

 public:
  
  WControl();
  WControl(const WControl&);
  WControl& operator=(const WControl&);
  ~WControl();

  bool operator==(const WControl&) const; 

  void setParticle(const char c) { ptype=c; }  ///< Set type e.g. n,p, etc.
  int setParam(const double,const double,const int,
		const int,const int);

  void setEnergy(const std::vector<double>&);
  void setCellInfo(const int,const double,const double);

  void renumber(const int,const int);  
  void populateCells(const std::map<int,MonteCarlo::Qhull*>&);
  void maskCell(const int); 
  void maskCellComp(const int,const int); 
  void setWeights(const std::vector<double>&);
  void setWeights(const int,const std::vector<double>&);
  void setWeights(const int,const int,const double);
  void rescale(const double,const double);
  void rescale(const int,const int,const double);
  void writeHead(std::ostream&) const;
  void writeTable(std::ostream&) const;

};


/*!
  \class Weights
  \version 1.0
  \date May 2006
  \author S. Ansell
  \brief Holds and processes the particle weight types

  This needs to have a flexible way of recording the 
  different weights.  So that the average temperature of a given type 
  and the density can be used to map the particle splitting.
*/

class Weights : 
{
 private:

  typedef std::map<char,WControl> CtrlTYPE;  ///< WControl map
  
  int Ngrp;                                  ///< Number of groups
  std::map<char,WControl> WC;                ///< Control info


 public:

  Weights();
  Weights(const Weights&);
  Weights& operator=(const Weights&);
  ~Weights();

  void clear();                             

  void setEnergy(const char,const std::vector<double>&);
  void setCell(const char,const int,const std::vector<double>&);
  void addParticle(const char);

  void populateCells(const std::map<int,MonteCarlo::Qhull*>&);
  void maskCell(const char,const int);
  void maskCellComp(const char,const int,const int); 
  void setWeights(const char,const int,const int,const double); 
  void setWeights(const char,const int,const std::vector<double>&); 
  void balanceScale(const char,const std::vector<double>&);
  void rescale(const char,const int,const int,const double);
  void renumber(const int,const int);

  void write(std::ostream&) const;
};

}  

#endif
