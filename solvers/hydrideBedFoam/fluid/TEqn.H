{
    Info<< "Solving for T" << endl;

    surfaceScalarField phi = linearInterpolate(U) & mesh.Sf();
    volScalarField Sh = mag(dH)/W*(1-porosity)*rhoSolid*wtpct*fvc::ddt(F);

    Info<< "Max T source = " << Foam::max(Sh).value() << endl;

    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        volScalarField rhoCpEff = porosity*rho*thermo.Cp()
             + (1 - porosity)*rhoSolid*CpSolid;

        surfaceScalarField rhoUCp = fvc::interpolate(rho*thermo.Cp()) * phi * porosity;

        tmp<fvScalarMatrix> TEqn
        (
            fvm::ddt(rhoCpEff, T)
          + fvm::div(rhoUCp, T)
         == fvm::laplacian(keff, T) //Calculate with ZBS and kf?
          + porosity*fvc::ddt(p)
          + Sh
        );

        TEqn().relax();
        TEqn().solve(mesh.solver(T.select(finalIter)));

        thermo.correct();    
    }

    Info<< "Min/max T: " << Foam::min(T).value() << ' ' << Foam::max(T).value() << endl;
}


