# File Explorer

A fully functional `Terminal-based File Explorer Application` with restricted feature set.

This application works in two modes :

- Normal Mode
- Command Mode

## Normal Mode

It is the default mode of the application, where the application starts. It's main functionality is to explore the current directory and navigate the filesystem.

Following are the details regarding the normal mode of the application :

- List of `Directories and Files` in the current folder where the application is started from is displayed , each in every new line with following details -
  - File/Directory Name
  - File/Directory Size
  - Ownership (User and Group)
  - Permissions
  - Date of Last Modification
- Entries for `Current and Parent directory` { "." and ".." } are also displayed with same details.
- Vertical Navigation in the application is done using respective `Up and Down` arrow keys.
- In case of vertical overflow, navigation is done using `k and l` keys.
- Open Directories or Files by pressing `Enter` key :
  - In case of Directory, application navigates to selected directory displaying contents and underlying details.
  - In case of a file, it is opened in the vi editor.
- Traversal in the application can be done as following:
  - `Left Arrow Key` : Takes the user to the previously visited directory.
  - `Right Arrow Key` : Takes the user to the next visited directory.
  - `Backspace` : Takes the user up one level in the directory tree of the filesystem.
  - `h` : Pressing "h" anytime in the application will take the user to the home directory which is considered to be the one where application is started from.
- To quit from the application User should press `q` key anytime

## Command Mode

This mode is used to enter different shell commands. Anytime in the application, User needs to press `Colon` { ":" } key which would then enable the user to enter commands in the status bar - at the last line (bottom) of the terminal screen.

Following are the details regarding the command mode of the application :

- Create File : `create_file <file_name> <destination_path>`
- Delete File : `delete_file <file_path>`
- Create Directory : `create_dir <dir_name> <destination_path>`
- Delete Directory : `delete_dir <dir_path>`
- Rename File : `rename <old_filename> <new_filename>`
- Copy files or directories : `copy <file(s)/dir_path> <destination_path>`
- Move files or directories : `move <file(s)/dir_path> <destination_path>`
- Goto : `goto <directory_path>`
  - This command would take the user back to Normal Mode inside the specified path/location.
- Search : `search <file/dir_name>`
  - This command would return `True` or `False` which depends on whether file/directory exists or not.
- `Backspace key` : In case of typing errors, user can press backspace key to delete and rectify the command.
- `TAB key` : Pressing "tab" key in the command mode will switch the user back to Normal Mode.
- `Quit` : When user needs to quit the application from command mode, `q` can be pressed in case there is no command typed. To force quit anytime `Shift + q` can be pressed which would terminate the application instantly.

### Notes :

1.  The root of the application is considered as : /Users/\<username> (which has been hardcoded). Before compiling, username has to be changed for the respective machine.
2.  In case of Absolute Paths "./\<path>" is being replaced with further level of directories to find the exact path required.
3.  To handle Relative Paths "~/" is being replaced with the root of the application as mentioned above to handle all the paths present and accessible to the logged user.
4.  It has been tried to make this application interactive, providing user needful directions and/or relevant messages.

#### Enjoy..!

---
