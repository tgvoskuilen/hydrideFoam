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

Class
    turbulentThermalContactResistanceFvPatchScalarField

Description
    Harmonic fixed value boundary condition for temperature, to be used
    for heat-transfer on back-to-back baffles.

    If my temperature is T1, heat conductivity K1 and neighbour is T2,K2

    both sides get fixedValue (K1/dx1*T1 + K2/dx2*T2)/(K1/dx1+K2/dx2)

    Example usage:
        myInterfacePatchName
        {
            type    compressible::turbulentTemperatureCoupledBaffle;
            neighbourFieldName  T;
            K                   lookup;
            KName               K; // or none
            value               uniform 300;
        }

    Needs to be on underlying mapped(Wall)FvPatch.

    Note: see temperatureCoupledBase on ways to specify K.

    Note: runs in parallel with arbitrary decomposition. Uses mapped
    functionality to calculate exchange.

    Note: lags interface data so both sides use same data.
    - problem: schedule to calculate average would interfere
    with standard processor swaps.
    - so: updateCoeffs sets both to same Twall. Only need to do
    this for last outer iteration but don't have access to this.

SourceFiles
    turbulentThermalContactResistanceFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#ifndef turbulentThermalContactResistanceFvPatchScalarField_H
#define turbulentThermalContactResistanceFvPatchScalarField_H

#include "fixedValueFvPatchFields.H"
#include "temperatureCoupledBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
        Class turbulentThermalContactResistanceFvPatchScalarField Declaration
\*---------------------------------------------------------------------------*/

class turbulentThermalContactResistanceFvPatchScalarField
:
    public fixedValueFvPatchScalarField,
    public temperatureCoupledBase
{
    // Private data

        //- Name of field on the neighbour region
        const word neighbourFieldName_;

        //- Thermal contact resistance of the interface
        dimensionedScalar Rtc_;

    // Private Member Functions

        //- Am I or neighbour owner of interface
        bool interfaceOwner(const polyMesh&, const polyPatch&) const;


public:

    //- Runtime type information
    TypeName("compressible::turbulentThermalContactResistance");


    // Constructors

        //- Construct from patch and internal field
        turbulentThermalContactResistanceFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        turbulentThermalContactResistanceFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  turbulentThermalContactResistanceFvPatchScalarField onto a new patch
        turbulentThermalContactResistanceFvPatchScalarField
        (
            const turbulentThermalContactResistanceFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchScalarField> clone() const
        {
            return tmp<fvPatchScalarField>
            (
                new turbulentThermalContactResistanceFvPatchScalarField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        turbulentThermalContactResistanceFvPatchScalarField
        (
            const turbulentThermalContactResistanceFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new turbulentThermalContactResistanceFvPatchScalarField
                (
                    *this,
                    iF
                )
            );
        }


    // Member functions

        //- Update the coefficients associated with the patch field
        virtual void updateCoeffs();

        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
