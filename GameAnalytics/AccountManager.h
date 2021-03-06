#pragma once
#include <vector>
#include <Windows.h>

#include "AccountInfo.h"

using namespace std;

#define AM AccountManager::getInstance()

enum class ECreationResponse
{
	NAME_MATCH,
	OVERLOAD,
	OK,
};

enum class EDeletionResponse
{
	NO_NAME,
	BAD_PASSWORD,
	OK,
};

/*
* class AccountManager
*
* singleton that initialises account information from a .text file
* into an array of objects, updates them at realtime and them saves
* the modified accounts back to the .text file
*
* @author: Bradley Booth, Academy of Interactive Entertainment, 2018
*/
class AccountManager
{
private:

	/*
	* AccountManager()
	* constructor, loads accounts from text
	*/
	DLL_EXP AccountManager();

	/*
	* ~AccountManager()
	* destructor, saves accounts using file i/o
	*/
	DLL_EXP ~AccountManager();

public:

	//list of accounts loaded
	vector<AccountInfo*> accounts;

	/*
	* getInstance
	*
	* gets a pointer to the only instance of the game analytics object,
	* creates the object if it doesn't already exist
	*
	* @returns static AccountManager* - pointer to the singleton
	*/
	DLL_EXP static AccountManager* getInstance();

	/*
	* getExecutableFolder
	*
	* gets the path that the .exe running from resides in
	*
	* @returns string - path to the root folder containing the .exe
	*/
	DLL_EXP string getExecutableFolder();

	/*
	* save
	*
	* writes all updated AccountInfo objects back 
	* to the file and then deallocates them
	* 
	* THIS SHOULD BE CALLED BEFORE THE APPLICATION ENDS
	* @returns void
	*/
	DLL_EXP void save();

	/*
	* createAccount
	*
	* creates a new account with the specified
	* username and password permenantly
	*
	* @param string name - the name of the new account
	* @param string pass - the password of the new account
	* @returns ECreationResponse - event return indicating the outcome of the attempted creation
	*/
	DLL_EXP ECreationResponse createAccount(string name, string pass);

	/*
	* removeAccount
	*
	* removes an existing account with the name and password
	*
	* @param string name - the name of the new account
	* @param string pass - the password of the new account
	* @returns EDeletionResponse - event return indicating the outcome of the attemtped deletion
	*/
	DLL_EXP EDeletionResponse removeAccount(string name, string pass);

	/*
	* searchAccount
	*
	* searches for an account and returns the result
	*
	* @param string name - the name of the account
	* @param string pass - the password of the account
	* @returns AccountInfo* - account info object, can be nullptr, indicating a failed search
	*/
	DLL_EXP AccountInfo* searchAccount(string name, string pass);
};