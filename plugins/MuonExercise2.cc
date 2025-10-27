
// system include files
#include <memory>
#include <iomanip>
#include <TTree.h>
#include <cmath>
#include <TLorentzVector.h>
#include <TVector3.h>
#include "TrackingTools/IPTools/interface/IPTools.h"
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "Math/GenVector/VectorUtil.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

//
// class declaration
//

class MuonExercise2 : public edm::one::EDAnalyzer<edm::one::SharedResources> {

  public:

    explicit MuonExercise2(const edm::ParameterSet&);
    ~MuonExercise2();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:

    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;
    void matchTrigger(const edm::Event&, edm::Handle<edm::TriggerResults>&, const pat::Muon&, std::map<std::string, bool>&);
    // void matchTrigger(const edm::Event&, edm::Handle<edm::TriggerResults>&, const pat::Muon&, bool&);
    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

    // ----------member data ---------------------------
    edm::EDGetTokenT<pat::MuonCollection> muonCollToken;
    edm::EDGetTokenT<edm::TriggerResults> trigResultsToken;
    edm::EDGetTokenT<reco::TransientTrack> TransientTrackToken;
    edm::EDGetTokenT<std::vector<reco::Vertex>> vertexToken_;


    TTree *tree;
    // float pt;
    int nMuon;
    float fourlep_mass;
    std::vector<float> pt;
    std::vector<float> eta;
    std::vector<float> pdgId;
    std::vector<float> phi;
    std::vector<float> dilep_mass;
    std::map<std::string, std::vector<bool>> muonMatched;
    const std::vector<std::string> triggerNames = {
        "HLT_IsoMu24_v",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ",
        "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL",
        "HLT_TripleMu_12_10_5",
    };

      
};

//
// constructors and destructor
//
MuonExercise2::MuonExercise2(const edm::ParameterSet& iConfig) {

  edm::InputTag theMuonLabel("slimmedMuons");
  edm::InputTag theTrigResultsLabel("TriggerResults::HLT");

  muonCollToken = consumes<pat::MuonCollection>(theMuonLabel);
  trigResultsToken = consumes<edm::TriggerResults>(theTrigResultsLabel);

  edm::Service<TFileService> fs;
  tree = fs->make<TTree>("MuonTree", "Tree storing muon pt");
  tree->Branch("pt", &pt);
  tree->Branch("eta", &eta);
  tree->Branch("pdgId", &pdgId);
  tree->Branch("dilep_mass", &dilep_mass);
  tree->Branch("fourlep_mass", &fourlep_mass);
  tree->Branch("phi", &phi);
  tree->Branch("nMuon", &nMuon, "nMuon/I");
  for (const auto& triggerName : triggerNames) {
        muonMatched[triggerName] = std::vector<bool>();
        tree->Branch(("Matched_" + triggerName).c_str(), &muonMatched[triggerName]);
    }


}
MuonExercise2::~MuonExercise2() {
}

void MuonExercise2::matchTrigger(const edm::Event& iEvent,
                                  edm::Handle<edm::TriggerResults>& trigResults,
                                  const pat::Muon& mu,
                                  std::map<std::string, bool>& matched) {
    // const std::string triggerName = "HLT_IsoMu24_v"; 
    // matched = false;
    matched.clear();
    for (const auto& triggerName : triggerNames) {
        matched[triggerName] = false;
    }
    for (const auto& muTrigger : mu.triggerObjectMatches()) {
        
        pat::TriggerObjectStandAlone& trigobj = const_cast<pat::TriggerObjectStandAlone&>(muTrigger);
        trigobj.unpackNamesAndLabels(iEvent, *trigResults);
        float dR_tmp = deltaR(mu.eta(), mu.phi(), trigobj.eta(), trigobj.phi());
        
        if (dR_tmp < 0.1) {
          for (const auto& triggerName : triggerNames) {
              // 判断是否是 HLT_TripleMu_12_10_5
              if (triggerName == "HLT_TripleMu_12_10_5") {
                  // 使用 pathNames(false, true)
                  for (const auto& path : trigobj.pathNames(false, true)) {
                      if (path.find(triggerName) != std::string::npos) {
                          // std::cout<<mu.pt()<<"   "<<mu.eta()<<"   "<<mu.phi()<<std::endl; 
                          // std::cout<<triggerName<<"   "<<trigobj.pt()<<"  "<< trigobj.eta()<<"   "<<trigobj.phi()<<std::endl; 
                          matched[triggerName] = true;
                      }
                  }
              } else {
                  // 默认使用 pathNames(true, true)
                  for (const auto& path : trigobj.pathNames(true, true)) {
                      if (path.find(triggerName) != std::string::npos) {
                          // std::cout<<mu.pt()<<"   "<<mu.eta()<<"   "<<mu.phi()<<std::endl; 
                          // std::cout<<triggerName<<"   "<<trigobj.pt()<<"  "<< trigobj.eta()<<"   "<<trigobj.phi()<<std::endl; 
                          matched[triggerName] = true;
                      }
                  }
              }
          }
      }

        // if (dR_tmp < 0.1) {
        //     for (const auto& path : trigobj.pathNames(true, true)) {
        //         for (const auto& triggerName : triggerNames) {
        //             if (path.find(triggerName) != std::string::npos) {
        //                 // std::cout<<mu.pt()<<"   "<<mu.eta()<<"   "<<mu.phi()<<std::endl; 
        //                 // std::cout<<triggerName<<"   "<<trigobj.pt()<<"  "<< trigobj.eta()<<"   "<<trigobj.phi()<<std::endl; 
                        
        //                 matched[triggerName] = true;
        //             }
        //         }
        //     }
        // }
    }
}



