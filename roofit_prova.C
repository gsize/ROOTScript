using namespace RooFit;

int roofit_prova() {
    RooRealVar mreco("mreco", "m_{reco} (MeV)", 10E3, 120E3);
    RooRealVar mtrue("mtrue", "m_{true} (MeV)", 10E3, 120E3);

    RooRealVar true_width("true_width", "true width", 0.5E3, 0.001, 50E3);
    RooGaussian true_dist1("true_dist1", "true1", mtrue, RooConst(90E3), true_width);
    RooGaussian true_dist2("true_dist2", "true2", mtrue, RooConst(30E3), true_width);
    RooAddPdf true_dist("true_dist", "true", RooArgList(true_dist1, true_dist2), RooConst(0.4));
    mtrue.setBins(500000, "cache1");
    mreco.setBins(500000, "cache2");

//    RooRealVar resolution("resolution", "resolution", 1E4, 0.001, 100E3);
    RooFormulaVar relative_resolution("relative_resolution", "sqrt((0.1 / sqrt(@0/1000.))**2 + (0.007)**2)", RooArgList(mtrue));
    RooFormulaVar resolution("resolution", "@0 * @1", RooArgList(relative_resolution, mtrue));
    
//    RooGaussian response("response", "response", mreco, mtrue, resolution);
//    RooGaussian response("response", "response", mtrue, RooConst(0), resolution);
    RooGaussian response("response", "response", mreco, mtrue, resolution);

    

//    RooFFTConvPdf result("result", "result", mtrue, true_dist, response);
//    RooProdPdf result("result", "result", true_dist, Conditional(response, mreco));
//    RooProdPdf result("result", "result", true_dist, response);
    RooProdPdf prod("prod", "prod", true_dist, response);

    RooAbsPdf* result = prod.createProjection(mtrue);

    RooPlot* true_frame = mtrue.frame();
    true_dist.plotOn(true_frame, LineStyle(kDashed));

    RooPlot* true_frame2 = mtrue.frame();
    relative_resolution.plotOn(true_frame2, Precision(1));

    RooPlot* resolution_frame = mtrue.frame();
    resolution.plotOn(resolution_frame);


    RooDataSet* reco_data = result->generate(mreco, 1000);
    RooPlot* reco_frame = mreco.frame();
    reco_data->plotOn(reco_frame);
    result->plotOn(reco_frame);
    

    c1 = new TCanvas();
    true_frame->Draw();

    c2 = new TCanvas();

    reco_frame->Draw();    


    c3 = new TCanvas();
    true_frame2->Draw();

    c4 = new TCanvas();
    resolution_frame->Draw();
}
