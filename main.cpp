
#include "EmSyRU.hpp"

string test_dlURL = "ftp622_4@ftp://ftp1.5hosting.com/job.tar";

using std::cout;
using std::endl;

void printUsage()
{
	cout << "ERROR: Wrong usage " << endl;
	cout << "Usage: " << endl;
	cout << "./EmSyRU 'user@ftp://path/to/jobfile' 'password' " << endl;
    cout << "./EmSyRU 'user@ftp://path/to/jobfile' 'password' ['/path/for/creating/workbench']" << endl;
    cout << endl;
    cout << "If no path for creating the workbench is given the workbench will be created next to executable" << endl;
}

int main(int argc, char **argv) 
{
	string dlURL, user, pw;
	string env = "";
	if(argc > 4)
		return printUsage(), 0;
    if(argc >= 3)
    {	
		//string user_url = string(argv[1]);
		string user_url = test_dlURL;
		unsigned found = user_url.find_last_of("@");
		if(found == 0xffffffff)
		{
			cout << "No user@url given" << endl;
			printUsage(); 
			return 0;
		}
		user = user_url.substr(0, found);
		dlURL = user_url.substr(found + 1, dlURL.size() - 1);
		//pw = string(argv[2]);
		pw = test_pw;
	}
	else
		return printUsage(), 0;
	if(argc == 4)
		env = string(argv[3]);
	EmSyRU emsy(user, dlURL, pw, env);
	return emsy.update();
}
