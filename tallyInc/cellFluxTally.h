/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/cellFluxTally.h
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
#ifndef cellFluxTally_h
#define cellFluxTally_h

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
  
  NList<int>  cellList;                  ///< List of surfaces
  NList<double> FSfield;                 ///< fs card 
  NList<double> SDfield;                 ///< sd card 

 public:
  
  explicit cellFluxTally(const int);
  cellFluxTally(const cellFluxTally&);
  virtual cellFluxTally* clone() const; 
  cellFluxTally& operator=(const cellFluxTally&);
  virtual ~cellFluxTally();
  /// ClassName
  virtual std::string className() const 
      { return "cellFluxTally"; }

      
  void setSD(const double);
  void addCells(const std::vector<int>&);   
  std::vector<int> getCells() const;
  void clearCells();

  virtual int addLine(const std::string&); 
  virtual void renumberCell(const int,const int);
  void writeHTape(const std::string&,const std::string&) const;
  virtual void write(std::ostream&) const;
  
};

}

#endif
