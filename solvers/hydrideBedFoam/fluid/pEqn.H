{
    Info<< "Solving pressure equation" << endl;

    tmp<volScalarField> Sp = fvc::ddt(T)/T;
    tmp<volScalarField> Su = (1-porosity)*rhoSolid*wtpct*fvc::ddt(F)/thermo.psi();

    tmp<volScalarField> Stot = Sp()*p + Su();

    Info<< "Max Stot*dT = " << Foam::max(Stot()).value() * mesh.time().deltaTValue() << endl;


    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        
        tmp<fvScalarMatrix> pEqn
        (
            fvm::ddt(p)
         == fvm::laplacian(kd*p/(porosity*thermo.mu()), p)
          + fvm::Sp(Sp(), p)
          - Su()
        );

        pEqn().relax();
        pEqn().solve(mesh.solver(p.select(finalIter)));

    }

    thermo.correct();

    U = -kd/(porosity*thermo.mu()) * fvc::grad(p);

    Info<< "U min,max = " << Foam::min(Foam::mag(U)).value() << ", "
        << Foam::max(Foam::mag(U)).value() << endl;

    Info<< "p min,max = " << Foam::min(p).value() << ", "
        << Foam::max(p).value() << endl;
}