void MuonExercise2::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;
  using namespace std;
  using namespace reco;
  using namespace pat;
  using namespace l1extra;
  edm::Handle<vector<pat::Muon>> muons;
  iEvent.getByToken(muonCollToken, muons);


  edm::Handle<edm::TriggerResults> trigResults;
  iEvent.getByToken(trigResultsToken, trigResults);
  nMuon = muons->size(); 
  if (nMuon < 4) return;
  pt.clear();
  eta.clear();
  pdgId.clear();
  phi.clear();
  dilep_mass.clear();
  // fourlep_mass.clear();
  for (const auto& triggerName : triggerNames) {
        muonMatched[triggerName].clear();
    }
  std::map<std::string, bool> matched;
  // std::cout<<"\n"<<std::endl;
  for (auto it = muons->cbegin(); it != muons->cend(); ++it) {

    if (std::fabs(it->dB(pat::Muon::PV3D) / it->edB(pat::Muon::PV3D)) > 4) continue; 
    float iso = (it->pfIsolationR03().sumChargedHadronPt + max(0., it->pfIsolationR03().sumNeutralHadronEt + it->pfIsolationR03().sumPhotonEt - 0.5*it->pfIsolationR03().sumPUPt)) / it->pt();
    if ( not (iso < 0.35) ) continue;
    if ( not (it->isMediumMuon()) ) continue;
    pt.push_back(it->pt());
    eta.push_back(it->eta());
    pdgId.push_back(it->pdgId());
    phi.push_back(it->phi());
    
    matchTrigger(iEvent, trigResults, *it, matched);

    for (const auto& triggerName : triggerNames) {
        muonMatched[triggerName].push_back(matched[triggerName]);
    }

  }
  if (pt.size() < 4) return;
  int pdgId_sum = pdgId[0] + pdgId[1] + pdgId[2] + pdgId[3];
  if (pdgId_sum != 0) return;
  TLorentzVector mu1;
  TLorentzVector mu2;
  TLorentzVector mu3;
  TLorentzVector mu4;
  mu1.SetPtEtaPhiM(pt[0], eta[0],phi[0],0.105);
  mu2.SetPtEtaPhiM(pt[1], eta[1],phi[1],0.105);
  mu3.SetPtEtaPhiM(pt[2], eta[2],phi[2],0.105);
  mu4.SetPtEtaPhiM(pt[3], eta[3],phi[3],0.105);
  fourlep_mass=(mu1+mu2+mu3+mu4).M();
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = i + 1; j < 4; ++j) {
        if (pdgId[i] + pdgId[j] != 0) continue;
        double diMuonRecMass = sqrt(2*pt[i]*pt[j]*(cosh(eta[i]-eta[j])-cos(phi[i]-phi[j])));
        dilep_mass.push_back(diMuonRecMass);
    }
  }
  if (muonMatched["HLT_IsoMu24_v"][0]==1) tree->Fill();
  // if (std::any_of(muonMatched["HLT_IsoMu24_v"].begin(), muonMatched["HLT_IsoMu24_v"].end(), [](bool m) { return m; })) {
  //       tree->Fill();
  //   }
  //
  // GEN Muons
  //  
//   edm::Handle <pat::PackedGenParticleCollection> genColl;
//   iEvent.getByToken(genCollToken, genColl);
//   int n = 0;
//   for (auto it = genColl->cbegin(); it != genColl->cend(); ++it) if ( abs((*it).pdgId()) == 13 && fabs((*it).eta()) < 2.4 && (*it).pt() > 1.5 ) n++;
//   cout << "Number of GEN muons: " << n << endl;

//   for (auto it = genColl->cbegin(); it != genColl->cend(); ++it) {
// 	const pat::PackedGenParticle& mcParticle = (*it);
//         if ( abs(mcParticle.pdgId()) != 13 ) continue; // skip this particle if it isn't a muon
//         h_genpt->Fill(mcParticle.pt());
//     // put your code here
    
//   }

}


// ------------ method called once each job just before starting event loop  ------------
void MuonExercise2::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void MuonExercise2::endJob() {
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void MuonExercise2::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonExercise2);
