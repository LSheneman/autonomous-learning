#include "SSwD_Archivist.h"

int& SSwD_Archivist::SSwD_Arch_dataInterval = Parameters::register_parameter("dataInterval_SSwD", 10, "How often to save a data file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SSwD", 10, "How often to save a genome file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_dataIntervalDelay = Parameters::register_parameter("dataIntervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay before saving data", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeIntervalDelay = Parameters::register_parameter("genomeIntervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay before saving genomes ", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_cleanupInterval = Parameters::register_parameter("cleanupInterval_SSwD", 10, "How often to cleanup old checkpoints", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_DataFilePrefix = Parameters::register_parameter("dataFilePrefix_SSwD", (string) "data", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_GenomeFilePrefix = Parameters::register_parameter("genomeFilePrefix_SSwD", (string) "genome", "name of data file (stores data i.e. scores", "ARCHIVIST_SSWD");
bool& SSwD_Archivist::SSwD_Arch_writeDataFiles = Parameters::register_parameter("writeDataFiles_SSwD", true, "if true, data files will be written", "ARCHIVIST_SSWD");
bool& SSwD_Archivist::SSwD_Arch_writeGenomeFiles = Parameters::register_parameter("writeGenomeFiles_SSwD", true, "if true, genome files will be written", "ARCHIVIST_SSWD");
