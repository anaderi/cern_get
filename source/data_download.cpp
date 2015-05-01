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
    int estimated       = int(per_entry * entries_count);

    int estimated_seconds   = estimated % 60;
    int estimated_minutes   = (estimated / 60) % 60;
    int estimated_hours     = estimated / 60 / 60;

    int elapsed_i         = int(elapsed);
    int elapsed_seconds   = elapsed_i % 60;
    int elapsed_minutes   = (elapsed_i / 60) % 60;
    int elapsed_hours     = elapsed_i / 60 / 60;

    cout    << current_entry_number << " / " << entries_count << " |   "
            << "Elapsed: " << elapsed_hours << "h. " << elapsed_minutes << "m. " << elapsed_seconds << "sec. / "
            << "Per entry: " << per_entry << " sec. / "
            << "Estimation: " << estimated_hours << "h. " << estimated_minutes << "m. " << estimated_seconds << "sec. "
            << endl;

    cout.flush();
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

    chain->SetCacheSize(1024 * 1024 * 50); // IT'S BEAUTIFUL MAGIC OF SPEED UP!

    cout << "done." << endl;

    Long64_t entries_count = chain->GetEntries();

    cout << entries_count << " entries." << endl;

    ofstream csv_out("resources/data.csv");

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
