#include<bits/stdc++.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

using namespace std;

vector<string> list_dir, list_files, list_comp;
vector <int> dir_check;

stack<string> prev_stack;
stack<string> next_stack;

static struct termios term, oterm;

int start_line, end_line;
int gstart, gend;
int flag = 0;
string str1 = "", str2 = "";
string root_path = "/Users/ashishchauhan/";

//----------------------------------------------------------------------------------------------------------------------------//
void Normal_mode(string directory_path);
void enable_non_canonical(int start, int end);
string key_pressed();
void get_dir_files(string directory_path);
void print_details(int start, int end);
void print_record(string dir_file);
//----------------------------------------------------------------------------------------------------------------------------//
void Command_mode(int start, int end);
vector<string> get_command(string input);
int check(string input);
void copy_file(string source, string destination);
void delete_file(string path);
void create_file(string source);
void create_dir(string source);
int search(string path, string search);
void goto_path(string path);
void delete_dir(string path);
void copy_dir(string path);
char check_dir_file(string path);
void copy_perm(string str1, string str2);
void new_command();
//----------------------------------------------------------------------------------------------------------------------------//

static int getch(void)
{
    int c = 0;
    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));

    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);

    return c;
}

void print_record(const char* dir_file_path)
{
    struct stat s;
    struct passwd *pw;
    struct group *gp;
    char *time;

    string path(dir_file_path);
    size_t loc = path.find_last_of('/');
    string name = path.substr(loc+1, path.length());
    string fd_name;

    for(int i=0; i<20 && i<name.size(); i++)
	{
		if(i<=16)
		fd_name += name[i];
		else
		fd_name += '.';
	}
    
    if( stat(dir_file_path, &s) == 0 )
    {
        //File Name
        string path(dir_file_path);
        size_t loc = path.find_last_of('/');
        string name = path.substr(loc+1, path.length());
        string fd_name;

        for(int i=0; i<20 && i<name.size(); i++)
        {
            if(i<=16)
            fd_name += name[i];
            else
            fd_name += '.';
        }

        cout << std::left << setw(25);
        cout << fd_name;

        //File Size
        float size = (s.st_size/1024.0);
        stringstream ss;
        ss << fixed << std::setprecision(2) << size;
        std::string s1 = ss.str();
        cout << std::left << setw(20);
        string sizef = s1 + " KB";
        cout << sizef;

        //Ownership
        cout << std::left << setw(20);
        pw=getpwuid(s.st_uid);
        cout << pw->pw_name;

        cout << std::left << setw(20);
        gp=getgrgid(s.st_gid);
        cout << gp->gr_name;
        
        //Permissions
        string permissions = (S_ISDIR(s.st_mode)) ? "d" : "-";
        permissions += (s.st_mode & S_IRUSR) ? "r" : "-";
        permissions +=(s.st_mode & S_IWUSR) ? "w" : "-";
        permissions +=(s.st_mode & S_IXUSR) ? "x" : "-";
        permissions +=(s.st_mode & S_IRGRP) ? "r" : "-";
        permissions +=(s.st_mode & S_IWGRP) ? "w" : "-";
        permissions +=(s.st_mode & S_IXGRP) ? "x" : "-";
        permissions +=(s.st_mode & S_IROTH) ? "r" : "-";
        permissions +=(s.st_mode & S_IWOTH) ? "w" : "-";
        permissions +=(s.st_mode & S_IXOTH) ? "x" : "-";

        cout << std::left << setw(20);
        cout << permissions;

        //Last Modified
        time = ctime(&s.st_mtime);
        for(int i=4; i<=15; i++)
            printf("%c", time[i]);

        cout << endl;
    }

}

void print_details(int start, int end)
{
    cout << "\033[2J";                              //clearing the terminal
    cout << "\033[0;0H";                            //setting the cursor to top-left

    for (int i = start; i <= end; i++)
    {
        print_record(list_comp[i].c_str());
    }
}

