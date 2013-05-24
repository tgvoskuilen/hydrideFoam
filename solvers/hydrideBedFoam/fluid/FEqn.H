{
    Info<< "Evaluating reaction rate" << endl;

    dimensionedScalar t0("t0",dimTime,1.0);
    volScalarField Rate = k0 * Foam::exp(-Ea/(Ru*T)) * (Foam::log(p/p0) - dH/(Ru*T) + dS/Ru);

    // Disable dehydriding for now (need hysteresis)
    Rate.max(0.0);
    
    //Calculate source terms using Runge-Kutta
    volScalarField k1 = Rate*(1-F);
    volScalarField k2 = Rate*(1-F-0.5*k1*t0);
    volScalarField k3 = Rate*(1-F-0.5*k2*t0);
    volScalarField k4 = Rate*(1-F-k3*t0);

    volScalarField dF = 1.0/6.0*(k1+2*k2+2*k3+k4);

    scalarField& Fif = F;
    const scalarField& F0 = F.oldTime();
    const scalar deltaT = mesh.time().deltaTValue();

    Fif = F0 + deltaT*dF.field();

    Info<< " Max dFdt = " << Foam::max(fvc::ddt(F)) << endl;
    Info<< " Min/max F: " << Foam::min(F).value() << ' ' << Foam::max(F).value() << endl;
}


