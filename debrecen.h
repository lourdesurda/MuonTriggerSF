#ifndef DEBRECEN_H
#define DEBRECEN_H

#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <vector>

class debrecen {
public:
    debrecen();
    virtual ~debrecen();

    void Init(const char* filename);
    void Loop();

private:
    TFile* fFile;
    TTree* fChain;

    TH2D* all_probe;
    TH2D* pass_probe;
    TH2D* fail_probe;

    TH1D* all_probe_1D;
    TH1D* pass_probe_1D;
    TH1D* fail_probe_1D;

    std::vector<float>* pt;
    std::vector<float>* eta;
    std::vector<float>* pdgId;
    std::vector<float>* dilep_mass;
    float fourlep_mass;
    std::vector<bool>* Matched_HLT_IsoMu24_v;
    std::vector<bool>* Matched_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
    std::vector<bool>* Matched_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
    std::vector<bool>* Matched_HLT_TripleMu_12_10_5;
    int nMuon;
};

#endif
