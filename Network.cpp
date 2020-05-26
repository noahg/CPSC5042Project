#include "Network.h"

using namespace std;

// getter for current client's socket identifier
int Network::getCurrentClientSocket() {
    return currentClientSocket;
}

// class constructor sets up the server and initializes users bank
Network::Network(int argc, char const *argv[]) {
    if (argc == 2) {
        port = atoi(argv[1]);
    } else {
        port = AUDREYS_PORT;
    }

    //initializing the bank of users
    ifstream userbankfile("UserBank.txt");
    string line;
    while (getline(userbankfile, line)) {
        User newUser;
        newUser.username = line;
        getline(userbankfile, line);
        newUser.password = line;
        users.push_back(newUser);
    }
    userbankfile.close();

    //creating a listening socket
    int opt = 1; 
    addrlen = sizeof(address); 
    
    // Creating socket file descriptor 
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    { 
        throw "Server's socket creation failed"; 
    } 

    //setting up the options for the socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        throw "setsockopt failed";
    } 

    //specifying the address and the port the server will connect to
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( port ); 

    // Forcefully attaching socket to the port  
    if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    { 
        throw "bind failed";
    } 

    //prepare to accept connections
    if (listen(server_fd, 3) < 0) 
    { 
        throw "listen failed"; 
    } 
    cout << "Server is listening on port " << port << endl;
}

// class destructor
Network::~Network() {
}

// this call is accepting a connection from a client and returning the 
// id of the socket of the new client connection
void Network::connect() {
    int newSocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (newSocket < 0) 
    { 
        throw "accept failed";
    } 
    cout << "New connection was made." << endl;
    currentClientSocket = newSocket;
    return;
}

// receives whatever is found in the specified socket
// and returns it
string Network::receive() {
    char userInput[1024] = {0};
    int valread = recv(currentClientSocket, userInput, 1024, 0);
    // cout << "valread: " << valread << endl;
    if (valread == 0) {
        return "";
    }
    return string(userInput);
}

//sends the inputted message into the specified socket
// will throw an error message if the sending fails
void Network::sendToClient(const string& message) {
    int valsend = send(currentClientSocket, message.c_str(), message.length(), 0);
    // cout << "valsend = " << valsend << endl;
    if (valsend == -1) {
        throw "error occured while sending data to server";
    }
}

//closes the socket and confirms closure into console
void Network::disconnectClient() {
    close(currentClientSocket);
    cout << "The client exit the game." << endl << endl;
}

int Network::receiveAndCheckAuthentication() {
    string loginOrSignup = receive(); //"log in " or "sign in"
    //cout << "user choice: " << loginOrSignin << endl;

    //send handshake
    sendToClient("ans");

    string authString = receive(); //"username=<value>,password=<value>"
    //cout << authString << endl; 

    //extract username and entered password from authString
    int equalPos = authString.find("=");
    int commaPos = authString.find(",");
    string inputUser = authString.substr(equalPos+1, commaPos - equalPos - 1);
    string inputPass = authString.substr(commaPos+10);

    if (loginOrSignup.compare("log in")==0) {
        return validateUsernamePassword(inputUser, inputPass);
    } else { // "sign up"
        return createNewUser(inputUser, inputPass);
    }
}

int Network::createNewUser(string inputUser, string inputPass) {
    //check if user already exists
    for (unsigned int i = 0; i < users.size(); i++) {
        if (users[i].username.compare(inputUser) == 0) {
            cout << "Auth fail: user already exists " << endl;
            return -3;
        }
    }
    
    //add to file
    ofstream userbankfile;
    userbankfile.open("UserBank.txt", ios_base::app);//append to file
    if (userbankfile.is_open()) {
        userbankfile << "\n"<< inputUser << "\n" << inputPass;
        userbankfile.close();
    }
    
    //todo: check if there is space for a new user
    //todo: make user capacity bigger if needed?

    //add to loaded userbank
    User newUser;
    newUser.username = inputUser;
    newUser.password = inputPass;
    users.push_back(newUser);   
    cout << "A new user signed up." << endl;
    return 0;
}

int Network::validateUsernamePassword(string inputUser, string inputPass) {
    //find the inputted user in our users bank, if so, initilize currentUserIndex
    bool isFound = false;
    for (unsigned int i = 0; i < users.size() && !isFound; i++) {
        if (users[i].username.compare(inputUser) == 0)  {
            //cout << "found user : " << user << endl;
            currentUserIndex = i;
            isFound = true;
        }
    }

    //if user is not found or if user is found but password is wrong, authentication fails
    if (!isFound) {
        cout << "Auth fail: user not found " << endl; //for string: " << authString << endl;
        return -1;
    } else if (inputPass.compare(users[currentUserIndex].password) != 0) {
        cout << "Auth fail: wrong password " << endl;
        return -2;
    } else {
        //cout << "Auth success " << endl;// for string: " << authString << endl;
        return 0;
    }
}


