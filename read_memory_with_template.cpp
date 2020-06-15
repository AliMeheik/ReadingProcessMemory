#include <string>
#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <optional>

using std::cin;
using std::cout;
using std::endl;
using std::hex;
using std::wstring;
using std::optional;


DWORD GetProcessID_byName( const std::wstring* process_name ) {
    PROCESSENTRY32 process_buffer;
    DWORD processId = 0;
	HANDLE process_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );

	if( process_snapshot != INVALID_HANDLE_VALUE ) {
		process_buffer.dwSize = sizeof( PROCESSENTRY32 );
		if( Process32First( process_snapshot, &process_buffer ) ) {
			do {
				if( !_wcsicmp( process_name->c_str(), process_buffer.szExeFile ) ) {
					processId = process_buffer.th32ProcessID;
                    break;
				}
			} while( Process32Next( process_snapshot, &process_buffer ) );
		}
        CloseHandle( process_snapshot );
	}
    return processId;
}

template<typename Read_type>
const optional<Read_type> ReadMemoryValue( const uintptr_t& address, const HANDLE *process_handle ) {
	Read_type read_value;
	if( !ReadProcessMemory( *process_handle, (LPCVOID)address, &read_value, sizeof( Read_type ), nullptr ) ) {
        return std::nullopt;
    }
	return read_value;
}

int main() {
    do {
        cout << "Enter process name: ";
        std::wstring process_name;
        getline( std::wcin, process_name );			
        DWORD process_id = GetProcessID_byName( &process_name );
        if( !process_id ) {
            cout << "Failed to get process id" << endl;
            break;
        }

        HANDLE process_handle = OpenProcess( PROCESS_ALL_ACCESS, false, process_id );

        if( !process_handle ) {
            cout << "Failed to open process" << endl;
            break;
        }

        uintptr_t address;
        cout << "Enter variable address: ";
        cin >> hex >> address;

        optional<int> value = ReadMemoryValue<int>( address, &process_handle );

        if( !value ) {
            cout << "Read failed" << endl;
            break;
        } 

        cout << "Read value: " << *value << endl;

    } while( 0 );

	system( "pause" );
	return 0;
}




