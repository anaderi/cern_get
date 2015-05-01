#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;

typedef chrono::time_point<chrono::steady_clock> time_point_t;

void print_time_info(const time_point_t started_at, const Long64_t entries_count, const Long64_t current_entry_number) {
    time_point_t current_time = chrono::steady_clock::now();
    double elapsed      = chrono::duration<double>(current_time - started_at).count();
    double per_entry    = elapsed / current_entry_number;
    int estimated       = int(per_entry * entries_count);

    int estimated_seconds   = estimated % 60;
    int estimated_minutes   = (estimated / 60) % 60;
    int estimated_hours     = estimated / 60 / 60;

    cout    << "Elapsed: " << elapsed << " sec. / "
            << "Per entry: " << per_entry << " sec. / "
            << "Estimation: " << estimated_hours << "h. " << estimated_minutes << "m. " << estimated_seconds << "sec. ";
}

int main(int argc, char *argv[]) {
    ifstream file("resources/root_files.txt");
    TChain *chain = new TChain("Events");

    string root_file_name;
    while (getline(file, root_file_name)) {
        cout << "Append file: " << root_file_name << endl;
        chain->Add(root_file_name.c_str());
    }

    cout << "Select branches... ";

    chain->SetBranchStatus("*", 0);
    chain->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.mass_", 1);
    chain->SetBranchStatus("EventAuxiliary", 1);

    cout << "done." << endl;

    Long64_t entries_count = chain->GetEntries();
    cout << entries_count << " entries." << endl;

    ofstream csv_out("resources/data.csv");

    cout << "Retrieving data:"  << endl;
    cout.flush();
    time_point_t started_at = chrono::steady_clock::now();
    for (Long64_t entry_number = 0; entry_number < entries_count; ++entry_number) {
        if (entry_number != 0 && entry_number % 100 == 0) {
            cout << entry_number << " / " << entries_count << " | ";
            print_time_info(started_at, entries_count, entry_number);
            cout << endl;
            cout.flush();
        }
        chain->GetEntry(entry_number);

        csv_out << chain->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.mass_")->GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.mass_")->GetValue() << "\n";
    }

    csv_out.close();
    cout << "done" << endl;

    return 0;

//   ifstream file("root_files.txt");
//   TChain *t = new TChain("Events");
//   while (getline(file, name)) {
//       cout << name << endl;
//       t->Add(name.c_str());
//   }
//
//   t->SetBranchStatus("*", 0);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.pt_", 1);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.eta_", 1);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.phi_", 1);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.mass_", 1);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fX", 1);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fY", 1);
//   t->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fZ", 1);
//
//   t->SetBranchStatus("EventAuxiliary", 1);
//
//   int nent = t->GetEntries();
//   cout << "n(ent): " << nent << endl;
//
//   ofstream file_out("data.csv");
//
//   for (int e = 0; e < nent; ++e) {
//       t->GetEntry(e);
//
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.pt_")->GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.pt_")->GetValue()
//                                                                                                                                      << ",";
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.eta_")->GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.eta_")->GetValue()
//                                                                                                                                       << ",";
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.phi_")->GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.phi_")->GetValue()
//                                                                                                                                       << ",";
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.mass_")->GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.mass_")->GetValue()
//                                                                                                                                       << ",";
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fX")->
//                                                GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fX")->GetValue() << ",";
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fY")->
//                                                GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fY")->GetValue() << ",";
//       file_out << t->GetBranch("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fZ")->
//                                               GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.vertex_.fCoordinates.fZ")->GetValue() << "\n";
//
//   }
//
//   return 0;
}
