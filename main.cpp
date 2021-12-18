#include<bits/stdc++.h>
#include<unistd.h> //getcwd() execl() fork()
#include<dirent.h> //struct dirent,opendir,DIR*
#include<termios.h> //tcgetattr(),tcsetattr()
#include<sys/stat.h> //stat()
#include<pwd.h> //getpwuid()
#include<grp.h>  //getgrgid()
#include<sys/ioctl.h> //terminal width and height
#include<fcntl.h> //open()
#include<cstdio> //fopen(),fclose(),fread(),fwrite()
#define clr printf("\033[H\033[J");
#define move(x,y) printf("\033[%d;%dH",x,y);
#define WINDOW_SIZE 15
using namespace std;
deque<struct dirent*> directories;
stack<string> back;
stack<string> front;
int cursor_pos=1,record=1;
string current_dir;
string home;
void interactive_mode();
int min(int a,int b)
{
    return a<b?a:b;
}

void get_dir(char* cwd_)
{
    directories.clear();
    struct dirent *dp;
    DIR *d=opendir(cwd_);
    if(d)
    {
        while((dp=readdir(d))!=NULL)
        {
            if( dp->d_name[0]=='.' )
            directories.push_front(dp);
            else
            directories.push_back(dp);
        }
    }
}


// void print_dir()
// {
//     for(int i=record-1;i<min(record+WINDOW_SIZE-1,directories.size());i++)
//     {
//         cout<<"#"<<i+1<<"       "<<directories[i]->d_name<<endl;
//     }
//     move(30,0);
//     cout<<current_dir;
//     move(cursor_pos,0);
// } 



void print_dir()
{
    for(int i=record-1;i<min(record+WINDOW_SIZE-1,directories.size());i++)
    {
        if((directories[i]->d_name)[0]=='.')
        {
            if((strlen(directories[i]->d_name)==1)||(strlen(directories[i]->d_name)==2 && (directories[i]->d_name)[1]=='.'))
            {
                // printf("%25s\n",directories[i]->d_name);
                printf("%d %25s\n",i+1,directories[i]->d_name);
                continue;
            }            
        }
        char cwd_[200];
        string current_path=current_dir+'/'+directories[i]->d_name;
        strcpy(cwd_,current_path.c_str());
        struct stat stats;
        lstat(cwd_,&stats);
        long double fileSize=stats.st_size/1024.0;//file size in KB

        //type (file/directory) and user,group,other rwx permission.
        string permission="";
        permission+=(stats.st_mode&S_IFDIR?'d':'f');
        permission+=(stats.st_mode&S_IRUSR?'r':'-');
        permission+=(stats.st_mode&S_IWUSR?'w':'-');
        permission+=(stats.st_mode&S_IXUSR?'x':'-');
        permission+=(stats.st_mode&S_IRGRP?'r':'-');
        permission+=(stats.st_mode&S_IWGRP?'w':'-');
        permission+=(stats.st_mode&S_IXGRP?'x':'-');
        permission+=(stats.st_mode&S_IROTH?'r':'-');
        permission+=(stats.st_mode&S_IWOTH?'w':'-');
        permission+=(stats.st_mode&S_IXOTH?'x':'-');
        string last_modified=ctime(&stats.st_mtime);//last modified time of file(time.h)
        string name_user=getpwuid(stats.st_uid)->pw_name;
        string name_group=getgrgid(stats.st_uid)->gr_name;
        // printf("%25s\t%20sKB\t",directories[i]->d_name,to_string(fileSize).c_str());
        printf("%d %25s\t%20sKB\t",i+1,directories[i]->d_name,to_string(fileSize).c_str());
        cout<<name_user<<"\t"<<name_group<<"\t"<<permission<<"\t"<<last_modified;
    }
    move(30,0);
    cout<<"\x1b[1;32mCurrent Diretory:-$\x1b[1;0m"<<current_dir;
    move(cursor_pos,0);
} 



