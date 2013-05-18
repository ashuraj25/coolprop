#include "CoolProp.h"
#include <vector>
#include "CPExceptions.h"
#include "FluidClass.h"
#include "Fluorine.h"

FluorineClass::FluorineClass()
{
    double n[] = {0, 1.51144749736E+00, -2.98666288409E+00, 3.29644905098E+00, -2.98458624201E+00, -2.28688966459E+00, -1.09492193400E+00, 3.04775277572E+00, 1.15689564208E-01, -1.16100171627E+00, 2.95656394476E-01, 7.11482542928E-02, -1.71363832155E-03, 6.65317955515E-04, 5.06026676251E+00, -6.29268435440E+00, 6.17784808739E+00, -1.55366191788E+00, -2.87170687343E+00, 3.17214480494E+00, -2.67969025215E+00, 2.71865479252E+00, -1.07191065039E+00, 1.26597342291E+00, -7.06244695489E-01, 2.68707888826E-01, 5.27251190274E-02, 5.44411481926E-02, 2.28949994105E-04, -5.47908264304E-10, -9.64273224950E-02, 3.68084486225E-04};
	double d[] = {0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 8, 9, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 6, 7, 8, 12, 4, 6, 6};
	double t[] = {0, 0, 0.5, 1.5, 2, 0.5, 1, 0.5, 2, 0.5, 1, 0, 0.5, 0, 1, 3, 4, 5, 1, 4, 5, 1, 3, 5, 4, 4, 1, 1, 5, 30, 20, 25};
	double c[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
	double g[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 1.078102576, 2.156205153, 3.234307729, 3.234307729};

    // Critical parameters
    crit.rho = 15.603*37.99681;
    crit.p = 5172.4;
    crit.T = 144.414;
    crit.v = 1.0/crit.rho;

    // Other fluid parameters
    params.molemass = 37.99681;
    params.Ttriple = 53.4811;
	params.ptriple = 0.23881;
    params.accentricfactor = 0.0449;
    params.R_u = 8.31448;

    // Limits of EOS
	limits.Tmin = params.Ttriple;
    limits.Tmax = 500.0;
    limits.pmax = 100000.0;
    limits.rhomax = 1000000.0*params.molemass;    

	// Residual part
    phirlist.push_back(new phir_power(n,d,t,c,1,13,32));
	phirlist.push_back(new phir_exponential(n,d,t,c,g,14,31,32));

	// Ideal-gas part
	const double a[]={0.0, 3.5011231, -0.60936946e-4/pow(crit.T,4), 0.63196690e-3/pow(crit.T,3), -0.74069617e-4*pow(crit.T,2), 1.012767, 8.9238556};
    const double b[]={0.0, 0, 4, 3, -2};
	std::vector<double> a_v(a, a+sizeof(a)/sizeof(double));
    std::vector<double> b_v(b, b+sizeof(b)/sizeof(double));

	phi0list.push_back(new phi0_lead(0,0));
	phi0list.push_back(new phi0_logtau(-1));
	phi0list.push_back(new phi0_cp0_poly(a_v,b_v,crit.T,298,1,4));
	phi0list.push_back(new phi0_Planck_Einstein(a[5],a[6]));

    name.assign("Fluorine");
    aliases.push_back(std::string("fluorine"));
    REFPROPname.assign("FLUORINE");

	BibTeXKeys.EOS = "deReuck-BOOK-1990";
	BibTeXKeys.CP0 = "deReuck-BOOK-1990";
	//BibTeXKeys.ECS_LENNARD_JONES = "Kiselev-IECR-2005";
	BibTeXKeys.SURFACE_TENSION = "Mulero-JPCRD-2012";
}

double FluorineClass::psat(double T)
{
    // Maximum absolute error is 0.271996 % between 53.481101 K and 144.413990 K
    const double t[]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 23, 27, 30, 34, 39, 45};
    const double N[]={0, 39.766789528099764, -3195.9994386396493, 112125.30646045343, -2289983.2435957235, 30524951.137652785, -282498200.63334149, 1880909335.4262767, -9185010257.6764565, 33103942124.324303, -87405206965.736526, 164501761128.9436, -206121775065.31271, 140272273147.70923, -64043585466.793396, 46073822724.952675, -24600191084.522541, 10929015687.894222, -12112436365.177746, 10838391013.322668, -5425836754.3918419, 1975405029.3848128, -450071720.05528659};
    double summer=0,theta;
    theta=1-T/reduce.T;
    for (int i=1;i<=21;i++)
    {
        summer += N[i]*pow(theta,t[i]/2);
    }
    return reduce.p*exp(reduce.T/T*summer);
}

double FluorineClass::rhosatL(double T)
{
    // Maximum absolute error is 0.052236 % between 53.481101 K and 144.413990 K
    const double t[] = {0, 0.16666666666666666, 0.3333333333333333, 0.5, 0.6666666666666666, 0.8333333333333334, 1.0, 1.1666666666666667, 1.3333333333333333, 1.6666666666666667, 2.0, 2.3333333333333335};
    const double N[] = {0, 1.2896950515325272, -36.259697697303622, 389.20444135863613, -2176.3325373261714, 7366.9110799263972, -15496.555526995258, 19515.494134734785, -12219.632837082365, 3756.9802418741001, -1364.3462914968964, 265.8645475479338};
    double summer=0,theta;
    theta=1-T/reduce.T;
	for (int i=1; i<=11; i++)
	{
		summer += N[i]*pow(theta,t[i]);
	}
	return reduce.rho*(summer+1);
}

double FluorineClass::rhosatV(double T)
{
    // Maximum absolute error is 0.251225 % between 53.481101 K and 144.413990 K
    const double t[] = {0, 0.16666666666666666, 0.3333333333333333, 0.5, 0.6666666666666666, 0.8333333333333334, 1.0, 1.1666666666666667, 1.3333333333333333, 1.5, 1.8333333333333333};
    const double N[] = {0, 5.9616454798694276, -183.74978318636624, 1940.0783642835002, -10472.357185702867, 33021.901889876041, -64463.557858790977, 78264.428052775431, -55910.789427023621, 19118.90631766606, -1327.7852195930525};
    double summer=0,theta;
    theta=1-T/reduce.T;    	
	for (int i=1; i<=10; i++)
	{
		summer += N[i]*pow(theta,t[i]);
	}
	return reduce.rho*exp(reduce.T/T*summer);
}
double FluorineClass::surface_tension_T(double T)
{
	// Mulero, JPCRD 2012
	return 0.03978*pow(1-T/reduce.T,1.218);
}