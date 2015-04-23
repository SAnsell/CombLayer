/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/surfaceTally.h
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
#ifndef tallySystem_surfaceTally_h
#define tallySystem_surfaceTally_h

namespace tallySystem
{
  /*!
    \class surfaceTally
    \version 1.0
    \date July 2006
    \author S. Ansell
    \brief Hold a surface tally
   */

class surfaceTally : public Tally
  {
   private:

    std::vector<int> SurfList;     ///< List of surfaces
    std::vector<int> FSfield;      ///< Tally Segment [surface divide]
    NList<double> SDfield;         ///< Segment divisor
    NList<int> CellFlag;           ///< Cell flag [CF]
    NList<int> SurfFlag;           ///< Surf flag [SF]

   public:

    explicit surfaceTally(const int);
    surfaceTally(const surfaceTally&);
    virtual surfaceTally* clone() const; 
    surfaceTally& operator=(const surfaceTally&);
    virtual ~surfaceTally();
    virtual std::string className() const 
      { return "surfaceTally"; }

    virtual int addLine(const std::string&); 
    /// Add a surface to the list
    void addSurface(const int N) { SurfList.push_back(N); }

    void setCellFlag(const std::vector<int>&);
    void setSurfFlag(const std::vector<int>&);
    void setSurfDivider(const std::vector<int>&);
    void setCellDivider(const std::vector<int>&);
    
    virtual void renumberCell(const int,const int);
    virtual void renumberSurf(const int,const int);

    virtual void write(std::ostream&) const;
    
  };
};

#endif
