# Concurrent Hash Table

This project implements a concurrent hash table in C for COP 4600.
It is designed to help students visualize concurrency concepts.

## 🚀 Features
- Implements read-write locks
- Utilizes condition variables
- Uses pthread library for threading

## 🏁 Getting Started

### 🛠️ Compilation

#### Windows
1. Open the terminal from the project folder.
2. Run the command: `mingw32-make`
3. To run the compiled file, use: `chash.exe`

#### Linux
1. Open the terminal from the project folder.
2. Run the command: `make`
3. To run the compiled file, use: `./chash`

## 📖 Usage

### ⚙️ General
1. Ensure that `commands.txt` is in the same directory as the executable.
2. Run the executable (`chash.exe` on Windows or `./chash` on Linux).
3. The program will read the commands from `commands.txt` and output the results to `output.txt`.

### 📄 Commands and Output files

#### Commands File
The `commands.txt` file contains a list of commands that the program will execute. Each command should be on a new line. The format of the commands is as follows:
- `insert <key> <value>`: Inserts a key-value pair into the hash table.
- `delete <key> 0`: Deletes the key-value pair associated with the given key.
- `search <key> 0`: Retrieves the value associated with the given key.

Example `commands.txt`:
```
threads,10,0
insert,Richard Garriot,40000
insert,Sid Meier,50000
insert,Shigeru Miyamoto,51000
delete,Sid Meier,0
insert,Hideo Kojima,45000
insert,Gabe Newell,49000
insert,Roberta Williams,45900
delete,Richard Garriot,0
insert,Carol Shaw,41000
search,Sid Meier,0
```

#### Output File
The `output.txt` file contains the results of the commands executed by the program. Each result corresponds to a command from the `commands.txt` file. The format of the output is as follows:
- For `insert` commands, the output will include the thread ID, the operation, the key, and the value.
- For `delete` commands, the output will include the thread ID, the operation, and the key.
- For `search` commands, the output will include the thread ID, the operation, and the result (`NOT FOUND` if the key does not exist, otherwise the key and value).
- Additionally, the output will include information about lock acquisitions and releases.
- Lastly, the final state of the table will be output, sorted by hash value.

Example `output.txt`:
```
Running 10 threads
1731624044100225536: INSERT, 1874280167, Shigeru Miyamoto, 51000
1731624044100227328: WRITE LOCK ACQUIRED
1731624044100245504: INSERT, 3449751099, Sid Meier, 50000
1731624044100284416: WRITE LOCK RELEASED
1731624044100291840: WAITING ON INSERTS
1731624044100296448: INSERT, 2569965317, Hideo Kojima, 45000
1731624044100297216: WRITE LOCK ACQUIRED
1731624044100350976: INSERT, 448054155, Gabe Newell, 49000
1731624044100388352: INSERT, 2500551319, Richard Garriot, 40000
1731624044100404992: INSERT, 909366975, Roberta Williams, 45900
1731624044100445184: WAITING ON INSERTS
1731624044100449792: INSERT, 660427450, Carol Shaw, 41000
1731624044100464384: WRITE LOCK RELEASED
1731624044100466432: READ LOCK ACQUIRED
1731624044100466944: SEARCH, NOT FOUND, NOT FOUND
1731624044100467200: READ LOCK RELEASED
1731624044100488192: WRITE LOCK ACQUIRED
1731624044100493824: WRITE LOCK RELEASED
1731624044100502016: WRITE LOCK ACQUIRED
1731624044100506112: WRITE LOCK RELEASED
1731624044100516608: WRITE LOCK ACQUIRED
1731624044100520704: WRITE LOCK RELEASED
1731624044100537856: WRITE LOCK ACQUIRED
1731624044100541952: WRITE LOCK RELEASED
1731624044100548096: WRITE LOCK ACQUIRED
1731624044100549120: WRITE LOCK RELEASED
1731624044100558592: DELETE AWAKENED
1731624044100572160: WRITE LOCK ACQUIRED
1731624044100572672: DELETE, 3449751099, Sid Meier
1731624044100573440: WRITE LOCK RELEASED
1731624044100581632: WRITE LOCK ACQUIRED
1731624044100581888: DELETE, 2500551319, Richard Garriot
1731624044100582656: WRITE LOCK RELEASED
Number of lock acquisitions: 10
Number of lock releases: 10
448054155, Gabe Newell, 49000
660427450, Carol Shaw, 41000
909366975, Roberta Williams, 45900
1874280167, Shigeru Miyamoto, 51000
2569965317, Hideo Kojima, 45000
```

## 🤝 Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## 📜 License

This project is licensed under the MIT License.
