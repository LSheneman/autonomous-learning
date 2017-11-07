#include "snapshot_Archivist.h"

int& Snapshot_Archivist::SS_Arch_dataInterval = Parameters::register_parameter("dataInterval_SS", 10, "How often to save a data file", "ARCHIVIST_SNAPSHOT");
int& Snapshot_Archivist::SS_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SS", 10, "How often to save a genome file", "ARCHIVIST_SNAPSHOT");
bool& Snapshot_Archivist::SS_Arch_writeDataFiles = Parameters::register_parameter("writeDataFiles_SS", true, "if true, data files will be written", "ARCHIVIST_SNAPSHOT");
bool& Snapshot_Archivist::SS_Arch_writeGenomeFiles = Parameters::register_parameter("writeGenomeFiles_SS", true, "if true, genome files will be written", "ARCHIVIST_SNAPSHOT");
