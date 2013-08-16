//  @@@LICENSE
//
//      Copyright (c) 2010-2013 LG Electronics, Inc.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  LICENSE@@@

#include <iostream>
#include "unistd.h"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "serviceinstall.h"


using namespace std;

static void usage(void) {
	cout << "Usage: test-serviceinstaller -i appId -r root -t type [-u]\n";
	exit(1);
}

int main(int argc, char* argv[])
{
    int i = 1;
    char *appId = NULL;
    char *root = NULL;
	char *type = NULL;
	int uninstall = 0;

    if (argc <= 1)
	usage();

    try {
		while (i < argc) {
		    if (argv[i][0] == '-') {
				switch (argv[i][1]) {
				    case 'i':
					i++;
					appId = argv[i];
					break;
				    case 'r':
					i++;
					root = argv[i];
					break;
					case 't':
					i++;
					type = argv[i];
					break;
					case 'u':
					uninstall = 1;
					break;
				    default:
					usage();
					break;
				}
			i++;
		    }
		    else break;
		}
		
		if (appId == NULL || root == NULL || type == NULL)
			usage();
		
		if (! uninstall)
			installApp(appId, type, root) ;
		else 
			uninstallApp(appId, type, root) ;

    }
    catch (std::exception& e) {
		cout << "An error occured: " << e.what() << "\n";
		return(1);
    }
	return(0);
}
