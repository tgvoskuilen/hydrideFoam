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

#include "turbulentThermalContactResistanceFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "mappedPatchBase.H"
#include "regionProperties.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool Foam::turbulentThermalContactResistanceFvPatchScalarField::interfaceOwner
(
    const polyMesh& nbrRegion,
    const polyPatch& nbrPatch
) const
{
    const fvMesh& myRegion = patch().boundaryMesh().mesh();

    if (nbrRegion.name() == myRegion.name())
    {
        return patch().index() < nbrPatch.index();
    }
    else
    {
        const regionProperties& props =
            myRegion.objectRegistry::parent().lookupObject<regionProperties>
            (
                "regionProperties"
            );

        label myIndex = findIndex(props.fluidRegionNames(), myRegion.name());
        if (myIndex == -1)
        {
            label i = findIndex(props.solidRegionNames(), myRegion.name());

            if (i == -1)
            {
                FatalErrorIn
                (
                    "turbulentThermalContactResistanceFvPatchScalarField"
                    "::interfaceOwner(const polyMesh&"
                    ", const polyPatch&)const"
                )   << "Cannot find region " << myRegion.name()
                    << " neither in fluids " << props.fluidRegionNames()
                    << " nor in solids " << props.solidRegionNames()
                    << exit(FatalError);
            }
            myIndex = props.fluidRegionNames().size() + i;
        }
        label nbrIndex = findIndex
        (
            props.fluidRegionNames(),
            nbrRegion.name()
        );
        if (nbrIndex == -1)
        {
            label i = findIndex(props.solidRegionNames(), nbrRegion.name());

            if (i == -1)
            {
                FatalErrorIn
                (
                    "coupleManager::interfaceOwner"
                    "(const polyMesh&, const polyPatch&) const"
                )   << "Cannot find region " << nbrRegion.name()
                    << " neither in fluids " << props.fluidRegionNames()
                    << " nor in solids " << props.solidRegionNames()
                    << exit(FatalError);
            }
            nbrIndex = props.fluidRegionNames().size() + i;
        }

        return myIndex < nbrIndex;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::turbulentThermalContactResistanceFvPatchScalarField::
turbulentThermalContactResistanceFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    temperatureCoupledBase(patch(), "undefined", "undefined-K"),
    neighbourFieldName_("undefined-neighbourFieldName"),
    Rtc_(dimensionedScalar("Rtc",dimArea*dimTemperature/dimPower,VSMALL))
{}


Foam::turbulentThermalContactResistanceFvPatchScalarField::
turbulentThermalContactResistanceFvPatchScalarField
(
    const turbulentThermalContactResistanceFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(ptf, p, iF, mapper),
    temperatureCoupledBase(patch(), ptf.KMethod(), ptf.KName()),
    neighbourFieldName_(ptf.neighbourFieldName_),
    Rtc_(ptf.Rtc_)
{}


Foam::turbulentThermalContactResistanceFvPatchScalarField::
turbulentThermalContactResistanceFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF, dict),
    temperatureCoupledBase(patch(), dict),
    neighbourFieldName_(dict.lookup("neighbourFieldName")),
    Rtc_(dict.lookup("Rtc"))
{
    if (!isA<mappedPatchBase>(this->patch().patch()))
    {
        FatalErrorIn
        (
            "turbulentThermalContactResistanceFvPatchScalarField::"
            "turbulentThermalContactResistanceFvPatchScalarField\n"
            "(\n"
            "    const fvPatch& p,\n"
            "    const DimensionedField<scalar, volMesh>& iF,\n"
            "    const dictionary& dict\n"
            ")\n"
        )   << "\n    patch type '" << p.type()
            << "' not type '" << mappedPatchBase::typeName << "'"
            << "\n    for patch " << p.name()
            << " of field " << dimensionedInternalField().name()
            << " in file " << dimensionedInternalField().objectPath()
            << exit(FatalError);
    }
}


Foam::turbulentThermalContactResistanceFvPatchScalarField::
turbulentThermalContactResistanceFvPatchScalarField
(
    const turbulentThermalContactResistanceFvPatchScalarField& wtcsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(wtcsf, iF),
    temperatureCoupledBase(patch(), wtcsf.KMethod(), wtcsf.KName()),
    neighbourFieldName_(wtcsf.neighbourFieldName_),
    Rtc_(wtcsf.Rtc_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::turbulentThermalContactResistanceFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    // Since we're inside initEvaluate/evaluate there might be processor
    // comms underway. Change the tag we use.
    int oldTag = UPstream::msgType();
    UPstream::msgType() = oldTag+1;

    // Get the coupling information from the mappedPatchBase
    const mappedPatchBase& mpp = refCast<const mappedPatchBase>
    (
        this->patch().patch()
    );
    const polyMesh& nbrMesh = mpp.sampleMesh();
    const fvPatch& nbrPatch = refCast<const fvMesh>
    (
        nbrMesh
    ).boundary()[mpp.samplePolyPatch().index()];

    // Force recalculation of mapping and schedule
    const mapDistribute& distMap = mpp.map();
    (void)distMap.schedule();

    tmp<scalarField> intFld = patchInternalField();

    if (interfaceOwner(nbrMesh, nbrPatch.patch()))
    {
        // Calculate the temperature by harmonic averaging including a wall
        // thermal contact resistance term
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        const turbulentThermalContactResistanceFvPatchScalarField& nbrField =
        refCast<const turbulentThermalContactResistanceFvPatchScalarField>
        (
            nbrPatch.lookupPatchField<volScalarField, scalar>
            (
                neighbourFieldName_
            )
        );

        // Swap to obtain full local values of neighbour internal field
        scalarField nbrIntFld(nbrField.patchInternalField());
        distMap.distribute(nbrIntFld);


    // Calculate myA and nbrA (Ai = deltai/(Ki*Rtc+VSMALL) )

        scalarField myA = 1.0 / (K(*this)*patch().deltaCoeffs()*Rtc_.value() + VSMALL);

        // Swap to obtain full local values of neighbour delta/(K*Rtc)
        scalarField nbrA(1.0/(nbrField.K(nbrField)*nbrPatch.deltaCoeffs()*Rtc_.value() + VSMALL));
        distMap.distribute(nbrA);

        // Calculate myTwall and nbrTwall
        
        scalarField myTwall = (intFld() + myA/(1+nbrA)*nbrIntFld) / (1 + myA - myA*nbrA/(1+nbrA));
        scalarField nbrTwall = (nbrIntFld + nbrA*myTwall) / (1 + nbrA);

        scalarField wallDT = myTwall - nbrTwall;

        Info<< "Max wall DT = " << Foam::max(Foam::mag(wallDT)) << endl;

        // Set Twalls (they will NOT be equal unless Rtc = 0)

        // Assign to me
        fvPatchScalarField::operator=(myTwall);

        // Distribute back and assign to neighbour
        distMap.reverseDistribute(nbrField.size(), nbrTwall);
        const_cast<turbulentThermalContactResistanceFvPatchScalarField&>
        (
            nbrField
        ).fvPatchScalarField::operator=(nbrTwall);
    }

    // Restore tag
    UPstream::msgType() = oldTag;

    fixedValueFvPatchScalarField::updateCoeffs();
}


void Foam::turbulentThermalContactResistanceFvPatchScalarField::write
(
    Ostream& os
) const
{
    fixedValueFvPatchScalarField::write(os);
    os.writeKeyword("neighbourFieldName")<< neighbourFieldName_
        << token::END_STATEMENT << nl;
    temperatureCoupledBase::write(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

makePatchTypeField
(
    fvPatchScalarField,
    turbulentThermalContactResistanceFvPatchScalarField
);

} // End namespace Foam

// ************************************************************************* //
