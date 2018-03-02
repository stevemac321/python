#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <cstring>
#include <algorithm>

struct verse {
	std::string name = "";
	std::string ref;
};
struct book {
	std::string name = "";
	std::vector<verse> verses;
};
bool is_blank(std::string &s)
{
	char *p = const_cast<char *>(s.c_str());
	if (p) {
		while (*p != '\0' && *p != '\n') {
			if (isalnum(*p))
				return false;

			++p;
		}
	}
	return true;
}
struct verseless {
	bool operator()(const verse &l, const verse &r) const
	{
		std::string left = l.name;
		std::string right = r.name;

		char *pl = const_cast<char *>(left.c_str());
		char *pr = const_cast<char *>(right.c_str());
		char *ls = pl;
		char *rs = pr;

		while (*pl != ':' && *pl != '\n')
			pl++;

		while (*pr != ':' && *pr != '\n')
			pr++;

		if (*pl == ':')
			*pl = '\0';

		if (*pr == ':')
			*pr = '\0';

		int il = atoi(ls);
		int ir = atoi(rs);

		if (il != ir)
			return il < ir;
		else {
			pl++;
			pr++;

			ls = pl;
			rs = pr;

			while (isdigit(*pl))
				++pl;

			while (isdigit(*pr))
				++pr;

			*pl = '\0';
			*pr = '\0';

			il = atoi(ls);
			ir = atoi(rs);

			return il < ir;
		}
	}
};
void parsesemicolon(std::vector<book> &v, std::string &chapter,
		    std::string &paragraph, std::string &footnote,
		    std::string &token)
{
	char *pcpy = const_cast<char *>(token.c_str());
	char *p = pcpy;
	char *pverse = nullptr;

	std::string book = "";

	while (*p != ':' && *p != '\n')
		++p;

	if (*p == ':') {
		while (!isspace(*(p - 1)) && (p - 1) > pcpy)
			--p;

		pverse = p;
		if (p > pcpy)
			--p;

		while (isspace(*p) && p > pcpy)
			--p;

		if (p > pcpy && !isspace(*p)) {
			*(p + 1) = '\0';
			book = pcpy;
			for (size_t i = 0; i < 66; i++)
				if (book == v[i].name) {
					verse vs;
					vs.name = pverse;
                                        vs.name.erase(remove_if(vs.name.begin(), vs.name.end(), isspace), vs.name.end());
					std::string ref("LBC ");
					ref += chapter;
					ref += ".";
					ref += paragraph;
					ref += ".";
					ref += footnote;
					vs.ref = ref;
					v[i].verses.push_back(vs);
				}
		}
	}
}
void parsehash(std::vector<book> &v, std::string &chapter,
	       std::string &paragraph, std::string &token)
{
	// get footnote number, verses
	std::regex re("(^[0-9]+) (.*)");
	std::smatch m;
	std::string footnote = "";
	std::string verses = "";

	if (std::regex_search(token, m, re)) {
		footnote = m[1];
		verses = m[2];
	}
	// tokenize on ;
	std::istringstream iss(verses);
	std::string semi;

	while (std::getline(iss, semi, ';'))
		parsesemicolon(v, chapter, paragraph, footnote, semi);
}
void parseline(std::vector<book> &v, std::string &chapter, std::string &line)
{
	std::regex rep("Paragraph ([0-9]+)");
	std::regex rev("\\((#[0-9]+.*?)\\)");
	std::string paragraph = "";
	std::string parens = "";
	std::smatch m;

	// find paragraph, set current paragraph
        if(strstr(line.c_str(), "Paragraph")) {
	        if (std::regex_search(line, m, rep))
		        paragraph = m[1];

	        // capture refs within parens
	        if (std::regex_search(line, m, rev))
		        parens = m[1];

                if(parens != "") {
	                // tokenize on #
	                std::istringstream iss(parens);
	                std::string token = "";

	                while (std::getline(iss, token, '#'))
		                parsehash(v, chapter, paragraph, token);
                }
        }
}
void parsefile(std::vector<book> &v, const std::string &filename)
{
	std::fstream fs(filename);
	std::regex re("Chapter ([0-9]+)");
	std::smatch m;
	std::string line = "";
	std::string chapter = "";

	if (fs.is_open()) {
		while (!fs.eof()) {
			std::getline(fs, line);
			if (!is_blank(line)) {
				if (std::regex_search(line, m, re)) 
					chapter = m[1];
                                else 
                                        if(chapter != "")
					        parseline(v, chapter, line);
                        }
		}
		fs.close();
	}
}

int main()
{
	const char *books[] = {"Genesis",
			       "Exodus",
			       "Leviticus",
			       "Numbers",
			       "Deuteronomy",
			       "Joshua",
			       "Judges",
			       "Ruth",
			       "1 Samuel",
			       "2 Samuel",
			       "1 Kings",
			       "2 Kings",
			       "1 Chronicles",
			       "2 Chronicles",
			       "Ezra",
			       "Nehemiah",
			       "Esther",
			       "Job",
			       "Psalm",
			       "Proverbs",
			       "Ecclesiastes",
			       "Song of Solomon",
			       "Isaiah",
			       "Jeremiah",
			       "Lamentations",
			       "Ezekiel",
			       "Daniel",
			       "Hosea",
			       "Joel",
			       "Amos",
			       "Obadiah",
			       "Jonah",
			       "Micah",
			       "Nahum",
			       "Habakkuk",
			       "Zephaniah",
			       "Haggai",
			       "Zechariah",
			       "Malachi",
			       "Matthew",
			       "Mark",
			       "Luke",
			       "John",
			       "Acts",
			       "Romans",
			       "1 Corinthians",
			       "2 Corinthians",
			       "Galatians",
			       "Ephesians",
			       "Philippians",
			       "Colossians",
			       "1 Thessalonians",
			       "2 Thessalonians",
			       "1 Timothy",
			       "2 Timothy",
			       "Titus",
			       "Philemon",
			       "Hebrews",
			       "James",
			       "1 Peter",
			       "2 Peter",
			       "1 John",
			       "2 John",
			       "3 John",
			       "Jude",
			       "Revelation"};

	std::vector<book> v(70);
	for (size_t i = 0; i < 66; i++)
		v[i].name = books[i];

	parsefile(v, "lbc.txt");

	for (size_t i = 0; i < 66; i++) {
		if (!v[i].verses.empty()) {
			std::sort(std::begin(v[i].verses),
				  std::end(v[i].verses), verseless());

			fprintf(stdout, "%s\n", v[i].name.c_str());

			for (auto i : v[i].verses)
                                fprintf(stdout, "%-*s %*s\n", 14, i.name.c_str(), 8, i.ref.c_str());
		}
	}
}
