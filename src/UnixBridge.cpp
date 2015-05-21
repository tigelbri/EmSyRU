#include "UnixBridge.hpp"
extern "C"
{
	void* xrealloc (void *ptr, size_t size)
	{
		void *value = realloc (ptr, size);
		if (value == 0)
			printf("Virtual memory exhausted");
		return value;
	}

	char* readlink_malloc (const char *filename)
	{
		int size = 100;
		char *buffer = NULL;

		while (1)
		{
			buffer = (char *) xrealloc (buffer, size);
			int nchars = readlink (filename, buffer, size);
			if (nchars < 0)
			{
				free (buffer);
				return NULL;
			}
			if (nchars < size)
				return buffer;
			size *= 2;
		}
	}
}


Logger UnixBridge::log_ = Logger();

UnixBridge::UnixBridge()
{
	
}

UnixBridge::~UnixBridge()
{

}

int UnixBridge::createSymbolicLink(string name, string content)
{
	return symlink(content.c_str(), name.c_str());
}


string UnixBridge::getSymLink(string link)
{
	char* readlink = readlink_malloc(link.c_str());
	if(readlink != NULL)
	{
		string folder = string(readlink);
		free(readlink);
		unsigned found = folder.find_last_of("/\\");
		return folder.substr(0, found);
	}
	else
		return "";
}

int UnixBridge::mvFile(string in, string out)
{
	return rename(in.c_str(), out.c_str());
}

int UnixBridge::deleteDir(string dir)
{
    int ret = 1;
    FTS *ftsp = NULL;
    FTSENT *curr;

    char *files[] = { (char *) dir.c_str(), NULL };

    // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
    //                in multithreaded programs
    // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
    //                of the specified directory
    // FTS_XDEV     - Don't cross filesystem boundaries
    ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
    if (!ftsp) {
        fprintf(stderr, "%s: fts_open failed: %s\n", dir.c_str(), strerror(errno));
        ret = 0;
        return ret;
    }

    while ((curr = fts_read(ftsp))) {
        switch (curr->fts_info) {
        case FTS_NS:
        case FTS_DNR:
        case FTS_ERR:
            fprintf(stderr, "%s: fts_read error: %s\n",
                    curr->fts_accpath, strerror(curr->fts_errno));
            break;

        case FTS_DC:
        case FTS_DOT:
        case FTS_NSOK:
            break;

        case FTS_D:
            break;

        case FTS_DP:
        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
            if (remove(curr->fts_accpath) < 0) {
                fprintf(stderr, "%s: Failed to remove: %s\n",
                        curr->fts_path, strerror(errno));
                ret = -1;
            }
            break;
        }
    }
    
    if (ftsp) {
        fts_close(ftsp);
    }

    return ret;
}

int	UnixBridge::mvDir(string move_from, string move_to)
{
	if(!(move_to.back() == '/'))
		move_to = move_to + "/";
	std::vector<string> files;
	if(!getFiles(move_from, "", files))
		return 0;
	for(string file : files)
	{
		unsigned found = file.find_last_of("/\\");
		string filename = file.substr(found + 1, file.size() - 1);
		int suc = mvFile(file, move_to + filename);
		if(suc == 0)
			log_ << "STATUS: Moving file " << filename << endl;
		else
			return log_ << "ERROR: Moving file " << file <<
				   " to " << move_to + filename << " fail Code: " << suc << endl, 0;
	}
	return 1;
}

int UnixBridge::getFiles(string dir, string upFile, std::vector<string>& files)
{
	DIR* dirp = opendir(dir.c_str());
	if(dirp == NULL)
		return log_ << "ERROR: Couldnt find files at path: " << dir << endl, 0;
	dirent* dp;
	while ((dp = readdir(dirp)) != NULL)
	{
		if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && strcmp(dp->d_name, upFile.c_str()) && dp->d_type != DT_DIR) 
		    files.push_back(dir + dp->d_name);
        if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && dp->d_type == DT_DIR)
			getFiles(string(dir + dp->d_name + "/"), upFile, files);
    }
	(void)closedir(dirp);
	if(files.size() == 0)
		return log_ << "ERROR: Couldnt find Updating files at path: " << dir << endl, 0;
	return 1;
}


int UnixBridge::createDir(string s, mode_t mode)
{    
	size_t pre=0,pos;
    string dir;
    int mdret = 0;

    if(s[s.size()-1]!='/'){
        // force trailing / so we can handle everything in loop
        s+='/';
    }

    while((pos=s.find_first_of('/',pre))!=std::string::npos){
        dir=s.substr(0,pos++);
        pre=pos;
        if(dir.size()==0) continue; // if leading / first time is 0 length
        if((mdret=mkdir(dir.c_str(),mode)) && errno!=EEXIST){
            return mdret;
        }
    }
    return mdret;
}
