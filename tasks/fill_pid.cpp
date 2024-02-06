#include "PidFiller.hpp"

#include "AnalysisTree/TaskManager.hpp"

using namespace AnalysisTree;

void fill_pid(const std::string& filelist, const std::string& pid_file, const std::string& output) {

  auto* man = TaskManager::GetInstance();
  man->SetOutputName(output, "aTree");

  // The output file will be based on the input one with additional branches (and possibly with removed other branches)
  man->SetWriteMode(eBranchWriteMode::kCopyTree);

  // Branches TrdTracks and RichRings will be removed from the output file, since they are irrelevant for the following analysis.
  // The branch VtxTracks, which is an input branch, will be also removed, because a new one, based on it, will be created.
  man->SetBranchesExclude({"TrdTracks", "RichRings", "VtxTracks"});

  // Initialize the Pid::Getter prepared at the fitting step: 1-st argument - file, 2-nd argument - name of the getter.
  auto* pid_task = new PidFiller(pid_file, "pid_getter");

  man->AddTask(pid_task);

  man->Init({filelist}, {"rTree"});
  man->Run(-1);
  man->Finish();
}

int main(int argc, char** argv) {
  if (argc <= 2) {
    std::cout << "Not enough arguments! Please use:" << std::endl;
    std::cout << "   ./fill_pid filelist pid_file" << std::endl;
    return EXIT_FAILURE;
  }

  const std::string filelist = argv[1];
  const std::string pid_file = argv[2];
  const std::string output_file = "pid.analysistree.root";

  fill_pid(filelist, pid_file, output_file);
  return EXIT_SUCCESS;
}
