#include "Archivist.h"

string& Default_Archivist::Arch_outputMethodStr = Parameters::register_parameter("outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), snapshot, SSwD (SnapShot with Delay)]", "ARCHIVIST");  // string parameter for outputMethod;

int& Default_Archivist::Arch_realtimeFilesInterval = Parameters::register_parameter("realtimeFilesInterval", 10, "How often to write to realtime data files", "ARCHIVIST");
bool& Default_Archivist::Arch_writeAveFile = Parameters::register_parameter("writeAveFile", true, "Save data to average file?", "ARCHIVIST");
bool& Default_Archivist::Arch_writeDominantFile = Parameters::register_parameter("writeDominantFile", true, "Save data to dominant file?", "ARCHIVIST");
string& Default_Archivist::Arch_AveFileName = Parameters::register_parameter("aveFileName", (string) "ave.csv", "name of average file (saves population averages)", "ARCHIVIST");
string& Default_Archivist::Arch_DominantFileName = Parameters::register_parameter("dominantFileName", (string) "dominant.csv", "name of dominant file (saves data on dominant organism)", "ARCHIVIST");
string& Default_Archivist::Arch_DefaultAveFileColumnNames = Parameters::register_parameter("DefaultAveFileColumns", (string) "[update,score,genomeSize,gates]", "data to be saved into average file (must be values that can generate an average)", "ARCHIVIST");