void get_dir_files(string directory_path)
{
    //clearing the dir and files vectors
    list_dir.clear();
    list_files.clear();
    list_comp.clear();
    dir_check.clear();

    //temporary vector for storing file and dir names
    struct dirent *dp;
    struct stat stats;
    DIR *dir;

    const char* path = (directory_path).c_str();
    dir = opendir(path);

    if (!dir) 
    {
        //if dir doesn't exist then return
        cout << "Error finding the directory. Try again!";
        return; 
    }

    while ((dp = readdir(dir) ) != NULL)
    {
    	string fullpath = directory_path + dp->d_name;
    	const char *file_dir_path= fullpath.c_str();

    	if (stat(file_dir_path, &stats) == 0)
        {
			if( stats.st_mode & S_IFDIR )
			{
                list_dir.push_back(file_dir_path);
			}
			else if( stats.st_mode & S_IFREG )
			{
                list_files.push_back(file_dir_path);
			}
		}
	}

    sort(list_files.begin(),list_files.end());
    sort(list_dir.begin(), list_dir.end());

    for(int i=0; i<list_dir.size(); i++)
    {
        list_comp.push_back(list_dir[i]);
        dir_check.push_back(0);
    }

    for(int i=0; i<list_files.size(); i++)
    {
        list_comp.push_back(list_files[i]);
        dir_check.push_back(1);
    }

    closedir(dir);
}

string key_pressed()
{
    char c = getch();

    if(c == '\033')
    {
        if(getch() == '[')
        {
            switch (getch())
            {
            case 'A':
                //cout << "UP key is pressed\n";
                return "UP";
                break;
            case 'B':
                //cout << "DOWN key is pressed\n";
                return "DOWN";
                break;
            case 'C':
                //cout << "RIGHT key is pressed\n";
                return "RIGHT";
                break;
            case 'D':
                //cout << "LEFT key is pressed\n";
                return "LEFT";
                break;    
            }
        }
    }
    else
    {
        switch (c)
        {
        case 'k':
            //cout << "k key is pressed\n";
            return "k";
            break;
        case 'l':
            //cout << "l key is pressed\n";
            return "l";
            break;
        case 'h':
            //cout << "h key is pressed\n";
            return "h";
            break;
        case 'q':
            //cout << "q key is pressed\n";
            return "q";
            break;
        case 'Q':
            //cout << "q key is pressed\n";
            return "Q";
            break;
        case 10:
            //cout << "ENTER key is pressed\n";
            return "ENTER";
            break;
        case 59:
            //cout << "COLON key is pressed\n";
            return "COLON";
            break;        
        case 127:
            //cout << "BACKSPACE key is pressed\n";
            return "BACKSPACE";
            break;
        case 9:
            //cout << "ESCAPE key is pressed\n";
            return "TAB";
            break;     
        }
    }

    string ret(1, c);
    return ret;
}

