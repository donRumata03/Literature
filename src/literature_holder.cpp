#include "pch.h"
#include "literature_holder.h"

vector<string> get_all_literature_paths(const string& base_path)
{
	auto authors_dirs = lsdir(base_path);
	// cout << authors_dirs.size() << endl;
	// cout << Slice(authors_dirs, 10, 20) << endl;

	vector<string> res;
	for (auto& author_dir : authors_dirs)
	{
		// cout << "Processing " << author_dir << " : " << endl;
		vector<string> author_dirs;
		try {
			author_dirs = lsdir(author_dir);
		}
		catch(...)
		{
			continue;
		}
			// cout << "Lsdir: " << author_dirs <<  endl;
		for (auto& artwork_dir : author_dirs)
		{
			// cout << "Artwork: " << artwork_dir << endl;
			try {
				string filename = "text.txt";
				string this_path = path_join(artwork_dir, filename);
				// cout << this_path << " ";
				bool ex = fs::exists(this_path);
				// cout << std::boolalpha << ex << endl;
				if (ex) res.push_back(this_path);
			}
			catch (...)
			{
				cout << "Error!" << endl;
			}
			// res.push_back(path_join());
			// cout << "Next artwork!" << endl;
		}
	}
	
	return res;
}
