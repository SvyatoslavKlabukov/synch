//Ќаписать программу, котора€ будет синхронизировать два каталога : каталог - источник и каталог - реплику.
//«адача программы Ц приводить содержимое каталога - реплики в соответствие содержимому каталога - источника.
//“ребовани€ :
//Х —ихронизаци€ должна быть односторонней : после завершени€ процесса синхронизации содержимое каталога - реплики 
//  должно в точности соответствовать содержимому каталогу - источника;
//Х —инхронизаци€ должна производитьс€ периодически;
//Х ќперации создани€ / копировани€ / удалени€ объектов должны логироватьс€ в файле и выводитьс€ в консоль;
//Х ѕути к каталогам, интервал синхронизации и путь к файлу логировани€ должны задаватьс€ параметрами командной строки при запуске программы.

#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <string>

void updateFile(std::string source, std::string repeat, std::fstream &fs);//создание и копирование файлов в каталог-реплике
void removeFile(std::string source, std::string repeat, std::fstream &fs);//удаление файлов в каталог-реплике

int main()
{
	std::cout << "Enter the path to the source folder"
		"(for example: C://test//veeam):" << std::endl;
	std::string path_source;
	std::cin >> path_source;
	std::cout << "Enter the path to the repeat folder"
		"(for example: C://test//synch):" << std::endl;
	std::string path_repeat;
	std::cin >> path_repeat;
	std::cout << "Enter the path to the log file"
		"(for example: C://test//log.txt):" << std::endl;
	std::string log_file;
	std::cin >> log_file;
	std::cout << "Enter the synchronization interval in seconds:" << std::endl;
	int seconds;
	std::cin >> seconds;

	//логирование в файл
	std::fstream fs;
	fs.open(log_file, std::ios::app);
	if (fs.is_open())
	{
		std::cout << "file-log is opened\n";
	}
	else
	{
		std::cout << "file-log is not opened\n";
	}

	//синхронизаци€
	while (true)
	{
		updateFile(path_source, path_repeat, fs);
		removeFile(path_source, path_repeat, fs);
		Sleep(1000 * seconds);
	}

	fs.close();
	
	return 0;

}

void updateFile(std::string source, std::string repeat, std::fstream &fs)
{
	//каталог-источник
	const std::filesystem::path& p20 = source;

	//каталог-реплика
	const std::filesystem::path& p21 = repeat;

	for (const auto& source : std::filesystem::directory_iterator(p20))
	{
		bool flag = true;
		for (const auto& repeat : std::filesystem::directory_iterator(p21))
		{
			std::filesystem::file_time_type time_file_source = last_write_time(source.path());
			std::filesystem::file_time_type time_file_repeat;
			if (source.path().filename() != repeat.path().filename())
			{
				continue;
			}
			else
			{
				time_file_repeat = last_write_time(repeat.path());
				if (time_file_source != time_file_repeat)
				{
					std::filesystem::remove(repeat.path());
					std::filesystem::copy(source, p21);
					std::cout << source.path().filename() << " COPIED" << std::endl;
					fs << source.path().filename() << " COPIED" << std::endl;
					flag = false;
					break;
				}
				else
				{
					flag = false;
					break;
				}
			}
		}
		if (flag)
		{
			std::filesystem::copy(source, p21);
			std::cout << source.path().filename() << " CREATED" << std::endl;
			fs << source.path().filename() << " CREATED" << std::endl;
		}
	}
}


void removeFile(std::string source, std::string repeat, std::fstream &fs)
{
	//каталог-источник
	const std::filesystem::path& p20 = source;

	//каталог-реплика
	const std::filesystem::path& p21 = repeat;

	for (const auto& repeat : std::filesystem::directory_iterator(p21))
	{
		bool flag = true;
		for (const auto& source : std::filesystem::directory_iterator(p20))
		{
			if (source.path().filename() != repeat.path().filename())
			{
				continue;
			}
			else
			{
				flag = false;
				break;
			}
		}
		if (flag)
		{
			std::cout << repeat.path().filename() << " DELETED" << std::endl;
			fs << repeat.path().filename() << " DELETED" << std::endl;
			std::filesystem::remove(repeat.path());
		}
	}
}