void enable_non_canonical(int start, int end)
{
    int first_line = 0;
    int last_line = list_comp.size()-1;
    
    // cout << "first_line : " << first_line
    //     << " last_line : " << last_line
    //     << " start : " << start
    //     << " end : " << end ;

    cout << "\033[0;0H";
    int cursor_pos = 0;

    // string key = key_pressed();
    // cout << "--------> " << key << " <--------";
    // exit(0);

    while(1)
    {
        string key = key_pressed();
        if(key == "q")
        {
            cout << "\033[2J";                              //clearing the terminal
            cout << "\033[0;0H";                            //setting the cursor to top-left
            exit(0);
        }
        else if(key == "UP" && cursor_pos != start )
        {
            cout << "\x1b[1A";
            cursor_pos--;
        }
        else if(key == "DOWN" && cursor_pos != end)
        {
            cout << "\x1b[1B";
            cursor_pos++;
        }
        else if(key == "k" && cursor_pos == start && start != first_line)
        {
            start--;
            end--;
            cursor_pos--;
            print_details(start, end);
            cout << "\033[0;0H";            //setting cursor on the first row
        }
        else if(key == "l" && cursor_pos == end && end != last_line)
        {
            start++;
            end++;
            cursor_pos++;
            print_details(start, end);
            cout << "\033[21;0H";           //setting cursor on the last row
        }
        else if(key == "ENTER")
        {
            if(dir_check[cursor_pos] == 1)
            {
                //file
                string dir = prev_stack.top();
                string pathname = dir + list_comp[cursor_pos];

                char path_abs[1024];
                strcpy(path_abs, pathname.c_str());
                
                pid_t pid = fork();
                if (pid == 0) 
                {
                    fflush(stdin);
                    char execName[] = "vi";
                    char *exec_args[] = {execName, path_abs, NULL};
                    execv("/usr/bin/vi", exec_args);
                } 
                else
                {
                    wait(NULL);
                }
            }
            else if(dir_check[cursor_pos] == 0)
            {
                //directory
                prev_stack.push(list_comp[cursor_pos]+'/');
                Normal_mode(prev_stack.top());
            }
        }
        else if(key == "RIGHT")
        {
            if(next_stack.size() > 1)
            {
                prev_stack.push(next_stack.top());
                next_stack.pop();
                Normal_mode(prev_stack.top());
            } 
        }
        else if(key == "LEFT")
        {
            if(prev_stack.size() > 1)
            {
                next_stack.push(prev_stack.top());
                prev_stack.pop();
                Normal_mode(prev_stack.top());
            }
        }
        else if(key == "BACKSPACE")
        {
            string str;
            string cur_path = prev_stack.top();

            if(cur_path != root_path)
            //str = "/Users/" + usrmname + "/"
            {    
                //emptying the path arrays
                while(!prev_stack.empty()) 
                    prev_stack.pop();

                while(!next_stack.empty())
                    next_stack.pop();

                //breaking down the paths
                for(int i=0; i<cur_path.length(); i++)
                {
                    if(cur_path[i]=='/' && i==cur_path.length()-1)
                    {
                        //ignoring the cur_path (recent_dir)
                    }
                    else if(cur_path[i]=='/' && i<cur_path.length()-1 && i!=0)
                    {
                        str += cur_path[i];
                        prev_stack.push(str);
                    }
                    else
                    {
                        str += cur_path[i];
                    }
                }
                cout << prev_stack.top() << endl;
                Normal_mode(prev_stack.top());
            }
        }
        else if(key == "h")
        {
            //emptying the path arrays
            while(!prev_stack.empty()) 
                prev_stack.pop();

            while(!next_stack.empty())
                next_stack.pop();

            char cur_dir[1024];
            getcwd(cur_dir, 1024);
            //cout << "Current working directory: " << cur_dir << endl;
            string path(cur_dir);
            path += "/";

            prev_stack.push(path);
            Normal_mode(prev_stack.top());
        }
        else if(key == "COLON")
        {
            //enter into the command mode
            Command_mode(start, end);
        }
    }
}

vector<string> get_command(string input)
{
    // cout << "input : " << input << endl;
    vector<string> cmd;

    string str = input;
    string str2 = "";
    
    int len = str.length();
    int i=0;

    while(i < len)
    {
        if(str[i] == ' ')
        {
            cmd.push_back(str2);
            str2="";
            i++;
        }

        if(str[i] == '~')
            str2 += root_path;
            //curl_path

        else
            str2 += str[i];

        i++;
    }

    cmd.push_back(str2);

    // for(auto i: cmd)  {
    //     cout << i << endl;
    // }
    return cmd;   
}

int check(string input)
{
    if(input == "copy")                     //copy_file_dir
        return 1;

    else if(input == "move")                //move_file_dir
        return 2;

    else if(input == "rename")              //rename_file
        return 3;

    else if(input == "create_file")         //create_file
        return 4;

    else if(input == "create_dir")          //create_dir
        return 5;

    else if(input == "delete_file")         //delete_file
        return 6;

    else if(input == "delete_dir")          //delete_dir
        return 7;

    else if(input == "goto")                //goto_dir
    	return 8;
    
    else if(input == "search")              //search_file_dir
    	return 9;

    else
        return 0;
}

