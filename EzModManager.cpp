// ror 2 turbo mod manager.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//
// konsola 30 linijek
// konsola 120 znakow w linijce
//
// system("CLS");
// <<std::flush

#include <iostream>
//#include <urlmon.h> //do pobrania database || URLDownloadToFile()
//#pragma comment(lib, "urlmon.lib")  //Needed for the URLDownloadToFile() function
#include <fstream> //użyłem systemowych komend zamiast tego :/ //still ważne do innych rzeczy
//#include <cstdlib> //podobno potrzebne do system(); ale działa bez tego :/
#include "json.hpp" //https://github.com/nbsdx/SimpleJSON
#include <vector>
#include <string>
#include <filesystem>
#include "zip_file.hpp" //https://github.com/tfussell/miniz-cpp
// check this shit out https://github.com/troldal/Zippy <<<< ABSOLUTE GARBAGE
#include <vector> // in void downloadMod()  using to keep track of already downloaded mods
#include <algorithm> // std::find //for vector bullshittery :/
//#include <ctime> // do nazwy pliku (zeby byl czas)
//używam systemowego curl  req.Windows 10 build 1803 (released early May 2018)
//używam systemowego tar req..Windows 10 1903 from build 17063 or later
#include <windows.h>
#include <stdlib.h>

using namespace std;
using json::JSON;
//using json = nlohmann::json;

int main();

