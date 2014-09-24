Double_t binding_energy(Int_t Z = 6,Int_t A = 12)
{
	Double_t a_v=15.835;
	Double_t a_s=18.33;
	Double_t a_c=0.714;
	Double_t a_a=92.80;
	Double_t a_p=11.2;
	
	Int_t dou = 0;
	Double_t B_Z_A=0.;
	Double_t B_v=0.;
	Double_t B_s=0.;
	Double_t B_c=0.;
	Double_t B_a=0.;
	Double_t B_p=0.;

	if(A%2 == 0){
		dou = 1;
		if(Z%2 == 1)
			dou = -1;
	}
	if(A%2 == 1)
		dou = 0;

	B_v = a_v*A;
	B_s = -a_s*pow(A,2/3.);
	B_c = -a_c*Z*Z*pow(A,-1/3.);
	B_a = -a_a*(0.5*A-Z)*(0.5*A-Z)/A;
	B_p = a_p*pow(A,-0.5)*dou;

	B_Z_A = B_v + B_s + B_c + B_a + B_p;
	return B_Z_A;
}

void cal_binding_energy(Int_t Z = 6,Int_t A=12)
{
	double en=0.;
	
	if(A<Z) printf("Sorry! A must be greater than Z!\n");
	en = binding_energy(Z,A);
	printf("Nuclear(%d,%d)",Z,A);
	std::cout<<"Binding Energy = "<<en<<" MeV"<<endl;
	return;
}
