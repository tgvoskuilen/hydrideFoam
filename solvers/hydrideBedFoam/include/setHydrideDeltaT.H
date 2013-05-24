/*---------------------------------------------------------------------------*/

if (adjustTimeStep)
{
    // Limit the timestep by the reaction rate, the pressurization rate, and
    // the heat flux courant number
    scalar dFMax = VSMALL;
    scalar dpMax = VSMALL;
    scalar DiNum = VSMALL;

    forAll(solidRegions, i)
    {
        #include "setRegionSolidFields.H"

        DiNum = max
        (
            heatFluxCoNum
            (
                solidRegions[i],
                runTime,
                T,
                rho*cp,
                kappa
            ),
            DiNum
        );
    }

    forAll(fluidRegions, i)
    {
        #include "setRegionFluidFields.H"

        dFMax = max(dFMax, max(fvc::ddt(F)).value());
        dpMax = max(dpMax, max(fvc::ddt(p)).value());

        tmp<volScalarField> rhoCp = porosity*rho*thermo.Cp() + (1-porosity)*rhoSolid*CpSolid;

        DiNum = max
        (
            heatFluxCoNum
            (
                fluidRegions[i],
                runTime,
                T,
                rhoCp(),
                keff
            ),
            DiNum
        );
    }

    scalar maxDeltaTF = 0.0005 / dFMax; //the most F can change is 0.0005
    scalar maxDeltaTp = 10 / dpMax;     //the most p can change is 10 Pa
    scalar maxDeltaTT = maxDi / DiNum * runTime.deltaTValue();

    // Set deltaT to the smallest of the max delta T values for F, T, and p
    Info<< "max deltaT = " << maxDeltaT << endl;
    runTime.setDeltaT
    (
        min
        (
            min(min(maxDeltaTF, maxDeltaTp), maxDeltaTT),
            maxDeltaT
        )
    );

    Info<< "deltaT = " <<  runTime.deltaT().value() << endl;
}

// ************************************************************************* //
