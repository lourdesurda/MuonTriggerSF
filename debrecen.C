#include "debrecen.h"
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <iostream>

debrecen::debrecen() : fChain(nullptr), fFile(nullptr),
    all_probe(nullptr), pass_probe(nullptr), fail_probe(nullptr),
    all_probe_1D(nullptr), pass_probe_1D(nullptr), fail_probe_1D(nullptr),
    pt(nullptr), eta(nullptr), pdgId(nullptr), dilep_mass(nullptr),
    Matched_HLT_IsoMu24_v(nullptr), Matched_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ(nullptr)
{
    all_probe   = new TH2D("all_probe","all_probe",50,0,200,50,0,2.5);
    pass_probe  = new TH2D("pass_probe","pass_probe",50,0,200,50,0,2.5);
    fail_probe  = new TH2D("fail_probe","fail_probe",50,0,200,50,0,2.5);

    all_probe_1D  = new TH1D("all_probe_1D","all_probe_1D",50,0,200);
    pass_probe_1D = new TH1D("pass_probe_1D","pass_probe_1D",50,0,200);
    fail_probe_1D = new TH1D("fail_probe_1D","fail_probe_1D",50,0,200);
}

debrecen::~debrecen() {
    if(fFile) fFile->Close();
}

void debrecen::Init(const char* filename) {
    fFile = TFile::Open(filename,"READ");
    if(!fFile || fFile->IsZombie()) {
        std::cerr << "ERROR opening file " << filename << std::endl;
        return;
    }
    fChain = (TTree*)fFile->Get("muon/MuonTree");
    if(!fChain) {
        std::cerr << "ERROR: could not get TTree muon/MuonTree" << std::endl;
        return;
    }

    // Set branch addresses
    fChain->SetBranchAddress("pt", &pt);
    fChain->SetBranchAddress("eta", &eta);
    fChain->SetBranchAddress("pdgId", &pdgId);
    fChain->SetBranchAddress("dilep_mass", &dilep_mass);
    fChain->SetBranchAddress("Matched_HLT_IsoMu24_v", &Matched_HLT_IsoMu24_v);
    fChain->SetBranchAddress("Matched_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", &Matched_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
}

void debrecen::Loop() {
    if(!fChain) return;

    Long64_t nentries = fChain->GetEntries();
    std::cout << "Number of entries: " << nentries << std::endl;

    for(Long64_t i = 0; i < nentries; ++i) {
        fChain->GetEntry(i);

        if(pt->size() < 4) continue;
        if(pt->at(1) < 12) continue;
        if(pt->at(2) < 10) continue;
        if(pt->at(3) < 5) continue;

        for(size_t j = 0; j < dilep_mass->size() && j < 4; ++j) {
            if(dilep_mass->at(j) < 0.8 || (dilep_mass->at(j) > 9 && dilep_mass->at(j) < 11)) continue;
        }

        if(std::abs(pdgId->at(1) + pdgId->at(2) + pdgId->at(3)) != 13) continue;

        all_probe->Fill(pt->at(3), eta->at(3), 1.0);
        all_probe_1D->Fill(pt->at(3), 1.0);

        bool passed = false;
        if(Matched_HLT_IsoMu24_v->at(0) &&
           ((Matched_HLT_IsoMu24_v->at(1) + Matched_HLT_IsoMu24_v->at(2) + Matched_HLT_IsoMu24_v->at(3)) > 0)) {
            passed = true;
        }

        if(passed) {
            pass_probe->Fill(pt->at(3), eta->at(3), 1.0);
            pass_probe_1D->Fill(pt->at(3), 1.0);
        } else {
            fail_probe->Fill(pt->at(3), eta->at(3), 1.0);
            fail_probe_1D->Fill(pt->at(3), 1.0);
        }
    }

    std::cout << "Loop finished. Histograms filled." << std::endl;

    TFile* outfile = new TFile("efficiency/root/output_test.root","RECREATE");
    all_probe->Write();
    pass_probe->Write();
    fail_probe->Write();
    all_probe_1D->Write();
    pass_probe_1D->Write();
    fail_probe_1D->Write();
    outfile->Close();

    std::cout << "Histograms saved to efficiency/root/output_test.root" << std::endl;
}

// Global helper function to simplify execution
void runDebrecen(const char* filename) {
    debrecen d;
    d.Init(filename);
    d.Loop();
}
