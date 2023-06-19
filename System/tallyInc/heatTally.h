/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/heatTally.h
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
#ifndef tallySystem_heatTally_h
#define tallySystem_heatTally_h

namespace tallySystem
{
  /*!
    \class heatTally
    \version 1.0
    \date August 2006
    \author S. Ansell
    \brief Hold a heat flux tally (type 6)

   */
class heatTally : public Tally
{
 private:
  
  int plus;                         ///< general sum flag [+f6]
  NList<int> cellList;           ///< cells to sum
  
 public:
  
  explicit heatTally(const int);
  heatTally(const heatTally&);
  heatTally* clone() const override; 
  heatTally& operator=(const heatTally&);
  ~heatTally() override;
  /// ClassName
  std::string className() const override 
  { return "heatTally"; }
  
  void addCells(const std::vector<int>&);
  void addCells(const std::set<int>&);   
  void clearCells();
  void setPlus(const int V) { plus=V; } ///< Set the + flag
  
  void renumberCell(const int,const int) override;
  int addLine(const std::string&) override; 
  void write(std::ostream&) const override;
  
};

}

#endif
