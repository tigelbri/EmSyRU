
#include "EmSyRU.hpp"

string logFile = "job.log";
string dlURL = "ftp://ftp1.5hosting.com/job.tar";
string upURL = "ftp://ftp1.5hosting.com/";
string job_dir = string("Workbench");
string env = "/home/tristan/test2/remote_update/build/";

int main(int argc, char **argv) 
{
	EmSyRU emsy(dlURL, upURL, env);
	return emsy.update();
}