void down()
{
    if((cursor_pos+record-1)==directories.size())
    return;
    if(cursor_pos==WINDOW_SIZE)
    {
        record++;
        clr;
        move(1,0);
        print_dir();
        move(cursor_pos,0);
    }
    else
    {
        clr;
        move(1,0);
        cursor_pos++;
        print_dir();
        move(cursor_pos,0);
    }
}
void up()
{
    if((cursor_pos+record-1)==1)
    return;
    if(cursor_pos==1)
    {
        record--;
        clr;
        move(1,0);
        print_dir();
        move(cursor_pos,0);
    }
    else
    {
        clr;
        move(1,0);
        cursor_pos--;
        print_dir();
        move(cursor_pos,0);
        
    }
}
void scroll_down()
{
    if(record+WINDOW_SIZE>directories.size())
    return;
    record+=WINDOW_SIZE;
    clr;
    move(1,0);
    print_dir();
    move(1,0);
    cursor_pos=1;
}
void scroll_up()
{
    if(record==1)
    return;
    record=record-WINDOW_SIZE<1?1:record-WINDOW_SIZE;
    clr;
    move(1,0);
    print_dir();
    move(1,0);
    cursor_pos=1;
}
void enter()
{
    char next[200];
    strcpy(next,directories[(record-1)+(cursor_pos-1)]->d_name);
    struct stat stats;
    if(next[0]=='.' && strlen(next)==1)
    {
        cursor_pos=1;
        record=1;
        clr;
        move(cursor_pos,0);
        print_dir();
        return;
    }
    if(next[0]=='.' && next[1]=='.' && strlen(next)==2)
    {
        char cwd_[200];
        strcpy(cwd_,current_dir.c_str());
        for(int i=strlen(cwd_)-1;i>=0;i--)
        if(cwd_[i]=='/')
        {
            cwd_[i]='\0';
            break;
        }
        if(strlen(cwd_)==0)
        {
            cursor_pos=1;
            record=1;
            clr;
            move(cursor_pos,0);
            print_dir();
            return;
        }
        back.push(current_dir);
        current_dir=string(cwd_);
        clr;
        get_dir(cwd_);
        cursor_pos=1;
        record=1;
        move(1,0);
        print_dir();
        return;
    }
        char next_path[200];
        strcpy(next_path,(current_dir+'/'+string(next)).c_str());
        lstat(next_path,&stats);
        if(stats.st_mode & S_IFDIR) // if next is directory
        {
            back.push(current_dir);
            current_dir=current_dir+'/'+string(next);
            char cwd_[200];
            strcpy(cwd_,current_dir.c_str());
            get_dir(cwd_);
            clr;
            record=1;
            cursor_pos=1;
            move(1,0);
            print_dir();
            // move(35,0);
            // cout<<"."<<next<<". is a directory\n";
            // move(cursor_pos,0);
        }
        else // if next is file
        {   
            if(fork()==0)//child process
            {
                char file[200];
                strcpy(file,(current_dir+'/'+string(next)).c_str());
                execl("/usr/bin/xdg-open","xdg-open",file,NULL);
                exit(0);
            }
            // move(35,0);
            // cout<<"."<<next<<". is NOT a directory\n";
            // move(cursor_pos,0);
        }
    
    
}
void left()
{
    if(back.empty())
    return;
    char cwd_[200];
    current_dir=back.top();
    front.push(back.top());
    strcpy(cwd_,back.top().c_str());
    clr;
    get_dir(cwd_);
    cursor_pos=1;
    record=1;
    move(cursor_pos,0);
    print_dir(); 
    back.pop();   
}
void right()
{
    if(front.empty())
    return;
    char cwd_[200];
    current_dir=front.top();
    back.push(front.top());
    strcpy(cwd_,front.top().c_str());
    clr;
    get_dir(cwd_);
    cursor_pos=1;
    record=1;
    move(cursor_pos,0);
    print_dir(); 
    front.pop();
}
void backspace()
{
    char cwd_[200];
    strcpy(cwd_,current_dir.c_str());
    for(int i=strlen(cwd_)-1;i>=0;i--)
    if(cwd_[i]=='/')
    {
        cwd_[i]='\0';
        break;
    }
    if(strlen(cwd_)==0)
    {
        cursor_pos=1;
        record=1;
        clr;
        move(cursor_pos,0);
        print_dir();
        return;
    }
    back.push(current_dir);
    current_dir=string(cwd_);
    clr;
    get_dir(cwd_);
    cursor_pos=1;
    record=1;
    move(1,0);
    print_dir();
    return;
}
void to_home()
{
    char temp_dir[200];
    back.push(current_dir);
    strcpy(temp_dir,home.c_str());
    get_dir(temp_dir);
    current_dir=home;
    cursor_pos=1;
    record=1;
    clr;
    move(cursor_pos,0);
    print_dir();
}