int search(string path1, string input)
{
    int file_dir = 0;
    dirent *dirPtr;
    struct dirent *dp;
    struct stat stats;

    const char *path = path1.c_str();
    DIR *dir = opendir(path);

    if (!dir) 
    {
        //if dir doesn't exist then return
        cout << "Error finding the directory. Try again!";
        return -1; 
    }
    
    while ((dp = readdir(dir)) != NULL)
    {  
        string filepath = path1 + dp->d_name;
        const char *filepth = filepath.c_str();
        
        if (stat(filepth, &stats) == 0)
        {
            if(stats.st_mode & S_IFDIR)
            {
                //for directory
                string str2 = dp->d_name;

                if(str2 == input)
                {
                    //dir found
                    cout << "\33[2K";               //delete current line in terminal
                    cout << "\033[49;0H";
                    cout << "TRUE  ";
                    return 1;
                }  

                else if(str2 != "." && str2 != "..")
                {
                    string str1 = filepath + '/';
                    file_dir = search(str1, input);
                    if(file_dir == 1)
                    {
                        cout << "\33[2K";           //delete current line in terminal
                        cout << "\033[49;0H";
                        cout << "TRUE  ";
                        return 1;
                    }
                }
            }
            else if(stats.st_mode & S_IFREG)
            {
                //for file
                if(input == dp->d_name)
                {
                    //file found
                    cout << "\33[2K";               //delete current line in terminal
                    cout << "\033[49;0H";
                    cout << "TRUE  ";
                    return 1;    
                }
            }
        }
    }
    closedir(dir);
    return file_dir;
}

void create_dir(string source)
{
    char src[100];
    strcpy(src, source.c_str());
    mkdir(src, 0777);
}

void delete_dir(string path_dir)
{
    const char *mdpath = path_dir.c_str();
    dirent *directoryPointer;
    DIR *currentdirectory = opendir(mdpath);

    if (!currentdirectory) 
    {
        //if dir doesn't exist then return
        cout << "Error finding the directory. Try again!";
        return; 
    }
    
    char path[500];

    while ((directoryPointer = readdir(currentdirectory)) != NULL)
    {
        if (strcmp(directoryPointer->d_name, ".") != 0 && strcmp(directoryPointer->d_name, "..") != 0)
        {
            strcpy(path, mdpath);
            strcat(path, "/");
            strcat(path, directoryPointer->d_name);
            delete_dir(path);
            if(directoryPointer->d_type==DT_REG)
            {
                if(remove(path))
                    cout << "Could not Delete : " << path << endl;
            }
        }
    }

    remove(mdpath);
    closedir(currentdirectory);   
}

char check_dir_file(string source)
{
    char src[1024];
    strcpy(src, source.c_str());
    struct stat stats;
    if (stat(src, &stats) == 0)
    {
        if( stats.st_mode & S_IFDIR )
        {
            return 'd';
        }
        else if( stats.st_mode & S_IFREG )
        {
            return 'f';
        }
    }
    return 'n';
}
 