void drawLogo();
void downloadDatabase();
string removeDots(string& x)
{
	auto it = std::remove_if(std::begin(x), std::end(x), [](char c) {return (c == '.');});
	x.erase(it, std::end(x));
	return x;
}
string removeNumbers(string& x) // usuwa jeszcze "-" i "_"
{
	auto it = std::remove_if(std::begin(x), std::end(x), [](char c) {return (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9') || (c == '0') || (c == '-') || (c == '_');});
	x.erase(it, std::end(x));
	return x;
}
int versionCompare(string v1, string v2)
{
	// 1 if v2 is smaller, 
	// -1 if v1 is smaller
	// 0 if equal


	// vnum stores each numeric
	// part of version
	int vnum1 = 0;
	int vnum2 = 0;

	// loop until both string are
	// processed
	for (int i = 0, j = 0; (i < v1.length() || j < v2.length());)
	{
		// storing numeric part of
		// version 1 in vnum1
		while (i < v1.length() && v1[i] != '.') {
			vnum1 = vnum1 * 10 + (v1[i] - '0');
			i++;
		}

		// storing numeric part of
		// version 2 in vnum2
		while (j < v2.length() && v2[j] != '.') {
			vnum2 = vnum2 * 10 + (v2[j] - '0');
			j++;
		}

		if (vnum1 > vnum2)
			return 1;
		if (vnum2 > vnum1)
			return -1;

		// if equal, reset variables and
		// go for next numeric part
		vnum1 = vnum2 = 0;
		i++;
		j++;
	}
	return 0;
}
void downloadMod(string url, vector<string> &myvector);
void installMods(string pluginPath);
void downloadDependencies(string modName, JSON database, vector<string> &TurboVector);

bool IWantToKillMySelfCuzTheresNoGoodZipLibForCPP = false; // controls whether use tar.exe (available since Windows 10 (1903) from build 17063 or later.) or shitty zip library i found :/
														   // true = use shitty zip lib; 
														   // false = use tar.exe (recommended)

// TODO: Nazwy zmiennych dzbanie :/

int main()
{
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	vector<string> InstalledMods;

	drawLogo();
	downloadDatabase();
	

	// ------- LOAD JSON -------
	cout << "Loading database.." << endl;
	


	fstream DatabaseFile("./database.json");
	string content;
	getline(DatabaseFile, content, '\0'); //wrzuc plik do stringa content
	JSON Database;
	Database = JSON::Load(content);

	//cout << Database;
	SetConsoleTextAttribute(hConsole, 2); // 2 - green  4 - red  15 - white
	cout << "Loaded!" << endl;
	SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
	
										   //cout << j[0]["versions"][0]["version_number"] << endl;
	//dla shit.json ^^

	// ------- LOAD JSON KONIEC -------


	// ------- LOAD PREP -------
	string PathToRor;
	fstream ConfigFile("./config.txt");
	string lmao;
	getline(ConfigFile, lmao);
	//cout << lmao << "lmao" << endl;
	if (lmao == "")
	{
		cout << "Path to RoR2 Folder: "; //  E:\Games\Risk of Rain 2 Anniversary
		getline(cin, PathToRor);
		ofstream config("./config.txt");
		config << PathToRor;
		config.close();
	}
	else
	{
		cout << "Used path to ror from config file" << endl;
		PathToRor = lmao;
	}
	string PathToPlugins = PathToRor + "\\BepInEx\\plugins";
	//cout << PathToRor << "path" << endl;;
	ConfigFile.close();


	std::filesystem::remove_all("temp");
	std::filesystem::create_directory("temp");
	// ------- LOAD PREP KONIEC-------
	
	
	int wybor;

	SetConsoleTextAttribute(hConsole, 4); // 2 - green  4 - red  15 - white
	cout << "Uwaga!" << endl;
	cout << "Musisz recznie zainstalowac: R2API, BepInExPack, HookGenPatcher" << endl;
	SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
	cout << "I pewnie mase innych..." << endl;
	cout << "btw po instalacji paczki odpal managera jeszcze raz i zupdatuj zainstalowane mody" << endl;
	cout << endl << "Czego ci trzeba:" << endl;
	cout << "1. Update currently installed mods." << endl; // downloads to ./temp  || (doesn't download dependencies, if there are any new :/)
	cout << "2. Install mods from link." << endl;
	cout << "3. Install mods from modlist." << endl; 
	cout << "4. Create modlist." << endl;
	cout << "5. Download (not install) R2API and its modules." << endl;
	cout << endl;
	cin >> wybor;

	
	switch (wybor)
	{
		case 1:
		{
			int ModCount = 0;
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(PathToPlugins))  // znajduje wszystkie pliki
			{
				if (dirEntry.path().extension() == ".json") // znajduje wszystkie manifest.json
				{
					//cout << dirEntry.path() << endl;
					JSON manifestJSON;
					string manifestSTR = "";
					fstream manifestFILE(dirEntry.path());
					getline(manifestFILE, manifestSTR, '\0'); // ´╗┐ (\xEF\xBB\xBF)  dodaje sie na początku plików UTF-8 | zmień kodowanie na UTF-8 bez BOM

					//------ COPE Z BOM
					string fuckingBOM = "\xEF\xBB\xBF";
					if (manifestSTR.find(fuckingBOM) != string::npos)
					{
						manifestSTR.erase(0, 3);
					}
					//------ KONIEC COPE Z BOM

					manifestJSON = JSON::Load(manifestSTR); // manifest.json załadowany jako JSON

					for (auto& lmao : Database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
					{

						//cout << lmao["name"] << endl;
						if (manifestJSON["name"].ToString() == lmao["name"].ToString()) // przy porównaniu musisz dać .ToString() bo tak :/ || JA ***** NIEKTORE MODY MAJA TAKIE SAME NAZWY A SA INNE 
						{
							for (auto& superZabawa : lmao["versions"].ArrayRange())
							{
								//cout << superZabawa << endl; //"description"
								if (superZabawa["description"].ToString() == manifestJSON["description"].ToString()) // sprawdza czy jakikolwiek opis wersji zgadza sie z tym zapisanym na kompie bo JA ***** NIEKTORE MODY MAJA TAKIE SAME NAZWY A SA INNE 
								{
									if (lmao["is_deprecated"].ToBool() == true) { break; } //sprawdza czy mod jest depracated | jezeli tak to skipuje sprawdzanie go
									bool boolshit = false;
									for (int categoryIndex = 0; categoryIndex < lmao["categories"].length(); categoryIndex++) {
										if (lmao["categories"][categoryIndex].ToString() == "Modpacks") {
											boolshit = true;
										}
									}
									if (boolshit) { break; }
									//cout << superZabawa << endl;
									string tempVersion = lmao["versions"][0]["version_number"].ToString();
									string DatabaseVersion = removeDots(tempVersion);	// pobiera wersje moda z database i usuwa kropki

									string manifestVersion = manifestJSON["version_number"].ToString();
									string LocalVersion = removeDots(manifestVersion);
									//cout << manifestJSON["name"].ToString() << " wersja database: " << DatabaseVersion << "  Local version: " << LocalVersion << " URL: " << lmao["package_url"].ToString()<<endl;

									int twojstary = versionCompare(tempVersion, manifestVersion);

									if (twojstary == 1) //stoi(string) konwertuje stringa na inta | stod na double | stof na floata
									{
										//cout << manifestJSON["name"] << " jest outdated. Twoja wersja to: "<<LocalVersion<<" A najnowsza z database to: "<<DatabaseVersion <<endl;
										downloadMod(lmao["versions"][0]["full_name"].ToString(),InstalledMods);
										ModCount++;
									}

									break; // jestesmy w petli z samymi wersjami a nie chcemy odpalac powyzszego kodu dla kazdej wersji kazdego moda, tylko raz | wiec uzywamy break po pierwszym uzyciu kodu
								}

							}
							//cout << "match found for: " << manifestJSON["name"] << endl;

						}
					}

				}
			}
			cout << endl << endl << "Updated " << ModCount << " mods" << endl;
			cout << "installing mods.." << endl;	// TAR wymaga win10 1903 (build 17063) | znajdz inną metode czy cos/ znalezione es
		
			//for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("./temp/"))  // znajduje wszystkie pliki
			//{
			//	//cout << dirEntry.path() << endl;

			//	// tar -xf {zipDir} -C {pathToPlugin} //chyba xd
			//}
		
			installMods(PathToPlugins);
			SetConsoleTextAttribute(hConsole, 2); // 2 - green  4 - red  15 - white
			cout << "installed mods :)" << endl;
			SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
			break;
		}
		case 2:
		{
			string userURL, lulURL;
			cout << "Podaj link (np. https://thunderstore.io/package/ontrigger/ItemStatsMod/): " << endl;
			//getline(cin, userURL);
			cin >> userURL;
		
			//cout << lulURL << endl;
			for (auto& lmao : Database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
			{
				lulURL = lmao["package_url"].ToString();
				lulURL.resize(lulURL.size() - 1); // dzieki temu dziala jak uzytkownik da link bez slasha na końcu
				if (lmao["package_url"].ToString() == userURL || lulURL == userURL)
				{
					downloadMod(lmao["versions"][0]["full_name"].ToString(),InstalledMods);
					downloadDependencies(lmao["full_name"].ToString(), Database, InstalledMods);
				}

			}
			//cout << "lmao" << endl; https://thunderstore.io/package/EnigmaDev/EnigmaticThunder/
			installMods(PathToPlugins);
			SetConsoleTextAttribute(hConsole, 2); // 2 - green  4 - red  15 - white
			cout << "mod installed :)" << endl;
			SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
			break;
		}
		case 3: 
		{
			string TurboFile;
			cout << endl << "Filename modlisty (z .txt): " << endl<<endl;
			//getline(cin, TurboFile);
			cin >> TurboFile;
			fstream MyModList(TurboFile);
			string tempModLine,lulURL;
			int LineCount=0;
			while (getline(MyModList, tempModLine)) 
			{
				LineCount++;
				if (tempModLine.find(";")) 
				{
					for (auto& lmao : Database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
					{
						lulURL = lmao["package_url"].ToString();
						lulURL.resize(lulURL.size() - 1); // dzieki temu dziala jak uzytkownik da link bez slasha na końcu
						if (lmao["package_url"].ToString() == tempModLine || lulURL == tempModLine)
						{
							//cout << lmao["versions"][0]["full_name"].ToString() << endl;
							downloadMod(lmao["versions"][0]["full_name"].ToString(), InstalledMods);
							downloadDependencies(lmao["full_name"].ToString(), Database, InstalledMods);
						}

					}
				}
				else 
				{
					//cout << "DEBUG: Comment found on line: " << LineCount << endl;
				}
			}
		
		
			cout << "installing mods.." << endl;
			installMods(PathToPlugins);
			SetConsoleTextAttribute(hConsole, 2); // 2 - green  4 - red  15 - white
			cout << "Mods installed!" << endl;
			SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
			break;
		}
		case 4: 
		{
			int modcount=0;
			string modfile = "";
			cout << "Filename modlisty: " << endl;
			cin >> modfile;
			cout << endl;
			//
			string lmao= modfile.substr(modfile.size() - 4); //4 ostanie znaki; użyte do sprawdzenia czy użytkownik wpisał z ".txt" nazwe pliku
			//
			if (lmao != ".txt") 
			{
				modfile = modfile + ".txt";
			}
			cout << "Generating modlist to: " << modfile << endl << endl;
			SetConsoleTextAttribute(hConsole, 4); // 2 - green  4 - red  15 - white
			cout << "Nie przejmuj sie tymi paroma errorami dx" << endl;
			SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
			std::ofstream ofs(modfile);
			ofs << ";Lines with >;< are comments\n";
			ofs << ";In this file you can put links to mods to downloadand install them in batch\n";
			ofs << ";example link : https://thunderstore.io/package/XoXFaby/BetterUI/\n";
			ofs << ";each new mod in new line\n\n";
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(PathToPlugins))  // znajduje wszystkie pliki
			{
				if (dirEntry.path().extension() == ".json") // znajduje wszystkie manifest.json
				{
					//cout << dirEntry.path() << endl;
					JSON manifestJSON;
					string manifestSTR = "";
					fstream manifestFILE(dirEntry.path());
					getline(manifestFILE, manifestSTR, '\0'); // ´╗┐ (\xEF\xBB\xBF)  dodaje sie na początku plików UTF-8 | zmień kodowanie na UTF-8 bez BOM

					//------ COPE Z BOM
					string fuckingBOM = "\xEF\xBB\xBF";
					if (manifestSTR.find(fuckingBOM) != string::npos)
					{
						manifestSTR.erase(0, 3);
					}
					//------ KONIEC COPE Z BOM

					manifestJSON = JSON::Load(manifestSTR); // manifest.json załadowany jako JSON

					for (auto& lmao : Database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
					{

						//cout << lmao["name"] << endl;
						if (manifestJSON["name"].ToString() == lmao["name"].ToString()) // szuka znaleziony mod w database
						{
							if (lmao["name"].ToString() == "R2API" || 
								lmao["name"].ToString() == "HookGenPatcher") { break; } //skip r2api etc..
							// skipowanie hookgen chyba useless dx

							for (auto& superZabawa : lmao["versions"].ArrayRange())
							{
								//cout << superZabawa << endl; //"description"
								if (superZabawa["description"].ToString() == manifestJSON["description"].ToString()) // sprawdza czy jakikolwiek opis wersji zgadza sie z tym zapisanym na kompie bo JA ***** NIEKTORE MODY MAJA TAKIE SAME NAZWY A SA INNE 
								{

									if (find(InstalledMods.begin(), InstalledMods.end(), lmao["name"].ToString()) == InstalledMods.end()) // == myvector.end() oznacza że find() przeleciał przez cały wektor i nie znalazł modName
									{
										InstalledMods.insert(InstalledMods.begin(), lmao["name"].ToString());
										modcount++;
									
										//cout << lmao["package_url"].ToString() << endl;
										
										ofs << lmao["package_url"].ToString() + "\n";
									}

									
									break; // jestesmy w petli z samymi wersjami a nie chcemy odpalac powyzszego kodu dla kazdej wersji kazdego moda, tylko raz | wiec uzywamy break po pierwszym uzyciu kodu
								}
							}
							
						}
					}
				}
			}
			SetConsoleTextAttribute(hConsole, 2); // 2 - green  4 - red  15 - white
			cout << "Ilosc modow to: " <<modcount << endl;
			SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
			ofs.close();
			break;
		}
		case 5: 
		{
			for (auto& lmao : Database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
			{
				if (lmao["full_name"].ToString() == "tristanmcpherson-R2API") {
					downloadMod(lmao["versions"][0]["full_name"].ToString(), InstalledMods);
					downloadDependencies(lmao["full_name"].ToString(), Database, InstalledMods);
				}
				
			}
			break;
		}
			
		case 20: 
		{
			installMods(PathToPlugins);
			break;
		}
		case 21:
		{
			string lmao= Database[467]["full_name"].ToString();
			downloadDependencies(lmao,Database, InstalledMods);
			break;
		}
		case 22: 
		{
			string content = "";

			cout << endl << "Loading mods from MyModList.txt" << endl << endl;
			fstream MyModList("MyModList.txt");
			string tempModLine, lulURL;
			int LineCount = 0;
			while (getline(MyModList, tempModLine))
			{
				LineCount++;
				if (tempModLine.find(";"))
				{
					for (auto& lmao : Database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
					{
						lulURL = lmao["package_url"].ToString();
						lulURL.resize(lulURL.size() - 1); // dzieki temu dziala jak uzytkownik da link bez slasha na końcu
						if (lmao["package_url"].ToString() == tempModLine || lulURL == tempModLine)
						{
							//cout << lmao["is_deprecated"].ToBool() << endl;
							if (lmao["is_deprecated"].ToBool() == 1) {
								cout << lmao["full_name"].ToString() << " is deprecated" << endl;
								//content = content + lmao["full_name"].ToString() = "\n";
							}
							else
							{
								//cout << lmao["full_name"].ToString() << " not deprecated" << endl;
							}
						}


					}
				}
				else
				{
					//cout << "DEBUG: Comment found on line: " << LineCount << endl;
				}
			}
			cout << content << endl;
			time_t now = time(0);
			std::string filename = std::to_string(now) + ".txt";
			std::ofstream ofs(filename);
			ofs << content;
			ofs.close();
			break;
		}
		default: 
		{
			break;
		}
		
	}

	std::cout << "Press enter to exit...";
	std::getchar();
	std::getchar();
}

void drawLogo() // https://patorjk.com/software/taag  między słowami 4 spacje || doom(pewnie zmienie pozniej)
{
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 4); // 2 - green  4 - red  15 - white
	cout << flush;
	system("CLS");
	string logo[8];

	logo[0] = R"(   _____   ___  ___          _                                             )";
	logo[1] = R"(  |  ___|  |  \/  |         | |                                            )";
	logo[2] = R"(  | |__ ___| .  . | ___   __| |_ __ ___   __ _ _ __   __ _  __ _  ___ _ __ )";
	logo[3] = R"(  |  __|_  | |\/| |/ _ \ / _` | '_ ` _ \ / _` | '_ \ / _` |/ _` |/ _ | '__|)";
	logo[4] = R"(  | |___/ /| |  | | (_) | (_| | | | | | | (_| | | | | (_| | (_| |  __| |   )";
	logo[5] = R"(  \____/___\_|  |_/\___/ \__,_|_| |_| |_|\__,_|_| |_|\__,_|\__, |\___|_|   )";
	logo[6] = R"(                                                            __/ |          )";
	logo[7] = R"(                                                           |___/           )";

	for (int i = 0; i < 8;i++)
	{
		cout << logo[i] << endl;
	}
	SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
}


void downloadDatabase() // https://thunderstore.io/api/v1/package/
{
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


	cout << "Downloading Database.." << endl;
	string myurl = "https://thunderstore.io/api/v1/package/";
	//string mypath = R"("E:\nwm\wtf code\EzModManagerRoR2\EzModManagerBattleground\database.json")";
	string mypath = "database.json";
	string str = "curl -# -L " + myurl + " >> " + mypath;
	
	const char *command = str.c_str();
	
	cout << flush;
	
	//cout << deletecommand<<endl;
	std::filesystem::remove("database.json");
	system(command);

	SetConsoleTextAttribute(hConsole, 2); // 2 - green  4 - red  15 - white
	cout << "Database Downloaded!"<<endl;
	SetConsoleTextAttribute(hConsole, 15); // 2 - green  4 - red  15 - white
}

void downloadMod(string modName, vector<string> &myvector) // modName = database["versions"][0]["full_name"]
{
	std::vector<std::string>::iterator it;
	it = myvector.begin();
	//myvector.insert(myvector.begin(), modName);

	//if (modName.find("tristanmcpherson-R2API") || modName.find("bbepis-BepInExPack")) { return; }
	string elobenc = "https://cdn.thunderstore.io/live/repository/packages/" + modName + ".zip";
	string command = "curl -# -L " + elobenc + " -o temp/" + modName + ".zip";

	if (find(myvector.begin(), myvector.end(), modName) == myvector.end()) // == myvector.end() oznacza że find() przeleciał przez cały wektor i nie znalazł modName
	{
		myvector.insert(myvector.begin(), modName);
		//cout << "frajer znaleziony: " << modName << endl;
		cout << "Downloading mod: " << modName << endl;
		system(command.c_str());
	}	

}

void downloadDependencies(string modName,JSON database,vector<string> &TurboVector) //modname = full_name
{
	if (modName == "") { return; }
	//if (modName.find("tristanmcpherson-R2API") || modName.find("bbepis-BepInExPack")) { return; }
	//cout << "current mod: " << modName << endl;
	for (auto& lmao : database.ArrayRange()) // przechodzi przez każde value w JSON database | dostęp poprzez lmao["name"]...
	{
		//cout << modName << "   " << lmao["full_name"].ToString() << endl;
		if (modName == lmao["full_name"].ToString()) 
		{
			for (auto& j : lmao["versions"][0]["dependencies"].ArrayRange())
			{
				
				if  ( //not every mod is normal :/
					(j.ToString().find("bbepis-BepInExPack") != string::npos) || 
					(j.ToString().find("tristanmcpherson-R2API") != string::npos) || 
					(j.ToString().find("RiskofThunder-HookGenPatcher") != string::npos) 
					)
				{
					//cout << "ten mod zostal skipniety: " << j.ToString() << endl;
				}
				else 
				{ 
					downloadMod(j.ToString(), TurboVector);
				}
				
			}
		}
	}
}

void installMods(string pluginPath)
{
	//usuwanie starych modów
	//cout << "installing mods lmao" << endl;

	for (const auto& dirEntry : std::filesystem::directory_iterator("./temp")) 
	{
		if (dirEntry.path().extension() == ".zip") 
		{
			//cout << dirEntry.path().extension() << endl;
			for (const auto& lmaoentry : std::filesystem::directory_iterator(pluginPath))
			{
				string OldModName = lmaoentry.path().filename().string();
				//cout << OldModName << endl;
				OldModName = removeDots(OldModName);
				OldModName = removeNumbers(OldModName); // usuwa liczby i jeszcze "-" i "_"
				string NewModName = dirEntry.path().filename().string();
				NewModName = removeDots(NewModName);
				NewModName = removeNumbers(NewModName);
				NewModName.resize(NewModName.size() - 3); // usuwa 3 ostatnie znaki (w naszym przypadku "zip") (kropki usunelismy w poprzednik kroku)
				//cout << OldModName << "   " << NewModName <<endl;
				
				if (OldModName == NewModName)
				{
					//cout << "pls delet: " << OldModName <<"  " << lmaoentry.path() <<endl;
					std::filesystem::remove_all(lmaoentry.path());
				}
			}
		//instalacja nowych
			if (IWantToKillMySelfCuzTheresNoGoodZipLibForCPP == true)
			{
				miniz_cpp::zip_file ModZip(dirEntry.path().string());
				//cout << dirEntry.path() << endl;
				string modPath = pluginPath + "/" + dirEntry.path().filename().string();
				modPath.resize(modPath.size() - 4); // usuwa 4 ostatnie znaki (w naszym przypadku ".zip")
				//cout << modPath << endl << endl;
				std::filesystem::create_directory(modPath);
				ModZip.extractall(modPath);
			}
			else // using tar.exe
			{
				string modPath = pluginPath + "/" + dirEntry.path().filename().string();
				modPath.resize(modPath.size() - 4); // usuwa 4 ostatnie znaki (w naszym przypadku ".zip")
				std::filesystem::create_directory(modPath);
				modPath = "\"" + modPath + "\"";
				string tarCommand ="tar -xf " + dirEntry.path().string() + " -C " + modPath;
				//cout << tarCommand << endl;
				system(tarCommand.c_str());
			}
		}
	}
}