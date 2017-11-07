#include "LODwAP_Archivist.h"

int& LODwAP_Archivist::LODwAP_Arch_dataInterval = Parameters::register_parameter("dataInterval_LODwAP", 10, "How often to write to data file", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_LODwAP", 10, "How often to write genome file", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_pruneInterval = Parameters::register_parameter("pruneInterval_LODwAP", 10, "How often to attempt to prune LOD and actually write out to files", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_terminateAfter = Parameters::register_parameter("terminateAfter_LODwAP", 100, "how long to run after updates (to get better coalescence)", "ARCHIVIST_LODWAP");
string& LODwAP_Archivist::LODwAP_Arch_DataFileName = Parameters::register_parameter("dataFileName_LODwAP", (string) "data.csv", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_LODWAP");
string& LODwAP_Archivist::LODwAP_Arch_GenomeFileName = Parameters::register_parameter("genomeFileName_LODwAP", (string) "genome.csv", "name of data file (stores data i.e. scores", "ARCHIVIST_LODWAP");
bool& LODwAP_Archivist::LODwAP_Arch_writeDataFile = Parameters::register_parameter("writeDataFile_LODwAP", true, "if true, a data file will be written", "ARCHIVIST_LODWAP");
bool& LODwAP_Archivist::LODwAP_Arch_writeGenomeFile = Parameters::register_parameter("writeGenomeFile_LODwAP", true, "if true, a genome file will be written", "ARCHIVIST_LODWAP");
