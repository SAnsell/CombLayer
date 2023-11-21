/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/surfaceTally.h
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
#ifndef tallySystem_surfaceTally_h
#define tallySystem_surfaceTally_h

namespace tallySystem
{
  /*!
    \class surfaceTally
    \version 1.0
    \date July 2006
    \author S. Ansell
    \brief Hold a surface/surface flux tally
   */

class surfaceTally : public Tally
  {
   private:

    bool fluxType;                 ///< Make a flux tally
    std::vector<int> SurfList;     ///< List of surfaces
    std::vector<int> FSfield;      ///< Tally Segment [surface divide]
    NList<double> SDfield;         ///< Segment divisor
    NList<int> CellFlag;           ///< Cell flag [CF]
    NList<int> SurfFlag;           ///< Surf flag [SF]

   public:

    surfaceTally(const bool,const int);
    explicit surfaceTally(const int);
    surfaceTally(const surfaceTally&);
    surfaceTally* clone() const override; 
    surfaceTally& operator=(const surfaceTally&);
    ~surfaceTally() override;
    std::string className() const override 
      { return "surfaceTally"; }

    int addLine(const std::string&) override; 
    /// Add a surface to the list
    void addSurface(const int N) { SurfList.push_back(N); }

    void setCellFlag(const std::vector<int>&);
    void setSurfFlag(const std::vector<int>&);
    void setSurfDivider(const std::vector<int>&);
    void setCellDivider(const std::vector<int>&);
    
    void renumberCell(const int,const int) override;
    void renumberSurf(const int,const int) override;

    void write(std::ostream&) const override;
    
  };
};

#endif
