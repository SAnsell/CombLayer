/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WCells.h
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
#ifndef WeightSystem_WCells_h
#define WeightSystem_WCells_h


namespace WeightSystem
{

/*!
  \class WCells
  \brief Cell based information for a particle
  \version 1.0
  \date May 2006
  \author S. Ansell
*/

class WCells : public WForm
{
 private:

  typedef std::map<int,WItem> ItemTYPE;  ///< cellNumber : Weight

  ItemTYPE WVal;       ///< Weight values         

  void writeHead(std::ostream&) const;
  void writeTable(std::ostream&) const;

 public:
  
  WCells();
  WCells(const std::string&);
  WCells(const WCells&);
  WCells& operator=(const WCells&);
  ~WCells();

  bool operator==(const WCells&) const; 
  
  void setCellInfo(const int,const double,const double);

  bool isMasked(const int) const;

  void renumberCell(const int,const int);  
  void populateCells(const std::map<int,MonteCarlo::Qhull*>&);
  void maskCell(const int); 
  void maskCellComp(const int,const size_t); 
  void setWeights(const std::vector<double>&);
  void setWeights(const int,const std::vector<double>&);
  void setWeights(const int,const size_t,const double);
  void scaleWeights(const int,const std::vector<double>&);
  void scaleWeights(const int,const double&);
    
  void rescale(const double,const double);
  void rescale(const int,const int,const double);
  void balanceScale(const std::vector<double>&);

  const std::vector<double>& getWeights(const int) const;

  void writePHITSHead(std::ostream&) const;
  void writePHITS(std::ostream&) const;
  void write(std::ostream&) const;
};

}  

#endif
