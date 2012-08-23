//  @@@LICENSE
//
//      Copyright (c) 2010-2012 Hewlett-Packard Development Company, L.P.
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

#include "serviceinstall.h"

static LSError lserror;
static LSMessageToken sessionToken;
static GMainLoop *mainLoop = NULL;

#ifdef DEBUG
#define DBG(fmt,...) g_message(fmt,__VA_ARGS__);
#else
#define DBG(fmt,...)
#endif

static void _mkdir(char *dir)
{
	char *tmp = NULL;
	char *p = NULL;
	size_t len;

	if ((tmp = strdup(dir)) != NULL) {
		len = strlen(tmp);
		if (tmp[len - 1] == '/') tmp[len - 1] = 0;
		for(p = tmp + 1; *p; p++)
			if (*p == '/') {
				*p = 0;
				mkdir(tmp, S_IRWXU);
				*p = '/';
			}
		mkdir(tmp, S_IRWXU);
	}
	if (tmp) free(tmp);
}


static bool fexists(string filename)
{
	struct stat s;
	if (stat(filename.c_str(),&s) == 0) return true;
	return false;
}


static string getAppDirectory(string root, string id, string type)
{
	string appDirectory= root;
	appDirectory += "/usr/palm/services/" + id;
	return appDirectory;
}

static string getPublicEndpointDirectory(string root)
{
	string endpointDirectory= PUBLIC_ENDPOINT_ROOT;
	return endpointDirectory;
}

static string getPrivateEndpointDirectory(string root)
{
	string endpointDirectory= PRIVATE_ENDPOINT_ROOT;
	return endpointDirectory;
}

static void generateEndpoint(string id, string serviceDirectory, string endpointDirectory) 
{
	string line;
	string destinationPath = endpointDirectory + "/" + id;
	DIR *d;

	if ((d=opendir(endpointDirectory.c_str())) == NULL) {
		_mkdir((char*)endpointDirectory.c_str());
	} else {
		closedir(d);
	}

	if (fexists(destinationPath)) return;

	DBG("Creating %s", destinationPath.c_str());

	ofstream newfile (destinationPath.c_str());	

    if  (newfile.is_open())
    {
		
		newfile << "[D-BUS Service]" << endl;
		newfile << "Name=" << id << endl;
		newfile << "Exec=/usr/bin/run-js-service -n "<< serviceDirectory << endl;
		
		newfile.close();
	}
	
}

static void deleteEndpoint(string id, string endpointDirectory)
{
	string destinationPath = endpointDirectory + "/" + id;
	if (fexists(destinationPath)) {
		DBG("Removing %s", destinationPath.c_str());
		unlink(destinationPath.c_str());
	}
}

static void deleteRoleFiles(string id)
{	
	string roleRoot = ROLE_FILE_PATH;
	string pubPath = roleRoot  + "/pub/" + id + ".json";
	string prvPath = roleRoot + "/prv/" + id + ".json";

	if (fexists(pubPath)) {
		DBG("Removing %s", pubPath.c_str());
		unlink(pubPath.c_str());
	}

	if (fexists(prvPath)) {
		DBG("Removing %s", prvPath.c_str());
		unlink(prvPath.c_str());
	}
}

static bool configuratorGetResponse(LSHandle *sh, LSMessage *reply, void *ctx)
{
	g_main_loop_quit(mainLoop);
	return true;
}

static void runConfigurator(string id, string serviceDirectory, string type, bool install)
{
	LSHandle *sh = NULL;
	string url = CONFIGURATOR_REGISTER_URL;
	string configuratorType;
	
	if (type.compare(TYPE_SERVICE) == 0)
		configuratorType = "service";
	else
		configuratorType = "app";
	
	string empty = ""; 
	string payload = "[{\"id\":\"" + id + "\","
			+  "\"type\":\"" + configuratorType +"\"," 
			+  "\"location\":\"third party\"}]";
		
	GMainContext *mainContext = g_main_context_new();
    mainLoop = g_main_loop_new(mainContext, FALSE); 
    if (mainLoop == NULL) goto error;

    bool retVal;
    LSErrorInit(&lserror);

    retVal = LSRegister(NULL, &sh, &lserror);
    if (!retVal)
    {
        LSErrorPrint (&lserror, stderr);
        LSErrorFree (&lserror);
        goto error;
    }

    retVal = LSGmainAttach(sh, mainLoop, &lserror); 

    if (!retVal)
    {
        LSErrorPrint (&lserror, stderr);
        LSErrorFree (&lserror);
        goto error;
    }
	
	if (!install)
		url = CONFIGURATOR_UNREGISTER_URL;

	DBG("Calling service: %s", url.c_str());
	DBG("Parameters: %s", payload.c_str());

    retVal = LSCallFromApplication(sh, url.c_str(), payload.c_str(), id.c_str(),
            configuratorGetResponse, NULL, &sessionToken, &lserror);

    if (!retVal)
    {
        LSErrorPrint (&lserror, stderr);
        LSErrorFree (&lserror);
        goto error;
    }

	g_main_loop_run(mainLoop);
	
error:

    if (mainLoop)
        g_main_loop_unref(mainLoop);

    if (sh)
    {
        retVal = LSUnregister(sh, &lserror);
        if (!retVal)
        {
            LSErrorPrint (&lserror, stderr);
            LSErrorFree (&lserror);
            goto error;
        }
    }
}



static void updateLunaService()
{
	// temporary until LS has an API or mechanism to pick up changes
	DBG("Updating LS Hub %s", "");
	int result = system("/usr/bin/ls-control scan-services");
}


void installApp(string id, string type, string root) 
{
	string appDirectory = getAppDirectory(root, id, type);
	
	if (type.compare(TYPE_SERVICE) == 0) {
		
		DBG("Installing service files %s","");

		// generate roles for LS2 permissions
		tritonGenerateRole(id);
						
		// create a hook for LS2 to know how to launch
		string endpointDirectory = getPublicEndpointDirectory(root);
		generateEndpoint(id, appDirectory, endpointDirectory);
		endpointDirectory = getPrivateEndpointDirectory(root);
		generateEndpoint(id, appDirectory, endpointDirectory);
		
		// tell the hub there's a new service in town
		updateLunaService();

	}
	
	// handle configurator for MojoDB
	runConfigurator(id, appDirectory, type, true);
	
}

void uninstallApp(string id, string type, string root) 
{	
	
	string appDirectory = getAppDirectory(root, id, type);
	
	// delete the role files
	deleteRoleFiles(id);
		
	if (type.compare(TYPE_SERVICE) == 0) {
		
		DBG("Removing service files %s","");
			
		// delete the endpoint files
		string endpointDirectory = getPublicEndpointDirectory(root);
		deleteEndpoint(id, endpointDirectory);
		endpointDirectory = getPrivateEndpointDirectory(root);
		deleteEndpoint(id, endpointDirectory);
		
		// tell the hub something changed
		updateLunaService();
	}
	
	// handle configurator for MojoDB
	runConfigurator(id, appDirectory, type, false);
	
}

