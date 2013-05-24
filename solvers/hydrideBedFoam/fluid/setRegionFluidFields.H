    fvMesh& mesh = fluidRegions[i];


    // Universal gas constant
    dimensionedScalar Ru("Ru",dimensionSet(1, 2, -2, -1, -1),8314);
    dimensionedScalar p0("p0",dimPressure,1e5);

    // Load fluid thermo
    basicPsiThermo& thermo = thermoFluid[i];

    // Load fluid fields
    volScalarField& rho = rhoFluid[i];
    volScalarField& kappa = kappaFluid[i]; //?
    volVectorField& U = UFluid[i];
    volScalarField& p = pFluid[i];
    volScalarField& T = TFluid[i];
    volScalarField& F = FFluid[i];
    volScalarField& keff = keffFluid[i];

    // Load bed properties
    dimensionedScalar porosity("porosity",dimless,porosityFluid[i]);
    dimensionedScalar rhoSolid("rhoSolid",dimDensity,rhoSolidFluid[i]);
    dimensionedScalar CpSolid("CpSolid",dimEnergy/dimMass/dimTemperature, CpSolidFluid[i]);
    dimensionedScalar kd("kd",dimArea,kdFluid[i]);

    // Load reaction properties
    dimensionedScalar k0("k0",dimless/dimTime, k0Fluid[i]);
    dimensionedScalar Ea("Ea",dimensionSet(1, 2, -2, 0, -1), EaFluid[i]*1000);
    dimensionedScalar dH("dH",dimensionSet(1, 2, -2, 0, -1), dHFluid[i]*1000);
    dimensionedScalar dS("dS",dimensionSet(1, 2, -2, -1, -1), dSFluid[i]*1000);
    dimensionedScalar wtpct("wtpct",dimless, wtpctFluid[i]);
    dimensionedScalar Ncp("Ncp",dimless, NcpFluid[i]);
    dimensionedScalar W("W",dimensionSet(1,0,0,0,-1), WFluid[i]);

    // What is this for??
    const dimensionedScalar initialMass
    (
        "initialMass",
        dimMass,
        initialMassFluid[i]
    );
