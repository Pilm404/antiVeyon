#include <iostream>
#include <cstdlib>
#include <Windows.h>

using namespace std;

bool solution() {
    string _solution;
    cout << "Are you sure you want to continue? [y/n]";
    cin >> _solution;
    if (_solution == "y") {
        return true;
    } else {
        return false;
    }
}

bool isUserRunSoftwareUsAdmin() {
    bool status = false;
    HANDLE tokenHandle = nullptr;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle)) {
        TOKEN_ELEVATION elevation;
        DWORD returnLength = 0;

        if (GetTokenInformation(tokenHandle, TokenElevation, &elevation, sizeof(elevation), &returnLength)) {
            status = elevation.TokenIsElevated;
        }
        CloseHandle(tokenHandle);
    }
    return status;
}

bool isServiceRunning(const string serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        std::cerr << "Failed to open Service Control Manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
    if (!hService) {
        if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST) {
            std::cout << "Service " << serviceName << " does not exist." << std::endl;
        } else {
            std::cerr << "Failed to open service. Error: " << GetLastError() << std::endl;
        }
        CloseServiceHandle(hSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS serviceStatus;
    DWORD bytesNeeded;

    if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&serviceStatus, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
        bool isRunning = (serviceStatus.dwCurrentState == SERVICE_RUNNING);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return isRunning;
    } else {
        std::cerr << "Failed to query service status. Error: " << GetLastError() << std::endl;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return false;
}

bool serviceExists(const string serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        return false;
    }

    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
    if (hService) {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return true;
    }
    CloseServiceHandle(hSCManager);
    return false;
}

int stopVeyonServise(const string serviceName) {
    if (!isUserRunSoftwareUsAdmin() && !isServiceRunning(serviceName)) {
        cout << "You do not have administrator rights, to restore the service you need to restart the computer, do you want to do this now?" << endl;
        if (solution()) {
            system("shutdown -r -t 0");
        } else {
            cout << "Return to main menu" << endl;
            return 0;
        }
    } else if (isUserRunSoftwareUsAdmin() && isServiceRunning(serviceName)) {
        if (!solution()) {
            return 0;
        }
        string stopCommand = "sc stop " + serviceName;

        cout << "Stopping service: " << serviceName << std::endl;

        int result = system(stopCommand.c_str());

        if (result == 0) {
            cout << "Service stopped successfully for this session." << endl;
        } else {
            cerr << "Failed to stop the service. Error code: " << result << endl;
            return 1;
        }

        return 0;
    } else if (isUserRunSoftwareUsAdmin() && !isServiceRunning(serviceName)) {
        if (!solution()) {
            return 0;
        }
        string stopCommand = "sc start " + serviceName;

        cout << "Starting service: " << serviceName << endl;

        int result = system(stopCommand.c_str());

        if (result == 0) {
            cout << "Service started successfully for this session." << endl;
            return 0;
        } else {
            cerr << "Failed to start the service. Error code: " << result << endl;
            return 1;
        }
    } else {
        cout << "You don't have administrator rights";
        return 0;
    }
}

int main() {
    string commmand;
    const string serviceName = "VeyonService";

    if (!serviceExists(serviceName)) {
        cout << "Could not find service Veyon" << endl << "Press ENTER to continue";
        cin.get();
        return 0;
    } else {
        cout << "The service is currently in a state of " << isServiceRunning(serviceName) << "." << endl;
    }
    if (!isUserRunSoftwareUsAdmin()) {
        cout << "The program was't launched as administrator, some errors are possible" << endl;
    }
    while (true) {
        cout << endl << endl << "Please select the action number:" << endl;
        cout << "1. Stop or restart Veyon service" << endl;
        cout << "2. Start or stop module" << endl;
        cout << "3. or quit for exit" << endl;
        cin >> commmand;

        if (commmand == "3" || commmand == "quit" || commmand == "q" || commmand == "Q" || commmand == "Quit") {
            return 0;
        } else if (commmand == "1") {
            cout << "Are you really want to stop Veyon Master?" << endl;
            stopVeyonServise(serviceName);
        } else if (commmand == "2") {
            cout << "Are you really want to start Veyon Master?" << endl;
        } else {
            cout << "Invalid command. Please try again." << endl;
        }
    }
    return 0;
}
