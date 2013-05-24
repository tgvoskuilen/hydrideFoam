    // Here the "fluid" regions are really "hydride" regions
    // gas phase properties are held by thermo, but bed properties
    // must be loaded from the dictionary too

    // Initialise fluid field pointer lists
    PtrList<basicPsiThermo> thermoFluid(fluidRegions.size());
    PtrList<volScalarField> rhoFluid(fluidRegions.size());
    PtrList<surfaceScalarField> phiFluid(fluidRegions.size()); //ARG!!
    PtrList<volScalarField> kappaFluid(fluidRegions.size());
    PtrList<volScalarField> TFluid(fluidRegions.size());
    PtrList<volScalarField> pFluid(fluidRegions.size());
    PtrList<volVectorField> UFluid(fluidRegions.size());
    PtrList<volScalarField> FFluid(fluidRegions.size());
    PtrList<volScalarField> keffFluid(fluidRegions.size());
    PtrList<compressible::turbulenceModel> turbulence(fluidRegions.size()); //ARG!!

    List<scalar> initialMassFluid(fluidRegions.size());

    //Reaction and bed property scalars
    List<scalar> k0Fluid(fluidRegions.size());
    List<scalar> EaFluid(fluidRegions.size());
    List<scalar> dHFluid(fluidRegions.size());
    List<scalar> dSFluid(fluidRegions.size());
    List<scalar> wtpctFluid(fluidRegions.size());
    List<scalar> NcpFluid(fluidRegions.size());
    List<scalar> WFluid(fluidRegions.size());

    List<scalar> porosityFluid(fluidRegions.size());
    List<scalar> rhoSolidFluid(fluidRegions.size());
    List<scalar> CpSolidFluid(fluidRegions.size());
    List<scalar> kdFluid(fluidRegions.size());


    // Populate fluid field pointer lists
    forAll(fluidRegions, i)
    {
        Info<< "*** Reading fluid mesh thermophysical properties for region "
            << fluidRegions[i].name() << nl << endl;

        IOdictionary reactionDict
        (
            IOobject
            (
                "reactionProperties",
                fluidRegions[i].time().constant(),
                fluidRegions[i],
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE,
                false
            )
        );

        IOdictionary propertyDict
        (
            IOobject
            (
                "thermophysicalProperties",
                fluidRegions[i].time().constant(),
                fluidRegions[i],
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE,
                false
            )
        );

        k0Fluid[i] = readScalar(reactionDict.lookup("k0"));
        EaFluid[i] = readScalar(reactionDict.lookup("Ea"));
        dHFluid[i] = readScalar(reactionDict.lookup("dH"));
        dSFluid[i] = readScalar(reactionDict.lookup("dS"));
        wtpctFluid[i] = readScalar(reactionDict.lookup("wtpct"));
        NcpFluid[i] = readScalar(reactionDict.lookup("Ncp"));
        WFluid[i] = readScalar(reactionDict.lookup("W"));

        porosityFluid[i] = readScalar(propertyDict.lookup("porosity"));
        rhoSolidFluid[i] = readScalar(propertyDict.lookup("rhoSolid"));
        CpSolidFluid[i] = readScalar(propertyDict.lookup("CpSolid"));
        kdFluid[i] = readScalar(propertyDict.lookup("kd"));


        Info<< "    Adding to keffFluid\n" << endl;
        keffFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "keff",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),
                fluidRegions[i],
                dimensionedScalar
                (
                    "keff",
                    dimPower/dimLength/dimTemperature, 
                    readScalar(propertyDict.lookup("keff"))
                )
            )
        );

        Info<< "    Adding to thermoFluid\n" << endl;
        thermoFluid.set
        (
            i,
            basicPsiThermo::New(fluidRegions[i]).ptr()
        );

        Info<< "    Adding to rhoFluid\n" << endl;
        rhoFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "rho",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),
                thermoFluid[i].rho()
            )
        );

        Info<< "    Adding to TFluid\n" << endl;
        TFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "Tfluid",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),
                thermoFluid[i].T()
            )
        );

        Info<< "    Adding to FFluid\n" << endl;
        FFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "F",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                fluidRegions[i]
            )
        );
        FFluid[i].oldTime();


        Info<< "    Adding to kappaFluid\n" << endl;
        kappaFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "kappa",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                thermoFluid[i].Cp()*thermoFluid[i].alpha()
            )
        );

        Info<< "    Adding to UFluid\n" << endl;
        UFluid.set
        (
            i,
            new volVectorField
            (
                IOobject
                (
                    "U",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),
                fluidRegions[i],
                dimensionedVector("U",dimLength/dimTime,vector::zero)
            )
        );


        Info<< "    Adding to phiFluid\n" << endl; //ARG!!
        phiFluid.set
        (
            i,
            new surfaceScalarField
            (
                IOobject
                (
                    "phi",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::READ_IF_PRESENT,
                    IOobject::AUTO_WRITE
                ),
                linearInterpolate(rhoFluid[i]*UFluid[i])
                    & fluidRegions[i].Sf()
            )
        );

        Info<< "    Adding to turbulence\n" << endl; //ARG!!
        turbulence.set
        (
            i,
            autoPtr<compressible::turbulenceModel>
            (
                compressible::turbulenceModel::New
                (
                    rhoFluid[i],
                    UFluid[i],
                    phiFluid[i],
                    thermoFluid[i]
                )
            ).ptr()
        );



        pFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "pFluid",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),
                thermoFluid[i].p()
            )
        );

/*

        // Force p_rgh to be consistent with p
        p_rghFluid[i] = thermoFluid[i].p() - rhoFluid[i]*ghFluid[i];*/

        /*pFluid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "p",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                thermoFluid[i].p()
            )
        );*/

        /*radiation.set
        (
            i,
            radiation::radiationModel::New(thermoFluid[i].T())
        );*/

        initialMassFluid[i] = fvc::domainIntegrate(rhoFluid[i]).value();

        /*Info<< "    Adding to KFluid\n" << endl;
        KFluid.set
        (
            i,
            new volScalarField
            (
                "K",
                0.5*magSqr(UFluid[i])
            )
        );*/

        /*Info<< "    Adding to dpdtFluid\n" << endl;
        dpdtFluid.set
        (
            i,
            new volScalarField
            (
                "dpdt",
                fvc::ddt(thermoFluid[i].p())
            )
        );*/
    }
