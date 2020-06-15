#include <string>
#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <optional>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::wstring;

std::optional<int> Get_process_id( const wstring process_name ) {

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof( PROCESSENTRY32 );
    HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
    if( snapshot != INVALID_HANDLE_VALUE ) {

        if( Process32First( snapshot, &entry ) ) {
            do {
                wstring sProcess_name = wstring( entry.szExeFile );
                if( process_name == sProcess_name ) {
                    return entry.th32ProcessID;
                }
            } while( Process32Next( snapshot, &entry ) );
        }
    }
    return std::nullopt;
}

int main() {

    wstring process_name;
    cout << "Enter process name: ";
    getline( std::wcin, process_name );
    std::optional<int> process_id = Get_process_id( process_name );
    if( process_id ) {
        HANDLE process_handle = OpenProcess( PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, *process_id );
        if( process_handle ) {
            uintptr_t target_address;
            DWORD read_value;
            cout << "Enter target address: ";
            cin >> std::hex >> target_address;
            ReadProcessMemory( process_handle, ( LPCVOID )target_address, &read_value, sizeof( DWORD ), nullptr );
            cout << "Read value is: " << read_value << endl;
            CloseHandle( process_handle );
        } else {
            cout << "Error getting process handle, ERROR_CODE [ " << GetLastError() << " ]" << endl;
        }
    } else {
        cout << "Process not found.." << endl;
    }
    system( "pause" );

    return 0;
}
