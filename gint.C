void gint() {
      TF1 *g = new TF1("g","gaus",-5,5);
      g->SetParameters(1,0,1);
      //default gaus integration method uses 6 points
      //not suitable to integrate on a large domain
      double r1 = g->Integral(0,5);
      double r2 = g->Integral(0,1000);

      //try with user directives computing more points
      Int_t np = 1000;
      double *x=new double[np];
      double *w=new double[np];
      g->CalcGaussLegendreSamplingPoints(np,x,w,1e-15);
      double r3 = g->IntegralFast(np,x,w,0,5);
      double r4 = g->IntegralFast(np,x,w,0,1000);
      double r5 = g->IntegralFast(np,x,w,0,10000);
      double r6 = g->IntegralFast(np,x,w,0,100000);
      printf("g->Integral(0,5)               = %g\n",r1);
      printf("g->Integral(0,1000)            = %g\n",r2);
      printf("g->IntegralFast(n,x,w,0,5)     = %g\n",r3);
      printf("g->IntegralFast(n,x,w,0,1000)  = %g\n",r4);
      printf("g->IntegralFast(n,x,w,0,10000) = %g\n",r5);
      printf("g->IntegralFast(n,x,w,0,100000)= %g\n",r6);
      delete [] x;
      delete [] w;
   }

