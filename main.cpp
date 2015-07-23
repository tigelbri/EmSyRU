//## Copyright (c) 2015, Tristan Igelbrink
//## All rights reserved.
//## 
//## Redistribution and use in source and binary forms, with or without
//## modification, are permitted provided that the following conditions 
//## are met:
//##
//## 1. Redistributions of source code must retain the above copyright
//## notice, this list of conditions and the following disclaimer.
//##
//## 2. Redistributions in binary form must reproduce the above copyright
//## notice, this list of conditions and the following disclaimer in the
//## documentation and/or other materials provided with the distribution.
//## 
//## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//## COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT
//## , INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//## SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//## STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//## OF THE POSSIBILITY OF SUCH DAMAGE.

#include "EmSyRU.hpp"

using std::cout;
using std::endl;

/**
* Method for usage printing.
*/
void printUsage()
{
	cout << "ERROR: Wrong usage " << endl;
	cout << "Usage: " << endl;
	cout << "./EmSyRU 'user@ftp://path/to/jobfile' 'password' " << endl;
    cout << "./EmSyRU 'user@ftp://path/to/jobfile' 'password' ['/path/for/creating/workbench']" << endl;
    cout << endl;
    cout << "If no path for creating the workbench is given the workbench will be created next to executable" << endl;
}

//!  EmSyru main. 
/*!
  Starts the EmSyRU and parses the command line parameters
  * user@ftp.server.com/path/to/jobfile
*/
int main(int argc, char **argv) 
{
	string dlURL, user, pw;
	string env = "";
	if(argc > 4)
		return printUsage(), 0;
    if(argc >= 3)
    {	
		string user_url = string(argv[1]);
		unsigned found = user_url.find_last_of("@");
		if(found == 0xffffffff)
		{
			cout << "No user@url given" << endl;
			printUsage(); 
			return 0;
		}
		user = user_url.substr(0, found);
		dlURL = user_url.substr(found + 1, dlURL.size() - 1);
		pw = string(argv[2]);
	}
	else
		return printUsage(), 0;
	if(argc == 4)
		env = string(argv[3]);
	EmSyRU emsy(user, dlURL, pw, env);
	return emsy.update();
}