void copy_dir(string source, string destination)
{
    // cout << "copy dir called" << endl;
    // cout << source << " " << destination  << endl;
    create_dir(destination);
    struct dirent *dp;
    struct stat stats;
    DIR *dir; 

    const char* path = (source).c_str();
    dir = opendir(path);

    if (!dir) 
    {
        //if dir doesn't exist then return
        cout << "Error finding the directory. Try again!";
        return; 
    }

    while ((dp = readdir(dir) ) != NULL)
    {
    	string fullpath = source + '/' + dp->d_name;
    	const char *file_dir_path= fullpath.c_str();
        // cout << "Copying " << fullpath  << " : "<< dp->d_name << endl;
        string str(dp->d_name);
        if(str == "." || str == "..")
        {
             //cout << "skipping ";
             continue;
        }
           
    	if (stat(file_dir_path, &stats) == 0)
        {
			if( stats.st_mode & S_IFDIR )
			{
                // cout << "dir recursion" << endl;
                // cout << fullpath << " " << destination << endl;
                copy_dir(fullpath, destination + '/' + dp->d_name);
			}
			else if( stats.st_mode & S_IFREG )
			{
                // cout << "copy recursion" << endl;
                // cout << fullpath << " " << destination << endl;
                copy_file(fullpath, destination + '/' + dp->d_name);
			}
		}
	}
}

void copy_file(string source, string destination)
{
    char src[1024], destn[1024];

    strcpy(destn, destination.c_str());
    strcpy(src, source.c_str());

    int fd1 = open(src, O_RDONLY, 0);
    int fd2 = open(destn, O_WRONLY| O_CREAT, 0);
    char c;

    while(read(fd1, &c, 1) != 0)
        write(fd2, &c, 1);

    copy_perm(src, destn);

    close(fd1);
    close(fd2);

    struct stat stats;
    if (stat(src, &stats) == -1) 
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
}

void delete_file(string path)
{
    char p[100];
    strcpy(p, path.c_str());

    int fd = remove(p);

    if(fd == -1)
    {
        cout << "Error : File can't be deleted. ";
        exit(0);
    }
}

void create_file(string source)
{
	char src[100];
    strcpy(src, source.c_str());

    int fd1 = open(src, O_CREAT, 0);

    chmod(src, S_IRUSR|S_IWUSR);

    close(fd1);
}

void goto_loc(string path)
{
	while(!next_stack.empty())
        next_stack.pop();

    prev_stack.push(path);
    Normal_mode(prev_stack.top());
}

void new_command()
{
    cout << "(Press 'k' to enter new command)";
    while(1)
    {
        string key = key_pressed();

        if(key == "k")
        {
            cout << "\33[2K";           //delete current line in terminal
            cout << "\033[49;0H";
            break;
        }
    }
}

void copy_perm(string str1, string str2)
{
    struct stat st;
    char path1[1024], path2[1024];

    strcpy(path1, str1.c_str());
    strcpy(path2, str2.c_str());

    stat(path1, &st);
    chmod(path2, st.st_mode);
}

