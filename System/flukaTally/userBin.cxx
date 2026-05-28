/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   flukaTally/userBin.cxx
 *
 * Copyright (c) 2004-2026 by Stuart Ansell and Konstantin Batkov
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "writeSupport.h"
#include "stringCombine.h"
#include "Vec3D.h"

#include "flukaTally.h"
#include "userBin.h"

namespace flukaSystem
{


userBin::userBin(const int ID,const int outID) :
  userBin("mesh",ID,outID)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBin::userBin(const std::string& tallyName,
		   const int ID,const int outID) :
  flukaTally(tallyName,ID,outID),
  meshType(10),particle("208"),
  Pts({0,0,0}),
  hasLinkTransform(false)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBin::userBin(const userBin& A) :
  flukaTally(A),
  meshType(A.meshType),particle(A.particle),
  Pts(A.Pts),minCoord(A.minCoord),
  maxCoord(A.maxCoord),
  hasLinkTransform(A.hasLinkTransform),
  linkOrigin(A.linkOrigin),
  linkX(A.linkX),linkY(A.linkY),linkZ(A.linkZ)
  /*!
    Copy constructor
    \param A :: userBin to copy
  */
{}

userBin&
userBin::operator=(const userBin& A)
  /*!
    Assignment operator
    \param A :: userBin to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
      meshType=A.meshType;
      particle=A.particle;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
      hasLinkTransform=A.hasLinkTransform;
      linkOrigin=A.linkOrigin;
      linkX=A.linkX;
      linkY=A.linkY;
      linkZ=A.linkZ;
    }
  return *this;
}


userBin*
userBin::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userBin(*this);
}

userBin::~userBin()
  /*!
    Destructor
  */
{}

void
userBin::setParticle(const std::string& P)
  /*!
    Set the mesh particle
    \param P :: Partile
  */
{
  particle=P;
  return;
}

void
userBin::setDoseType(const std::string& P,
		     const std::string& D)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: particle to use
    \param D :: set dose type
  */
{
  ELog::RegMethod RegA("userBin","setDoseType");

  flukaTally::setDoseType(P,D);
  return;
}


void
userBin::setLinkTransform(const Geometry::Vec3D& origin,
			  const Geometry::Vec3D& X,
			  const Geometry::Vec3D& Y,
			  const Geometry::Vec3D& Z)
  /*!
    Store the link point world position and orientation axes so that
    write() can emit ROT-DEFIni / ROTPRBIN cards aligning the USRBIN
    mesh with the link point's local frame.
    \param origin :: Link point world position
    \param X :: Link point X axis (world frame)
    \param Y :: Link point Y axis (world frame)
    \param Z :: Link point Z axis (world frame)
  */
{
  hasLinkTransform = true;
  linkOrigin = origin;
  linkX = X;
  linkY = Y;
  linkZ = Z;
  return;
}

void
userBin::setIndex(const std::array<size_t,3>& IDX)
  /*!
    Sets the individual index for each x,y,z
    \param IDX :: array of three object
  */
{
  ELog::RegMethod RegA("userBin","setIndex");
  for(size_t i=0;i<3;i++)
    {
      if (!IDX[i])
	throw ColErr::IndexError<size_t>(IDX[i],i,"IDX[index] zero");
      Pts[i]=IDX[i];
    }

  return;
}


void
userBin::setCoordinates(const Geometry::Vec3D& A,
			const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  ELog::RegMethod RegA("userBin","setCoordinates");

  minCoord=A;
  maxCoord=B;
  // Add some checking here
  for(size_t i=0;i<3;i++)
    {
      if (std::abs(minCoord[i]-maxCoord[i])<Geometry::zeroTol)
	throw ColErr::NumericalAbort(StrFunc::makeString(minCoord)+" ::: "+
				     StrFunc::makeString(maxCoord)+
				     " Equal components");
      if (minCoord[i]>maxCoord[i])
	std::swap(minCoord[i],maxCoord[i]);
    }


  return;
}

void
userBin::writeAuxScore(std::ostream& OX) const
  /*!
    Write an auxScore card
    \param OX :: Ouput stream
  */
{
  if (!auxParticle.empty() && particle=="DOSE-EQ")
    {
      std::ostringstream cx;
      cx<<"AUXSCORE USRBIN "<<auxParticle<<" - "<<keyName
	<<" - - "<<doseType;
      StrFunc::writeFLUKA(cx.str(),OX);
    }
  return;
}


void
userBin::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region.
    When a link transform has been set via setLinkTransform(), three
    ROT-DEFIni cards and one ROTPRBIN card are emitted so that the
    USRBIN mesh follows the link point under geometry rotations.
    The USRBIN extents are expressed in the link point's local frame.
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  const int outputUnit(getOutUnit());

