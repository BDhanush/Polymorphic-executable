#include <bits/stdc++.h>
#include <fcntl.h>

using namespace std;

unordered_map<int,int> global,back;

void daemonize() {
    pid_t pid = fork(); // Create a child process
    
    if (pid < 0) {
        std::cerr << "Fork failed!\n";
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process exits
        exit(EXIT_SUCCESS);
    }

    // Child process continues
    if (setsid() < 0) {
        std::cerr << "Failed to create new session.\n";
        exit(EXIT_FAILURE);
    }

    // Redirect standard file descriptors (optional but useful)
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    open("/dev/null", O_RDONLY);   // Redirect stdin to /dev/null
    open("/dev/null", O_RDWR);     // Redirect stdout to /dev/null
    open("/dev/null", O_RDWR);     // Redirect stderr to /dev/null
}

void copyExecutable(const char* sourcePath, const char* destPath) {
    std::ifstream src(sourcePath, std::ios::binary);
    std::ofstream dst(destPath, std::ios::binary);

    if (!src || !dst) {
        std::cerr << "Failed to open file for copying.\n";
        exit(EXIT_FAILURE);
    }

    dst << src.rdbuf(); // Copy content
    std::cout << "Executable copied to temporary location.\n";
}

// Function to modify the copied executable
void modifyExecutableMutate(const char* srcPath,const char* destPath) {
    // std::ofstream inputExe(path, std::ios::binary | std::ios::out | std::ios::in);
    ifstream inputExe(srcPath, std::ios::binary);

    // Check if the file is open
    if (!inputExe.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    // Get the size of the file
    inputExe.seekg(0, std::ios::end);        // Move to the end of the file
    std::streamsize size = inputExe.tellg(); // Get the position (which is the size)
    inputExe.seekg(0, std::ios::beg);        // Move back to the beginning

    // Create a vector to hold the data
    std::vector<char> buffer(size);

    // Read the file into the buffer
    if (inputExe.read(buffer.data(), size)) {
        std::cout << "File read successfully!" << std::endl;
    } else {
        std::cerr << "Error reading file." << std::endl;
    }

    // Close the file
    inputExe.close();

    vector<int> index(size);
    for(int i=0;i<size;i++)
    {
        index[i]=i;
    }

    random_shuffle(index.begin(),index.end());
    for(int i=0;i<size;i+=2)
    {
        swap(buffer[index[i]],buffer[index[i+1]]);
        int first=index[i],second=index[i+1];
        if(back.find(first)==back.end())
        {
            back[first]=first;
        }
        if(back.find(second)==back.end())
        {
            back[second]=second;
        }
        global[back[first]]=second;
        global[back[second]]=first;
        swap(back[second],back[first]);
    }
    ofstream dst(destPath, std::ios::binary);
    for(int i=0;i<size;i++)
    {
        dst<<buffer[i];
    }

}

void modifyExecutableDemutate(const char* srcPath,const char* destPath) {
    // std::ofstream inputExe(path, std::ios::binary | std::ios::out | std::ios::in);
    ifstream inputExe(srcPath, std::ios::binary);

    // Check if the file is open
    if (!inputExe.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    // Get the size of the file
    inputExe.seekg(0, std::ios::end);        // Move to the end of the file
    std::streamsize size = inputExe.tellg(); // Get the position (which is the size)
    inputExe.seekg(0, std::ios::beg);        // Move back to the beginning

    // Create a vector to hold the data
    std::vector<char> buffer(size);

    // Read the file into the buffer
    if (inputExe.read(buffer.data(), size)) {
        std::cout << "File read successfully!" << std::endl;
    } else {
        std::cerr << "Error reading file." << std::endl;
    }

    // Close the file
    inputExe.close();

    ofstream dst(destPath, std::ios::binary);
    for(int i=0;i<size;i++)
    {
        if(global.find(i)==global.end())
        {
            global[i]=i;
        }
        dst<<buffer[global[i]];
    }

}

void replaceExecutable(const char* originalPath, const char* modifiedPath) {
    if (rename(modifiedPath, originalPath) == 0) {
        std::cout << "Executable replaced successfully.\n";
    } else {
        std::cerr << "Failed to replace the executable.\n";
    }
}

void mutate(const char* path)
{
    string temp=path;
    temp+="1";
    modifyExecutableMutate(path,temp.c_str());
    replaceExecutable(path,temp.c_str());
}

void deMutate(const char* path)
{
    string temp=path;
    temp+="1";
    modifyExecutableDemutate(path,temp.c_str());
    replaceExecutable(path,temp.c_str());
}

int main(int argc,char* args[])
{
    if(argc!=3)
    {
        cout<<"incorrect number of arguments\n";
        cout<<"usage: <epoch> <executable_path>\n";
        return 0;
    }
    long long int targetEpoch;

    try{
        targetEpoch=stoll(args[1]);
    }catch (const std::invalid_argument& e) {
        std::cout << "Invalid epoch value: " << e.what() << "\n";
        return 0;
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: " << e.what() << "\n";
        return 0;
    }

    daemonize();
        
    auto now = std::chrono::system_clock::now();
    auto epochNow = std::chrono::system_clock::to_time_t(now);
    bool isTargetTime=targetEpoch>epochNow;
    while(isTargetTime)
    {
        mutate(args[2]);
        now=std::chrono::system_clock::now();
        epochNow=std::chrono::system_clock::to_time_t(now);
        isTargetTime=targetEpoch>epochNow;
    }
    deMutate(args[2]);
    string command="chmod u+x ";
    command+=args[2];
    system(command.c_str());
    return 0;
}