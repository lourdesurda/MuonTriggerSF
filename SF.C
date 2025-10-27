#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <iostream>

void ScaleFactor() {
    // Open the histogram file
    TFile *f = TFile::Open("efficiency/root/output_test.root");
    if (!f || f->IsZombie()) {
        std::cerr << "Error: could not open efficiency/root/output_test.root" << std::endl;
        return;
    }

    // Load the 1D histograms
    TH1D *hAll = (TH1D*)f->Get("all_probe_1D");
    TH1D *hPass = (TH1D*)f->Get("pass_probe_1D");

    if (!hAll || !hPass) {
        std::cerr << "Error: required histograms not found!" << std::endl;
        f->Close();
        return;
    }

    // Create a TGraph for efficiency
    int nBins = hAll->GetNbinsX();
    TGraph *g_eff = new TGraph(nBins);
    for (int i = 1; i <= nBins; ++i) {
        double all = hAll->GetBinContent(i);
        double pass = hPass->GetBinContent(i);
        double eff = (all > 0) ? pass / all : 0.0;
        g_eff->SetPoint(i-1, hAll->GetBinCenter(i), eff);
    }

    // Create a canvas and draw
    TCanvas *c1 = new TCanvas("c1", "Muon Scale Factors", 800, 600);
    g_eff->SetMarkerStyle(20);
    g_eff->SetMarkerColor(kBlue);
    g_eff->SetTitle("Muon Efficiency; p_{T} [GeV]; Efficiency");
    g_eff->Draw("AP"); // AP = axis + points

    // Save as PDF
    c1->SaveAs("MuonEfficiency.pdf");

    std::cout << "Efficiency calculated and drawn successfully in MuonEfficiency.pdf" << std::endl;

    f->Close();
}
