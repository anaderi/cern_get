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

void print_measurement(const time_point_t started_at, const Long64_t entries_count, const Long64_t current_entry_number) {
    time_point_t current_time = chrono::steady_clock::now();
    double elapsed      = chrono::duration<double>(current_time - started_at).count();
    double per_entry    = elapsed / current_entry_number;
    int remaining       = int(per_entry * (entries_count - current_entry_number));

    int elapsed_i         = int(elapsed);
    int elapsed_seconds   = elapsed_i % 60;
    int elapsed_minutes   = (elapsed_i / 60) % 60;
    int elapsed_hours     = elapsed_i / 60 / 60;

    int remaining_seconds   = remaining % 60;
    int remaining_minutes   = (remaining / 60) % 60;
    int remaining_hours     = remaining / 60 / 60;

    cout    << current_entry_number << " / " << entries_count << " |   "
            << "Elapsed: "   << elapsed_hours << "h " << elapsed_minutes << "m " << elapsed_seconds << "sec / "
            << "Per entry: " << per_entry << " sec / "
            << "Remaining: " << remaining_hours << "h " << remaining_minutes << "m " << remaining_seconds << "sec "
            << endl;

    cout.flush();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "cern_get INPUT_FILE OUTPUT_FILE" << endl;
        return 0;
    }

    cout << "Processing " << argv[1] << "..." << endl;
    ifstream file(argv[1]);
    TChain *chain = new TChain("Events");

    string root_file_name;
    while (getline(file, root_file_name)) {
        cout << "Append file: " << root_file_name << endl;
        chain->Add(root_file_name.c_str());
    }

    cout << "Select branches... ";

    chain->SetBranchStatus("*", 0);
    chain->SetBranchStatus("recoCaloJets_ak5CaloJets__RECO.obj.mass_", 1);

    chain->SetCacheSize(1024 * 1024 * 50); // IT'S BEAUTIFUL MAGIC OF SPEED UP!

    cout << "done." << endl;

    cout << "Calculating entries count... " << endl;
    cout.flush();

    Long64_t entries_count = chain->GetEntries();

    cout << entries_count << " entries." << endl;

    ofstream csv_out(argv[2]);

    cout << "Retrieving data:"  << endl;
    cout.flush();
    time_point_t started_at = chrono::steady_clock::now();
    for (Long64_t entry_number = 0; entry_number < entries_count; ++entry_number) {
        if (entry_number != 0 && entry_number % 500 == 0) {
            print_measurement(started_at, entries_count, entry_number);
        }

        // working with data
        chain->GetEntry(entry_number);
        csv_out << chain->GetLeaf("recoCaloJets_ak5CaloJets__RECO.obj.mass_")->GetValue() << "\n";
    }

    print_measurement(started_at, entries_count, entries_count);
    csv_out.close();
    cout << "done" << endl;

    return 0;
}