void normal_mode()
{
    clr;
    char cwd_[200];
    strcpy(cwd_,home.c_str());
    record=1;
    cursor_pos=1;
    move(cursor_pos,0);
    getcwd(cwd_,200);
    get_dir(cwd_);
    print_dir();
    move(1,0);
    /*
    UP 65
    DOWN 66
    LEFT 68
    RIGHT 67
    BACKSPACE 127
    ENTER 10
    */
    while(1)
    {
        char ch=cin.get();
        switch(ch)
        {
            case 127://BACKSPACE
                backspace();
                break;
            case 10://ENTER
                enter();
                break;
            case 65://UP
                up();
                break;
            case 66://DOWN
                down();
                break;
            case 68://LEFT
                left();
                break;
            case 67://RIGHT
                right();
                break;
            case 'k'://SCROLL UP
                scroll_up();
                break;
            case 'l'://SCROLL DOWN
                scroll_down();
                break;
            case 'h'://HOME
                to_home();
                break;
            case ':'://Interactive mode
                interactive_mode();
                return;
                break;
            case 'q'://EXIT 
                clr;
                cout<<endl;
                return;
                break;
            // default:cout<<"Enter correct choice  ";
        }
    }
}
void enable_rawmode()
{
	struct termios old, new_;
    tcgetattr(STDIN_FILENO, &old);
    new_ = old;
    new_.c_lflag &= (~ICANON);
    new_.c_lflag &= (~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_);
}
void disable_rawmode()
{
	termios old,new_;
	tcgetattr(STDIN_FILENO, &old);
	new_ = old;
	new_.c_lflag |= (ICANON);
    new_.c_lflag |= (ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_);
}
void process_command(string command)
{
    /*
    creat() rename() mkdir()
    */
    clr;
    move(1,0);
    cout<<"\x1b[1;32mEntered Command Mode,Please enter your command-$\x1b[1;0m";
    move(30,0);
    cout<<"\x1b[1;32mCurrent Diretory:-$\x1b[1;0m"<<current_dir;
    vector<string> commands;
    string temp="";
    if(command.length()==0)
    {
        move(1,0);
        cout<<"Please enter a valid command";
        return;
    }
    for(int i=0;i<command.length();i++)
    {
        if(command[i]==' ')
        {
            commands.push_back(temp);
            temp="";
            continue;
        }
        temp.push_back(command[i]);
    }
    commands.push_back(temp);
    move(2,0);
    // for(int i=0;i<commands.size();i++)
    // {
    //     cout<<commands[i]<<endl;
    // }
    
    if(commands[0]=="rename")//rename()
    {
        if(commands.size()==3)
        {
            char cwd_[200];
            strcpy(cwd_,current_dir.c_str());
            get_dir(cwd_);
            for(int i=0;i<directories.size();i++)
            {
                if(string(directories[i]->d_name)==commands[1])
                {
                    if(rename(commands[1].c_str(),commands[2].c_str())==-1)
                    cout<<"Unable to rename";
                    else
                    cout<<commands[1]<<" has been renamed to "<<commands[2];
                    return;
                }
            }
            cout<<commands[1]<<" not found";
        }
        else
        {
            cout<<"Please provide valid number of arguments with rename command (rename <old_filename> <new_filename>)";
        }        
    }
    else
    if(commands[0]=="move")
    {
        if(commands.size()>=3)
        {
            unsigned long long size;
            for(int i=1;i<commands.size()-1;i++)
            {
                char fileName[200],dest_dir[200],buffer[BUFSIZ];
                strcpy(fileName,commands[i].c_str());
                FILE* src=fopen(fileName,"r");
                if(commands[commands.size()-1][0]=='.')
                {
                    strcpy(dest_dir,(current_dir+'/'+commands[i]).c_str());
                }
                else
                if(commands[commands.size()-1][0]=='~')
                {
                    commands[commands.size()-1].erase(0,1);
                    strcpy(dest_dir,(home+commands[commands.size()-1]+'/'+commands[i]).c_str());
                }
                else
                strcpy(dest_dir,(commands[commands.size()-1]+'/'+commands[i]).c_str());
                FILE* dest=fopen(dest_dir,"w");
                while(size=fread(buffer,1,BUFSIZ,src))
                fwrite(buffer,1,size,dest);
                fclose(src);fclose(dest);
                struct stat stats;
                lstat(dest_dir,&stats);
                if(!(stats.st_mode & S_IFDIR))
                {
                    remove(commands[i].c_str());
                }
                
            }
            cout<<"Files have been removed";

        }
        else
        {
            cout<<"Please provide valid number of arguments with move command (move <source_file(s)> <dest_dir_path>)";
        } 
    }
    else
    if(commands[0]=="copy")
    {
        if(commands.size()>=3)
        {
            unsigned long long size;
            for(int i=1;i<commands.size()-1;i++)
            {
                char fileName[200],dest_dir[200],buffer[BUFSIZ];
                strcpy(fileName,commands[i].c_str());
                FILE* src=fopen(fileName,"r");
                if(commands[commands.size()-1][0]=='.')
                {
                    strcpy(dest_dir,(current_dir+'/'+commands[i]).c_str());
                }
                else
                if(commands[commands.size()-1][0]=='~')
                {
                    commands[commands.size()-1].erase(0,1);
                    strcpy(dest_dir,(home+commands[commands.size()-1]+'/'+commands[i]).c_str());
                }
                else
                strcpy(dest_dir,(commands[commands.size()-1]+'/'+commands[i]).c_str());
                FILE* dest=fopen(dest_dir,"w");
                while(size=fread(buffer,1,BUFSIZ,src))
                fwrite(buffer,1,size,dest);
                fclose(src);fclose(dest);
                cout<<"Files have been copied";
            }

                cout<<"Files have been copied";
        }
        else
        {
            cout<<"Please provide valid number of arguments with move command (copy <source_file(s)> <dest_dir_path>)";
        } 
    }
    else
    if(commands[0]=="create_file")//creat() 
    {
        if(commands.size()==3)
        {
            if(commands[2][0]=='~')
            {
                commands[2].erase(0,1);
                creat((home+commands[2]+'/'+commands[1]).c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);
                cout<<commands[1]<<" File created Successfully";
            }
            else
            if(commands[2][0]=='.')
            {
                creat((current_dir+'/'+commands[1]).c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);
                cout<<commands[1]<<" File created Successfully";
            }
            else
            {
                creat(commands[1].c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);
                cout<<commands[1]<<" File created Successfully";
            }
        }
        else
        {
            cout<<"Please provide valid number of arguments with create_file command (create_file <file_name> <destination_path>  or create_file <file_name>)";
        }
    }
    else
    if(commands[0]=="create_dir")//mkdir() 
    {
        if(commands.size()==3)
        {
            if(commands[2][0]=='~')
            {
                commands[2].erase(0,1);
                mkdir((home+commands[2]+'/'+commands[1]).c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);
                cout<<commands[1]<<" Directory created Successfully";
            }
            else
            if(commands[2][0]=='.')
            {
                mkdir((current_dir+'/'+commands[1]).c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);
                cout<<commands[1]<<" Directory created Successfully";
            }
            else
            {
                mkdir(commands[1].c_str(),S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);
                cout<<commands[1]<<" File created Successfully";
            }
        }
        else
        {
            cout<<"Please provide valid number of arguments with create_file command (create_file <file_name> <destination_path>  or create_file <file_name>)";
        }
    }
    else
    if(commands[0]=="delete_file")//remove()
    {
        if(commands.size()==2)
        {
            remove(commands[1].c_str());
            cout<<commands[1]<<" deleted successfully";
        }
        else
        {
            cout<<"Please provide valid number of arguments with delete_file command (delete_file <file_path> )";
        }
    }
    else
    if(commands[0]=="delete_dir")
    {
        cout<<"Delete Dir";
    }
    else
    if(commands[0]=="goto")//modify current_dir 
    {
        if(commands.size()==2)
        {
            char cwd_[200];
            strcpy(cwd_,commands[1].c_str());
            DIR *d=opendir(cwd_);
            if(d)
            {
                current_dir=commands[1];
                clr;
                move(30,0);
                cout<<"\x1b[1;32mCurrent Diretory:-$\x1b[1;0m"<<current_dir;
                move(1,0);
                cout<<"\x1b[1;32mEntered Command Mode,Please enter your command-$\x1b[1;0m";
                move(2,0);
                cout<<"Reached to Directory:"<<current_dir;
            }
            else
            {
                cout<<"Directory:"<<commands[1]<<" does not exist.Please enter path wrt root (/home/....)";
            }
        }
        else
        {
            cout<<"Please provide valid number of arguments with goto command (goto <directory/file location path(absolute)>)";
        }
    }
    else
    if(commands[0]=="search")//recursive search
    {
        if(commands.size()==2)
        {
            char cwd_[200];
            strcpy(cwd_,commands[1].c_str());
            DIR *d=opendir(cwd_);
            if(d)
            {
                current_dir=commands[1];
                cout<<"Reached to Directory:"<<current_dir;
            }
            else
            {
                cout<<"Directory:"<<commands[1]<<" does not exist.Please enter path wrt root (/home/....)";
            }
        }
        else
        {
            cout<<"Please provide valid number of arguments with search command(search <filename> or search <directory>)";
        }
    }
    else
    {
        cout<<"Please enter valid command";
    }
    

}
void interactive_mode()
{
    //disable_rawmode();
    clr;
    move(30,0);
    cout<<"\x1b[1;32mCurrent Diretory:-$\x1b[1;0m"<<current_dir;
    move(1,0);
    cout<<"\x1b[1;32mEntered Command Mode,Please enter your command-$\x1b[1;0m";
    
    string command;
    while(1)
    {
        char ch=cin.get();
        switch(ch)
        {
            case 'q':move(0,0);
                clr;
                disable_rawmode();
                exit(0);
                break;
            case 'Q':move(0,0);
                clr;
                disable_rawmode();
                exit(0);
                break;
            case 27://ESC
                normal_mode();
                return;
                break;
            case 10://ENTER
                process_command(command);
                move(30,0);
                cout<<"\x1b[1;32mCurrent Diretory:-$\x1b[1;0m"<<current_dir;
                move(1,0);
                cout<<"\x1b[1;32mEntered Command Mode,Please enter your command-$\x1b[1;0m";
                command.clear();
                break; 
            case 127://BACKSPACE
                if(command.size()==0)
                break;
                command.pop_back();
                clr;
                move(30,0);
                cout<<"\x1b[1;32mCurrent Diretory:-$\x1b[1;0m"<<current_dir;
                move(1,0);
                cout<<"\x1b[1;32mEntered Command Mode,Please enter your command-$\x1b[1;0m"<<command;
                break;
            default:cout<<ch;
                command.push_back(ch);
                break;

        }       
    }
    return;
}

int main()
{
    clr;
    struct stat stats;
    char cwd_[200];
    getcwd(cwd_,200);
    current_dir=string(cwd_);
    lstat(cwd_,&stats);
    string name_user=getpwuid(stats.st_uid)->pw_name;
    home="/home/"+name_user;
    
    // struct winsize w;
    // ioctl(fileno(stdout), TIOCGWINSZ, &w);
    // int width = (int)(w.ws_col);
    // int height = (int)(w.ws_row);
    // cout<<width<<" "<<height<<endl;
    // cout << "\e[8;100;100t";//set widht and height of terminal
    // ioctl(fileno(stdout), TIOCGWINSZ, &w);
    //  width = (int)(w.ws_col);
    //  height = (int)(w.ws_row);
    // cout<<width<<" "<<height<<endl;

    
    // home=string(cwd_);
    enable_rawmode();
    normal_mode();
    disable_rawmode();
    clr;
    return 0;
}   