void Command_mode(int start, int end)
{
    flag = 0;

    //Initiating the terminal window
    cout << "\033[48;0H";
	cout << "Welcome to Command Mode : ";
	cout << "\033[49;0H" << "";

    int flag = 0, i, len;

    while(flag == 0)
    {
        string key = key_pressed();

        if(key == "q" && str1.empty() || key == "Q")
        {
            cout << "\033[2J";              //clearing the terminal
            cout << "\033[0;0H";            //setting the cursor to top-left
            exit(0);
        }
        else if(key == "TAB")
        {
            Normal_mode(prev_stack.top());
        }
        else if(key == "BACKSPACE")
        {
            if(!str1.empty())
            {
                str1.pop_back();
                printf("\33[2K\r");
                cout << str1;
            }
        }
        else if(key == "ENTER")
        {
        	string search_str = "", path1 = "";
        	vector<string> command = get_command(str1);

        	str1 = "";

        	int cases = check(command[0]);

        	switch(cases) 
            {
            case 1:
                //copy_file_dir --> DONE
                i = 0;
                len = command.size();
                //cout << "cur_dir : "<< prev_stack.top() << endl;
                for(i = 1; i < len-1; i++)
                {
                    //cout << command[i] << endl;
                    str1 = prev_stack.top() + command[i];
                    str2 = command[len-1] + '/' + command[i];
                    //cout << str1 << " " << str2 << endl;
                    char check = check_dir_file(str1);
                    //cout << "checked" << endl;
                    if(check == 'f')
                    {
                        //cout << "file :" << endl;
                        copy_file(str1, str2);
                    }
                    else if(check == 'd')
                    {
                        //cout << "dir :" << endl;
                        copy_dir(str1, str2);
                        //cout << "dir copy done" << endl;
                    }
                }
                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "COPIED ";

                new_command();
                Command_mode(start, end);

                break;

            case 2:
                //move_file_dir --> DONE
                i=0;
                len=command.size();

                for(i = 1; i < len-1; i++)
                {
                    str1 = prev_stack.top() + command[i];
                    str2 = command[len-1] + '/' + command[i];

                    char check = check_dir_file(str1);

                    if(check == 'f')
                    {
                        copy_file(str1, str2);
                        delete_file(str1);
                    }
                    else if(check == 'd')
                    {
                        copy_dir(str1, str2);
                        delete_dir(str1);
                    }
                }
                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "MOVED ";

                new_command();
                Command_mode(start, end);

                break;
            
            case 3:
                //rename_file --> DONE
                str1 = prev_stack.top() + command[1];
                str2 = prev_stack.top() + command[2];
                
                copy_file(str1,str2);
                delete_file(str1);

                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "RENAMED ";

                new_command();
                Command_mode(start, end);

                break;

            case 4:
                //create_file --> DONE
                create_file(command[2] + '/' + command[1]);

                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "File CREATED ";

                new_command();
                Command_mode(start, end);

                break;

            case 5:
                //create_dir --> DONE
                create_dir(command[2] + '/' + command[1]);

                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "Directory CREATED ";

                new_command();
                Command_mode(start, end);

                break;

            case 6:
                //delete_file --> DONE
                len = command.size();
                search_str = command[len-1];
                
                delete_file(search_str);

                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "File DELETED ";

                new_command();
                Command_mode(start, end);

                break;

            case 7:
                //delete_dir --> DONE
                delete_dir(command[1]);

                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "Directory DELETED ";

                new_command();
                Command_mode(start, end);

                break;

            case 8:
                //goto --> DONE
                goto_loc(command[1] + '/');
                break;

            case 9:
                //search --> DONE
                search_str = command[1];
                path1 = prev_stack.top();

                int ans;
                ans = search(path1 , search_str);

                if(ans == 0)
                {
                    cout << "\33[2K";           //delete current line in terminal
                    cout << "\033[49;0H";
                    cout << "FALSE  ";
                }

                new_command();
                Command_mode(start, end);

                break;

            default:
                cout << "\33[2K";           //delete current line in terminal
                cout << "\033[49;0H";
                cout << "Invalid Command : (Press 'k' to enter new command)";
                while(1)
                {
                    string key2 = key_pressed();

                    if(key2 == "k")
                    {
                        cout << "\33[2K";           //delete current line in terminal
                        cout << "\033[49;0H";
                        Command_mode(start, end); 
                    }
                }
                break;
            }
       	}
        else
        {
            cout << key;
            str1 += key;
        }
    }
}

void Normal_mode(string directory_path)
{
    flag = 1;

    //getting files and directories
    get_dir_files(directory_path);

    //Initiating the terminal window
    cout << "\e[3;0;0t";                            //setting the terminal to top-left
    cout << "\e[8;55;205t";                         //setting height and width

    start_line = 0;
    end_line = 20;
    //cout << list_dir_files.size()-1 << endl;
    if(list_comp.size() < end_line)
        end_line = list_comp.size()-1;

    //printing details of files and directories
    print_details(start_line, end_line);
    //cout << directory_path << endl;

    //Non-Canonical Mode
    enable_non_canonical(start_line, end_line);
    
}

int main()
{
    char cur_dir[1024];
    getcwd(cur_dir, 1024);
    string path(cur_dir);
	path += "/";
    //cout << "Current working directory: " << path << endl;
    prev_stack.push(path);

    Normal_mode(prev_stack.top());

    return 0;
}