/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/fissionTally.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
  NList<double> SDfield;                 ///< sd card 

 public:
  
  explicit fissionTally(const int);
  fissionTally(const fissionTally&);
  virtual fissionTally* clone() const; 
  fissionTally& operator=(const fissionTally&);
  virtual ~fissionTally();
  /// ClassName
  virtual std::string className() const 
      { return "fissionTally"; }
      
  void setSD(const double);
  void addCells(const std::vector<int>&);   
  void addIndividualCells(const std::vector<int>&);   
  std::vector<int> getCells() const;
  void clearCells();

  virtual int addLine(const std::string&); 
  virtual void renumberCell(const int,const int);
  virtual int makeSingle();
  virtual void write(std::ostream&) const;
  
};

}

#endif
