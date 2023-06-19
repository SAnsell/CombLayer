/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/cellFluxTally.h
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
#ifndef tallySystem_cellFluxTally_h
#define tallySystem_cellFluxTally_h

namespace tallySystem
{
  /*!
    \class cellFluxTally
    \version 1.0
    \date July 2006
    \author S. Ansell
    \brief Hold a cell flux tally (type 4)
   */

class cellFluxTally : public Tally
{
 private:
  
  NList<int>  cellList;                  ///< List of cells
  NList<double> FSfield;                 ///< fs card 

 public:
  
  explicit cellFluxTally(const int);
  cellFluxTally(const cellFluxTally&);
  cellFluxTally* clone() const override; 
  cellFluxTally& operator=(const cellFluxTally&);
  ~cellFluxTally() override;
  /// ClassName
  std::string className() const override 
      { return "cellFluxTally"; }

      
  void addCells(const std::vector<int>&);
  void addCells(const std::set<int>&);   
  void addIndividualCells(const std::vector<int>&);   
  std::vector<int> getCells() const;
  void clearCells();
  int setSDField(const double) override;
  int mergeTally(const Tally&) override;

  
  int addLine(const std::string&) override; 
  void renumberCell(const int,const int) override;
  int makeSingle() override;
  void writeHTape(const std::string&,const std::string&) const;
  void write(std::ostream&) const override;

  
};

}

#endif
