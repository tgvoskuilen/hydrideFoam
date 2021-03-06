/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "heatFluxCoNum.H"
#include "fvc.H"

Foam::scalar Foam::heatFluxCoNum
(
    const fvMesh& mesh,
    const Time& runTime,
    const volScalarField& T,
    const volScalarField& rhoCp,
    const volScalarField& k
)
{
    scalar DiNum = 0.0;
    scalar meanDiNum = 0.0;

    volScalarField fluxByCapacity = Foam::mag(fvc::laplacian(k, T))/(rhoCp*T);
    
    DiNum = gMax(fluxByCapacity.internalField())*runTime.deltaT().value();
    
    meanDiNum = (average(fluxByCapacity)).value()*runTime.deltaT().value();

    Info<< "Region: " << mesh.name() << " Diffusion Number mean: " << meanDiNum
        << " max: " << DiNum << endl;

    return DiNum;
}



// ************************************************************************* //
