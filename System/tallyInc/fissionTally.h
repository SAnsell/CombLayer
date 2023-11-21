/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/fissionTally.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef tallySystem_fissionTally_h
#define tallySystem_fissionTally_h

namespace tallySystem
{
  /*!
    \class fissionTally
    \version 1.0
    \date April 2014
    \author S. Ansell
    \brief Hold a fission tally (type 7)
   */

class fissionTally : public Tally
{
 private:
  
  NList<int>  cellList;                  ///< List of surfaces
  NList<double> FSfield;                 ///< fs card 

 public:
  
  explicit fissionTally(const int);
  fissionTally(const fissionTally&);
  fissionTally* clone() const override; 
  fissionTally& operator=(const fissionTally&);
  ~fissionTally() override;
  /// ClassName
  std::string className() const override 
      { return "fissionTally"; }
      
  void addCells(const std::vector<int>&);   
  void addIndividualCells(const std::vector<int>&);   
  std::vector<int> getCells() const;
  void clearCells();

  int addLine(const std::string&) override; 
  void renumberCell(const int,const int) override;
  int makeSingle() override;
  void write(std::ostream&) const override;
  
};

}

#endif
