void DrawEfficiencyExample01(){
  // Example macro to draw efficiencies
  gStyle->SetOptStat(0);
  
  TFile *file = new TFile("AnalysisResults.root", "READ");
  TH1F *hRecoPion = (TH1F*) file  -> Get ("my-example-task/ptHistogramPion");
  TH1F *hRecoKaon = (TH1F*) file  -> Get ("my-example-task/ptHistogramKaon");
  TH1F *hRecoProton = (TH1F*) file  -> Get ("my-example-task/ptHistogramProton");
  TH1F *hGenPion = (TH1F*) file  -> Get ("my-example-task/ptGeneratedPion");
  TH1F *hGenKaon = (TH1F*) file  -> Get ("my-example-task/ptGeneratedKaon");
  TH1F *hGenProton = (TH1F*) file  -> Get ("my-example-task/ptGeneratedProton");
  
  TCanvas *cn = new TCanvas("cn", "", 800,600);
  cn->SetTicks(1,1);
  //cn->SetLogy();
  cn->SetLeftMargin(0.14);
  cn->SetBottomMargin(0.14);
  cn->SetRightMargin(0.03);
  cn->SetTopMargin(0.03);
  
  hRecoPion->Sumw2();
  hRecoKaon->Sumw2();
  hRecoProton->Sumw2();
  hGenPion->Sumw2();
  hGenKaon->Sumw2();
  hGenProton->Sumw2();
  
  hRecoPion->Rebin(10);
  hRecoKaon->Rebin(10);
  hRecoProton->Rebin(10);
  hGenPion->Rebin(10);
  hGenKaon->Rebin(10);
  hGenProton->Rebin(10);
  
  hRecoPion->Divide(hRecoPion, hGenPion, 1.0, 1.0, "B");
  hRecoKaon->Divide(hRecoKaon, hGenKaon, 1.0, 1.0, "B");
  hRecoProton->Divide(hRecoProton, hGenProton, 1.0, 1.0, "B");
  
  hRecoPion->SetTitle("");
  hRecoPion->GetYaxis()->SetTitle("Acceptance x efficiency");
  hRecoPion->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hRecoPion->GetYaxis()->SetTitleSize(0.066);
  hRecoPion->GetXaxis()->SetTitleSize(0.066);
  hRecoPion->GetXaxis()->SetRangeUser(0,4);
  
  hRecoPion->SetLineColor(kGreen+1);
  hRecoKaon->SetLineColor(kRed+1);
  hRecoProton->SetLineColor(kBlue+1);
  hRecoPion->SetMarkerColor(kGreen+1);
  hRecoKaon->SetMarkerColor(kRed+1);
  hRecoProton->SetMarkerColor(kBlue+1);
  hRecoPion->SetMarkerStyle(20);
  hRecoKaon->SetMarkerStyle(20);
  hRecoProton->SetMarkerStyle(20);
  hRecoPion->SetMarkerSize(0.667);
  hRecoKaon->SetMarkerSize(0.667);
  hRecoProton->SetMarkerSize(0.667);
  
  hRecoPion->Draw();
  hRecoKaon->Draw("same");
  hRecoProton->Draw("same");

  TLegend *leg = new TLegend(0.55, 0.23, 0.84, 0.38);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->AddEntry(hRecoPion, "#pi", "l");
  leg->AddEntry(hRecoKaon, "K", "l");
  leg->AddEntry(hRecoProton, "p", "l");
  leg->Draw();
  
  cn->SaveAs("efficiency.pdf");
}