  if (hasLinkTransform)
    {
      const Geometry::Vec3D localMin = minCoord - linkOrigin;
      const Geometry::Vec3D localMax = maxCoord - linkOrigin;

      // Check if every component of each axis is 0 or ±1 (rotation is a
      // multiple of 90°, so the mesh stays axis-aligned in world frame).
      auto isAxComponent = [](double c) -> bool {
        return std::abs(c) < Geometry::zeroTol ||
               std::abs(std::abs(c) - 1.0) < Geometry::zeroTol;
      };
      auto isAxisAligned = [&](const Geometry::Vec3D& v) -> bool {
        return isAxComponent(v[0]) && isAxComponent(v[1]) && isAxComponent(v[2]);
      };

      if (isAxisAligned(linkX) && isAxisAligned(linkY) && isAxisAligned(linkZ))
        {
          // Rotation is axis-aligned: map the 8 local corners to world frame
          // and take the component-wise min/max.  No ROT-DEFIni/ROTPRBIN needed.
          Geometry::Vec3D wMin(0,0,0), wMax(0,0,0);
          bool first = true;
          for (int sx = 0; sx <= 1; sx++)
            for (int sy = 0; sy <= 1; sy++)
              for (int sz = 0; sz <= 1; sz++)
                {
                  const double lx = sx ? localMax[0] : localMin[0];
                  const double ly = sy ? localMax[1] : localMin[1];
                  const double lz = sz ? localMax[2] : localMin[2];
                  Geometry::Vec3D wc(
                    linkOrigin[0] + lx*linkX[0] + ly*linkY[0] + lz*linkZ[0],
                    linkOrigin[1] + lx*linkX[1] + ly*linkY[1] + lz*linkZ[1],
                    linkOrigin[2] + lx*linkX[2] + ly*linkY[2] + lz*linkZ[2]);
                  if (first)
                    {
                      wMin = wMax = wc;
                      first = false;
                    }
                  else
                    {
                      for (int i = 0; i < 3; i++)
                        {
                          if (wc[i] < wMin[i]) wMin[i] = wc[i];
                          if (wc[i] > wMax[i]) wMax[i] = wc[i];
                        }
                    }
                }

          // Permute bin counts from local-frame order to world-frame order.
          // R = [linkX|linkY|linkZ]: for world axis i the nonzero entry in
          // row i identifies which local axis j (0=X,1=Y,2=Z) maps there.
          std::array<size_t, 3> worldPts;
          for (int i = 0; i < 3; i++)
            {
              if (std::abs(linkX[i]) > 0.5)
                worldPts[i] = Pts[0];
              else if (std::abs(linkY[i]) > 0.5)
                worldPts[i] = Pts[1];
              else
                worldPts[i] = Pts[2];
            }

          cx << "USRBIN " << meshType << " " << particle << " "
             << outputUnit << " " << wMax << " " << keyName;
          StrFunc::writeFLUKA(cx.str(), OX);

          cx.str("");
          cx << "USRBIN " << wMin << " ";
          for (size_t i = 0; i < 3; i++)
            cx << worldPts[i] << " ";
          cx << "  & ";
          StrFunc::writeFLUKA(cx.str(), OX);

          writeAuxScore(OX);
        }
      else
        {
          // Non-axis-aligned rotation: emit ROT-DEFIni + ROTPRBIN cards.
          //
          // R = [linkX | linkY | linkZ] (columns = local axes in world coords)
          // R = Rz(alpha)*Ry(beta)*Rx(gamma)  →  R^T = Rx(-gamma)*Ry(-beta)*Rz(-alpha)
          //
          // FLUKA ROT-DEFIni Theta=0: j=3→Rz(-Phi), j=2→Ry(-Phi), j=1→Rx(-Phi).
          // Cards with the same SDUM compose left-to-right (later = outermost).

          const double toDeg = 180.0 / M_PI;
          const double cosB = std::sqrt(linkX[0]*linkX[0] + linkX[1]*linkX[1]);

          double alpha, beta, gamma;
          beta = std::atan2(-linkX[2], cosB) * toDeg;

          if (cosB > 1e-10)
            {
              alpha = std::atan2(linkX[1], linkX[0]) * toDeg;
              gamma = std::atan2(linkY[2], linkZ[2]) * toDeg;
            }
          else
            {
              alpha = 0.0;
              if (linkX[2] < 0)
                gamma = std::atan2(linkY[0], linkY[1]) * toDeg;
              else
                gamma = std::atan2(-linkY[0], linkY[1]) * toDeg;
            }
          if (alpha < 0.0) alpha += 360.0;
          if (beta  < 0.0) beta  += 360.0;
          if (gamma < 0.0) gamma += 360.0;

          const int rotIdx = std::abs(outputUnit);
          const std::string rotName = "R" + std::to_string(rotIdx);

          cx << "ROT-DEFI "
             << (3 + rotIdx * 1000) << " "
             << 0.0 << " " << alpha << " "
             << (-linkOrigin) << " "
             << rotName;
          StrFunc::writeFLUKA(cx.str(), OX);

          if (std::abs(beta) > Geometry::zeroTol)
            {
              cx.str("");
              cx << "ROT-DEFI "
                 << (2 + rotIdx * 1000) << " "
                 << 0.0 << " " << beta << " "
                 << 0.0 << " " << 0.0 << " " << 0.0 << " "
                 << rotName;
              StrFunc::writeFLUKA(cx.str(), OX);
            }

          if (std::abs(gamma) > Geometry::zeroTol)
            {
              cx.str("");
              cx << "ROT-DEFI "
                 << (1 + rotIdx * 1000) << " "
                 << 0.0 << " " << gamma << " "
                 << 0.0 << " " << 0.0 << " " << 0.0 << " "
                 << rotName;
              StrFunc::writeFLUKA(cx.str(), OX);
            }

          cx.str("");
          cx << "USRBIN " << meshType << " " << particle << " "
             << outputUnit << " " << localMax << " " << keyName;
          StrFunc::writeFLUKA(cx.str(), OX);

          cx.str("");
          cx << "USRBIN " << localMin << " ";
          for (size_t i = 0; i < 3; i++)
            cx << Pts[i] << " ";
          cx << "  & ";
          StrFunc::writeFLUKA(cx.str(), OX);

          writeAuxScore(OX);

          cx.str("");
          cx << "ROTPRBIN - " << rotName << " - " << keyName << " - - ";
          StrFunc::writeFLUKA(cx.str(), OX);
        }
    }
  else
    {
      cx << "USRBIN " << meshType << " " << particle << " "
         << outputUnit << " " << maxCoord << " " << keyName;
      StrFunc::writeFLUKA(cx.str(), OX);

      cx.str("");
      cx << "USRBIN " << minCoord << " ";
      for (size_t i=0; i<3; i++)
        cx << Pts[i] << " ";
      cx << "  & ";
      StrFunc::writeFLUKA(cx.str(), OX);
      writeAuxScore(OX);
    }

  flukaTally::write(OX);
  return;
}

}  // NAMESPACE flukaSystem
