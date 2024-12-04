#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

const std::vector<int> VEYON_PORTS = {11100, 11200, 11300};
const int SCAN_TIMEOUT = 1;

bool checkConnections() {
    bool connection_found = false;

    PMIB_TCPTABLE_OWNER_PID tcpTable = nullptr;
    DWORD size = 0;

    if (GetExtendedTcpTable(nullptr, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER) {
        tcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(size);
    }

    if (tcpTable && GetExtendedTcpTable(tcpTable, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
        for (DWORD i = 0; i < tcpTable->dwNumEntries; i++) {
            MIB_TCPROW_OWNER_PID row = tcpTable->table[i];
            if (row.dwState == MIB_TCP_STATE_ESTAB) {
                for (int port : VEYON_PORTS) {
                    if (ntohs((u_short)row.dwLocalPort) == port) {
                        connection_found = true;
                        break;
                    }
                }
            }
            if (connection_found) break;
        }
    }

    if (tcpTable) free(tcpTable);

    return connection_found;
}

void showNotification(const std::string& message) {
    NOTIFYICONDATAA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATAA);
    nid.uFlags = NIF_INFO;

    strncpy_s(nid.szInfo, message.c_str(), sizeof(nid.szInfo) - 1);
    strncpy_s(nid.szInfoTitle, "Connection Alert", sizeof(nid.szInfoTitle) - 1);

    Shell_NotifyIconA(NIM_ADD, &nid);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    Shell_NotifyIconA(NIM_DELETE, &nid);
}

int main() {
    std::cout << "Monitoring started. Press Ctrl+C to stop.\n";

    while (true) {
        if (checkConnections()) {
            showNotification("Connection detected!");
        }
        std::this_thread::sleep_for(std::chrono::seconds(SCAN_TIMEOUT));
    }

    return 0;
}
