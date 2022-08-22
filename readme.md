Listing currently open directories using low-level approach.

* Open an handle to explorer.exe with PROCESS_DUP_HANDLE and PROCESS_QUERY_INFORMATION rights.
* Get a list of names with open windows related to explorer.exe
* Iterate through handles, duplicate each one that belongs to explorer, check if it is a File Object and also in the window list.
